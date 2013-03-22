/*
 * QEnmlWriter.cpp
 *
 *  Created on: May 20, 2010
 *      Author: alexr
 */

#include "QEnmlWriter.h"
#include <QRegExp>
#include "md5.h"
#include <QDebug>

static QString htmlElements[] = {
		"tag_image",
		"tag_audio",
		"tag_pdf",
		"tag_ink",
		"tag_app",
		"tag_crypt",
		"tag_todo",
		NULL
};

static QString enmlMediaExt[] = {
		"gif",
		"jpg",
		"png",
		"wav",
		"mp3",
		"amr",
		"pdf",
		"ink",
		NULL
};

static QString enmlMediaTypes[] = {
		"image/gif",
		"image/jpeg",
		"image/png",
		"audio/wav",
		"audio/mpeg",
		"audio/amr",
		"application/pdf",
		"application/vnd.evernote.ink",
		"application/octet-stream",
		NULL
};

bool QHtmlContentHandler::startDocument()
{
	m_sOutputEnml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" \
	"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">";

	return true;
}

QHtmlContentHandler::HtmlElement QHtmlContentHandler::getHtmlTag(const QString& fileName, QString& htmlTag)
{
	int idx = 0;

	while(enmlMediaExt[idx] != NULL)
	{
		if(fileName.endsWith(enmlMediaExt[idx]))
		{
			if(idx < 3)
			{
				htmlTag = htmlElements[HTML_IMG];
				return HTML_IMG;
			}
			if(idx < 6)
			{
				htmlTag = htmlElements[HTML_AUDIO];
				return HTML_AUDIO;
			}
			if(idx == 6)
			{
				htmlTag = htmlElements[HTML_PDF];
				return HTML_PDF;
			}
			if(idx == 7)
			{
				htmlTag = htmlElements[HTML_INK];
				return HTML_INK;
			}

			htmlTag = htmlElements[HTML_APP];
			return HTML_APP;
		}

		idx++;
	}

	htmlTag = htmlElements[HTML_APP];
	return HTML_APP;
}

QString QHtmlContentHandler::getContentType(const QString& contentSource)
{
	int idx = 0;

	while(enmlMediaExt[idx] != NULL)
	{
		if(contentSource.endsWith(enmlMediaExt[idx]))
		{
			return enmlMediaTypes[idx];
		}

		idx++;
	}

	return "application/octet-stream";
}

bool QHtmlContentHandler::startElement( const QString & namespaceURI, const QString & localName, const QString & , const QXmlAttributes & atts )
{
	flushBuffer();
	m_currentTag = localName;
	//qDebug() << __FUNCTION__ << localName;
	switch(decodeHtmlElement(localName)) {
	case HTML_CRYPT:
		return openCryptTag(atts);
	case HTML_TODO:
		return openTodoTag(atts);
	case HTML_STANDARD:
		return openHtmlTag(localName, atts);
	default:
		return openMediaTag(atts);
	}
}

bool QHtmlContentHandler::endElement ( const QString & namespaceURI, const QString & localName, const QString &  )
{
	flushBuffer();
	//qDebug() << __FUNCTION__ << localName;
	switch(decodeHtmlElement(localName)) {
	case HTML_CRYPT:
	case HTML_TODO:
		return closeTodoTag();
	case HTML_STANDARD:
		return closeEnmlTag(localName);
	default:
		return closeMediaTag(localName);
	}
}

QHtmlContentHandler::HtmlElement QHtmlContentHandler::decodeHtmlElement(const QString& elemName)
{
	int idx = 0;

	while(htmlElements[idx] != NULL)
	{
		if(elemName == htmlElements[idx])
			return (HtmlElement)idx;

		idx++;
	}

	return HTML_STANDARD;
}

bool QHtmlContentHandler::openHtmlTag(const QString& tagName, const QXmlAttributes& atts)
{
	if(tagName == "html" || tagName == "head" || tagName == "meta" || m_bEnmlTagOpened)
		return true;

	if(tagName == "body")
	{
		m_sOutputEnml += "<en-note";
	}
	else
	{
		m_sOutputEnml += "<" + tagName;
	}

	for(int i = 0; i < atts.length(); i++)
	{
		m_sOutputEnml += " " + atts.localName(i) + "=\"" + atts.value(i) + "\"";
	}

	m_sOutputEnml += ">";

	return true;
}

bool QHtmlContentHandler::openCryptTag(const QXmlAttributes& atts)
{
	return openHtmlTag("en-crypt", atts);
}

bool QHtmlContentHandler::openTodoTag(const QXmlAttributes& atts)
{
	openHtmlTag("en-todo", atts);
	closeEnmlTag("en-todo");
	m_bEnmlTagOpened = true;
	return true;
}

bool QHtmlContentHandler::closeTodoTag()
{
	m_bEnmlTagOpened = false;
	return true;
}

bool QHtmlContentHandler::openMediaTag(const QXmlAttributes& atts)
{
	QString contentType;
	QString mediaContentHash;
	QByteArray mediaContent;

	if(atts.value("src").isNull())
	{
		// If there is no src tag, this is external resource
		contentType = getContentType(atts.value("filename"));
		mediaContentHash = atts.value("hash");
	}
	else
	{
		mediaContent = getMediaContent(atts.value("src"), contentType);
		mediaContentHash = byteArrayToMd5Array(mediaContent).toHex();
	}
	m_sOutputEnml += "<en-media type=\"" + contentType + "\" hash=\"" + mediaContentHash + "\"/>";

	if(findResource(mediaContentHash) == false)
	{
		addMedia(contentType, mediaContent, atts);
	}

	//qDebug() << m_sOutputEnml << "\n";
	m_bEnmlTagOpened = true;
	return true;
}

bool QHtmlContentHandler::closeMediaTag(const QString& elemName)
{
	m_bEnmlTagOpened = false;
	return true;
}

bool QHtmlContentHandler::closeEnmlTag(const QString& localName)
{
	if(localName == "html" || localName == "head" || m_bEnmlTagOpened)
		return true;

	if(localName == "body")
		m_sOutputEnml += "</en-note>";
	else
		m_sOutputEnml += "</" + localName + ">";

	return true;
	//qDebug() << m_sOutputEnml << "\n";
}

QByteArray QHtmlContentHandler::getMediaContent(const QString& contentSource, QString& contentType)
{
	if(contentSource.startsWith("data:"))
	{
		return getInlineContent(contentSource, contentType);
	}
	else
	{
		return getStoredContent(contentSource, contentType);
	}
}

QByteArray QHtmlContentHandler::getInlineContent(const QString& contentSource, QString& contentType)
{
	QRegExp contentSourceRegexp("^data:(.*);base64,(.*)$");
	QString data;

	if(contentSourceRegexp.exactMatch(contentSource))
	{
		contentType =  contentSourceRegexp.cap(1);
		data = contentSourceRegexp.cap(2);

		//qDebug() << __FUNCTION__ << contentType << data;

		return QByteArray::fromBase64(data.toAscii());
	}

	return 0;
}

QByteArray QHtmlContentHandler::getStoredContent(const QString& contentSource, QString& contentType)
{
	bool retVal;

	QFile contentFile(QString::fromLocal8Bit(contentSource.toStdString().c_str(), contentSource.length()));
	retVal = contentFile.open(QIODevice::ReadOnly);

	contentType = getContentType(contentFile.fileName()); /*enmlMediaTypes[idx];*/
	return contentFile.readAll();
}

void QHtmlContentHandler::addMedia(QString sMimeType, const QByteArray& baMediaContent, const QXmlAttributes& atts)
{
	QByteArray mediaHash = byteArrayToMd5Array(baMediaContent);
	QString mediaContentHash = mediaHash.toHex();
	//Resource* noteResource = new Resource;
	Resource noteResource;

	/*noteResource.noteGuid = m_Note->guid;
	noteResource.__isset.noteGuid = true;*/

	noteResource.mime = sMimeType.toStdString();
	noteResource.__isset.mime = true;

	string initialDataString((const char*)baMediaContent, baMediaContent.size());

	noteResource.data.body = string(initialDataString.begin(), initialDataString.begin() + baMediaContent.size());
	noteResource.data.__isset.body = true;


	qDebug() << __FUNCTION__ << "Data size" << noteResource.data.body.size();

	string initialBodyHashString((const char*)mediaHash, mediaHash.size());
	noteResource.data.bodyHash = string(initialBodyHashString.begin(), initialBodyHashString.begin() + mediaHash.size());
	noteResource.data.__isset.bodyHash = true;

	//qDebug() << "Writing raw hash" << QString(byteArrayToMd5Array(baMediaContent).data());

	noteResource.data.size = baMediaContent.size();
	noteResource.data.__isset.size = true;

	noteResource.__isset.data = true;

	noteResource.attributes.fileName = atts.value("filename").toStdString();
	noteResource.attributes.__isset.fileName = true;
	noteResource.__isset.attributes = true;

	m_Note->resources.push_back(noteResource);
	m_Note->__isset.resources = true;
}

bool QHtmlContentHandler::findResource(const QString& mediaContentHash)
{
	vector<Resource>::iterator resourceIterator = m_Note->resources.begin();
	LimitsConstants lc;

	//qDebug() << __FUNCTION__ << m_Note->resources.size();

	while(resourceIterator != m_Note->resources.end())
	{
		QByteArray b = QByteArray::fromRawData((*resourceIterator).data.bodyHash.c_str(), lc.EDAM_HASH_LEN);

		if(b.toHex() == mediaContentHash)
		{
			return true;
		}

		resourceIterator++;
	}

	return false;
}

QByteArray	QHtmlContentHandler::byteArrayToMd5Array(const QByteArray& data)
{
	md5_state_t	state;
	md5_byte_t	digest[16];
	QByteArray sHexOutput;

	md5_init(&state);
	md5_append(&state, (const md5_byte_t*)(const char*)data, data.size());
	md5_finish(&state, digest);

	return QByteArray((const char*)digest, 16);
}

/* QEnmlWriter */
QEnmlWriter::QEnmlWriter(Note* note, const QString& htmlDocument)
: m_Note(note), m_sHtmlDocument(htmlDocument)
{
	/*
    QRegExp rBrTag("(<br>)");
    QRegExp rImgTag("(<img src=\"[^>]+\")>");
    //QRegExp rMetaFix("(<meta [^>]+)>");
    m_sHtmlDocument.replace(rBrTag, "<br/>");
    m_sHtmlDocument.replace(rImgTag, "\\1/>");
    */
    //m_sHtmlDocument.replace(rMetaFix, "\\1/>");
	//        sNoteHtml.replace(rImgSrcTag, "/");

    //qDebug() << __FUNCTION__ << m_sHtmlDocument;

	// Clear all note resource before running
	//m_Note->resources.clear();
	m_HtmlDocumentInputSource.setData("<?xml version=\"1.0\" encoding=\"UTF-8\"?>" + m_sHtmlDocument);

	m_HtmlContentHandler = shared_ptr<QHtmlContentHandler>(new QHtmlContentHandler(m_Note));
	setContentHandler(m_HtmlContentHandler.get());
	setErrorHandler(m_HtmlContentHandler.get());
}

QString QEnmlWriter::byteArrayToMd5String(const QByteArray& data)
{
	md5_state_t	state;
	md5_byte_t	digest[16];
	QString sHexOutput;

	md5_init(&state);
	md5_append(&state, (const md5_byte_t*)(const void*)data, data.size());
	md5_finish(&state, digest);

	for (int di = 0; di < 16; ++di)
		sHexOutput += QString("%1").arg((ushort)digest[di], 0, 16);
	//sprintf(hex_output + di * 2, "%02x", digest[di]);

	return sHexOutput;
}
