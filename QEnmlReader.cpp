/***********************************************************************
Copyright (C) 2010 Alex R <burbulator@gmail.com>

This file is part of Qvernote.

Qvernote is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Qvernote is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Qvernote.  If not, see <http://www.gnu.org/licenses/>.

***********************************************************************/
#include "QEnmlReader.h"
#include <QBuffer>
#include <QDebug>
#include "QAttachmentWidget.h"
#include "QPopplerSupport.h"
#include <iostream>

using namespace std;

static QString enmlElements[] = {
		"en-note", "en-media", "en-crypt", "en-todo", "img", NULL
};

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

static QString enmlMediaTypes[] = {
		"image/gif",
		"image/jpeg",
		"image/png",
		"audio/wav",
		"audio/x-wav",
		"audio/mpeg",
		"audio/amr",
		"application/pdf",
		"application/vnd.evernote.ink",
		"application/octet-stream",
		NULL
};

bool QEnmlContentHandler::startElement( const QString & namespaceURI, const QString & localName, const QString & , const QXmlAttributes & atts )
{
	//qDebug() << __FUNCTION__ << namespaceURI << localName << qName;

	switch(decodeEnmlElement(localName)) {
	case EN_TODO:
		openTodoElement(atts);
		break;
	case EN_NOTE:
		currentEnmlContentType = CT_HTML;
		break;
	case EN_MEDIA:
		currentEnmlContentType = CT_MEDIA;
		addMedia(atts);
		break;
	//case EN_IMG:
	//	if(m_bLoadExternalReferences == false)
	//		break;
	default:
		openHtmlElement(localName, atts);
	}

	return true;
}

bool QEnmlContentHandler::endElement ( const QString & namespaceURI, const QString & localName, const QString & )
{
	//qDebug() << __FUNCTION__ << localName;
	switch(decodeEnmlElement(localName)) {
	case EN_TODO:
	case EN_NONE:
		closeHtmlElement(localName);
		break;
	//case EN_IMG:
	//	if(m_bLoadExternalReferences)
	//		closeHtmlElement(localName);
	default:
		break;
	}

	currentEnmlContentType = CT_NONE;

	//qDebug() << "=============";
	//qDebug() << m_sOutputHtml;
	//qDebug() << "===============";
	return true;
}

bool QEnmlContentHandler::characters ( const QString & ch )
{
	//qDebug() << __FUNCTION__ << ch;
	appendCharacterData(ch, m_sOutputHtml);
	//m_sOutputHtml += ch;
	return true;
}

void QEnmlContentHandler::openHtmlElement(const QString& elemName, const QXmlAttributes & atts)
{
	QString attrList = "<" + elemName;

	for(int i = 0; i < atts.count(); i++)
	{
		// Remove style for thumbnails
		if(atts.localName(i) == "style" && m_bLoadExternalReferences == false)
			continue;

		attrList += " " + atts.localName(i) + "=\"" + atts.value(i) + "\"";
	}

	attrList += ">";
	m_sOutputHtml += attrList;
	//qDebug() << __FUNCTION__ << elemName;
}

void QEnmlContentHandler::closeHtmlElement(const QString& elemName)
{
	// Do not draw closing tag for br!!!
	if(elemName != "br")
		m_sOutputHtml += "</" + elemName + ">";
	//qDebug() << __FUNCTION__ << elemName;
}

void QEnmlContentHandler::addMedia(const QXmlAttributes & atts)
{
	int mediaSize;
	QByteArray mediaData;
	QString htmlTag;
	QString fileName;

	mediaSize = decodeResource(atts, mediaData, fileName);

	if(mediaSize <= 0)
	{
		qDebug() << "Error retrieving media" << atts.value("type");
		return;
	}
	switch(decodeEnmlMediaType(atts, htmlTag)) {
	case MT_IMAGE:
		if(m_Note.attributes.source != "web.clip")
		{
			m_sOutputHtml += "<tag_image filename=\"" + fileName +"\" src=\"data:" + atts.value("type") + ";base64," + mediaData + "\"><img src=\"data:" + atts.value("type") + ";base64," + mediaData + "\"/></tag_image>";
			m_bHasImage = true;
			imageAttributes = atts;
		}
		else
		{
			m_sOutputHtml += "<img src=\"data:" + atts.value("type") + ";base64," + mediaData + "\"/>";
		}
		break;
	case MT_AUDIO:
	{
		QAttachmentWidget attWidget(fileName, mediaSize);
		attWidget.setAudioType();
		QByteArray widgetIcon = QAttachmentWidget::toPng(&attWidget);
		m_sOutputHtml += "<" + htmlTag + " filename=\"" + fileName + "\" hash=\""+atts.value("hash")+"\"><a  onclick=\"javascript:JSApi.handleResource('" + atts.value("hash") + "')\"><img src=\"data:image/png;base64," + widgetIcon.toBase64() + "\"/></a></" + htmlTag + ">";
		m_bHasAudio = true;
		break;
	}
	case MT_PDF:
	{
		QAttachmentWidget attWidget(fileName, mediaSize);
		attWidget.setPdfType();
		QByteArray widgetIcon = QAttachmentWidget::toPng(&attWidget);
		//m_sOutputHtml += "<" + htmlTag + " filename=\"" + fileName + "\" hash=\""+atts.value("hash")+"\"><a href=\"" + atts.value("hash") + "\"><img src=\"data:image/png;base64," + widgetIcon.toBase64() + "\"/></a></" + htmlTag + ">";
		m_sOutputHtml += "<" + htmlTag + " filename=\"" + fileName + "\" hash=\""+atts.value("hash")+"\"><a onclick=\"javascript:JSApi.handleResource('" + atts.value("hash") + "')\"><img src=\"data:image/png;base64," + widgetIcon.toBase64() + "\"/></a></" + htmlTag + ">";
		m_bHasPdf = true;
		pdfAttributes = atts;
		break;
	}
	default:
	{
		QAttachmentWidget attWidget(fileName, mediaSize);
		QByteArray widgetIcon = QAttachmentWidget::toPng(&attWidget);
		m_sOutputHtml += "<" + htmlTag + " filename=\"" + fileName + "\" hash=\""+atts.value("hash")+"\"><a onclick=\"javascript:JSApi.handleResource('" + atts.value("hash") + "')\"><img src=\"data:image/png;base64," + widgetIcon.toBase64() + "\"/></a></" + htmlTag + ">";
		break;
	}
	}

	//qDebug() << __FUNCTION__ << m_sOutputHtml;
}

void QEnmlContentHandler::openTodoElement(const QXmlAttributes & atts)
{
	QString checkedStr = (atts.value("checked").size() > 0)? atts.value("checked") : "false";
	QString todoHtmlTag = "<tag_todo checked=\"" + checkedStr + "\">" \
		"<input id=\"" + QString::number(m_nTodoCount) + "\"";

	if(atts.value("checked") == "true")
	{
		todoHtmlTag += " checked ";
	}

	todoHtmlTag += "type=\"checkbox\" onclick=\"javascript:JSApi.setChecked(" + QString::number(m_nTodoCount) + ")\"/></tag_todo>";

	//qDebug() << __FUNCTION__ << todoHtmlTag;
	m_sOutputHtml += todoHtmlTag;
	m_nTodoCount++;
}

QEnmlContentHandler::EnmlElement QEnmlContentHandler::decodeEnmlElement(const QString& elemName)
{
	int cmpElemIdx = 0;

	while(enmlElements[cmpElemIdx] != NULL)
	{
		if(elemName == enmlElements[cmpElemIdx])
			return (EnmlElement)cmpElemIdx;

		cmpElemIdx++;
	}

	return EN_NONE;
}

QEnmlContentHandler::EnmlMediaType QEnmlContentHandler::decodeEnmlMediaType(const QXmlAttributes & atts, QString& htmlTag)
{
	int cmpMediaIdx = 0;

	while(enmlMediaTypes[cmpMediaIdx] != NULL)
	{
		if(atts.value("type") == enmlMediaTypes[cmpMediaIdx])
		{
			if(cmpMediaIdx < 3)
			{
				htmlTag = htmlElements[MT_IMAGE];
				return MT_IMAGE;
			}
			if(cmpMediaIdx < 7)
			{
				htmlTag = htmlElements[MT_AUDIO];
				return MT_AUDIO;
			}
			if(cmpMediaIdx == 7)
			{
				htmlTag = htmlElements[MT_PDF];
				return MT_PDF;
			}
			if(cmpMediaIdx == 8)
			{
				htmlTag = htmlElements[MT_INK];
				return MT_INK;
			}

			htmlTag = htmlElements[MT_APPLICATION];
			return MT_APPLICATION;
		}

		cmpMediaIdx++;
	}

	return MT_UNKNOWN;
}

QEnmlContentHandler::EnmlMediaType QEnmlContentHandler::decodeEnmlMediaType(const QString mediaType)
{
	int cmpMediaIdx = 0;

	while(enmlMediaTypes[cmpMediaIdx] != NULL)
	{
		if(mediaType == enmlMediaTypes[cmpMediaIdx])
		{
			if(cmpMediaIdx < 3)
			{
				return MT_IMAGE;
			}
			if(cmpMediaIdx < 7)
			{
				return MT_AUDIO;
			}
			if(cmpMediaIdx == 7)
			{
				return MT_PDF;
			}
			if(cmpMediaIdx == 8)
			{
				return MT_INK;
			}

			return MT_APPLICATION;
		}

		cmpMediaIdx++;
	}

	return MT_UNKNOWN;
}

int	QEnmlContentHandler::decodeResource(const QXmlAttributes & atts, QByteArray& bodyData, QString& fileName)
{
	vector<Resource>::iterator resourceIterator = m_Note.resources.begin();
	LimitsConstants lc;
	QString resourceHash = atts.value("hash");
	//qDebug() << "resourceHash" << resourceHash;
	QString bodyHash;
	Resource r;

	if(m_maxHtmlSize > 0 && m_sOutputHtml.length() >= m_maxHtmlSize)
		return 0;

	while(resourceIterator != m_Note.resources.end())
	{
		//cout << "std::string hash " << (*resourceIterator).data.bodyHash << endl;

		QByteArray b = QByteArray::fromRawData((*resourceIterator).data.bodyHash.c_str(), lc.EDAM_HASH_LEN);

		//qDebug() << "QByteArray: " << b.toHex();

		//bodyHash = QString((*resourceIterator).data.bodyHash.c_str());
		//qDebug() << "bodyHash" << bodyHash;
		//qDebug() << "b.toHex()" << b.toHex();
		//qDebug() << "Data size:" << (*resourceIterator).data.size;
		if(b.toHex() == resourceHash)
		{
			//qDebug() << "Html size" << m_sOutputHtml.length();
			// Load only images. All other stuff will be loaded on demand
			if(atts.value("type").startsWith("image"))
			{
				//qDebug() << "This is image, loading...";

				if(m_hEvernote->loadResource(r, (*resourceIterator).guid) == true)
				{
					//m_Note.resources.insert(resourceIterator, r);
					(*resourceIterator) = r;
					bodyData = QByteArray::fromRawData(r.data.body.c_str(), r.data.size).toBase64();
				}
				else
				{
					qDebug() << "Failed to load resource";
				}
			}
			fileName = QString::fromStdString((*resourceIterator).attributes.fileName);
			return (*resourceIterator).data.size;
		}

		resourceIterator++;
	}

	return 0;
}

int	QEnmlContentHandler::extractRawResource(const QXmlAttributes & atts, QByteArray& bodyData, QString& fileName)
{
	vector<Resource>::iterator resourceIterator = m_Note.resources.begin();
	LimitsConstants lc;
	QString resourceHash = atts.value("hash");
	QString bodyHash;
	Resource r;

	while(resourceIterator != m_Note.resources.end())
	{
		QByteArray b = QByteArray::fromRawData((*resourceIterator).data.bodyHash.c_str(), lc.EDAM_HASH_LEN);
		bodyHash = QString((*resourceIterator).data.bodyHash.c_str());
		if(b.toHex() == resourceHash)
		{
			if(m_hEvernote->loadResource(r, (*resourceIterator).guid) == true)
			{
				//m_Note.resources.insert(resourceIterator, r);
				(*resourceIterator) = r;
				bodyData = QByteArray::fromRawData(r.data.body.c_str(), r.data.size);
			}
			else
			{
				qDebug() << "Failed to load resource";
				return 0;
			}
			fileName = QString::fromStdString((*resourceIterator).attributes.fileName);
			return (*resourceIterator).data.size;
		}

		resourceIterator++;
	}

	return 0;
}

void QEnmlContentHandler::appendCharacterData(const QString& ch, QString& htmlData)
{
	QString str = ch;

	if(ch.contains('<'))
	{
		str.replace('<', "&lt;");
	}

	if(ch.contains('>'))
	{
		str.replace('>', "&gt;");
	}

	htmlData += str;
}

QEnmlReader::QEnmlReader(const Note& note)
: m_bLoadExternalReferences(true) {
	//m_EnmlDocumentInputSource = shared_ptr<QXmlInputSource>(new QXmlInputSource());
	m_EnmlDocumentInputSource.setData(QString::fromUtf8(note.content.c_str()));

	m_EnmlContentHandler = shared_ptr<QEnmlContentHandler>(new QEnmlContentHandler(note));

	setContentHandler(m_EnmlContentHandler.get());
	setErrorHandler(m_EnmlContentHandler.get());
}

QEnmlReader::~QEnmlReader() {

}

bool QEnmlReader::parse(int maxHtmlSize)
{
	m_EnmlContentHandler->setMaxHtmlSize(maxHtmlSize);
	m_EnmlContentHandler->setLoadExternalReferences(m_bLoadExternalReferences);
	return QXmlSimpleReader::parse(&m_EnmlDocumentInputSource);
}

bool QEnmlReader::parse(const Note& note,  int maxHtmlSize)
{
	m_EnmlDocumentInputSource.setData(QString::fromUtf8(note.content.c_str()));

	m_EnmlContentHandler = shared_ptr<QEnmlContentHandler>(new QEnmlContentHandler(note, maxHtmlSize));

	setContentHandler(m_EnmlContentHandler.get());
	setErrorHandler(m_EnmlContentHandler.get());

	return parse();
}

void QEnmlReader::createThumbnail(bool isAutoclean)
{
	QByteArray mediaData;
	QString fileName;
	int mediaSize;
	//QThumbnailCreator thumbnailCreator;
	QEnmlContentHandler* enmlHandler = (QEnmlContentHandler*)contentHandler();

	if(isAutoclean)
		QObject::connect(&thumbnailCreator, SIGNAL(snapshotReady()), this, SLOT(destroy()));

	thumbnailCreator.setGuid(enmlHandler->getNote().guid);

	if(enmlHandler->hasAudio())
	{
		QPixmap audioPixmap(QThumbnailCreator::defaultThumbnailSize());
		QString voiceStr = trUtf8("VOICE");
		audioPixmap.fill();
		QPainter p(&audioPixmap);
		int strWidth = p.fontMetrics().width(voiceStr);

		p.drawPixmap(audioPixmap.width() / 2 - 24, 10, QPixmap(":/IconAdd/file-audio.png"));
		p.drawText(audioPixmap.width() / 2 - strWidth / 2, 80, voiceStr);
		thumbnailCreator.fromPixmap(audioPixmap);
	}
	else if(enmlHandler->hasImage())
	{
		mediaSize = enmlHandler->extractRawResource(enmlHandler->getImageAttributes(), mediaData, fileName);
		thumbnailCreator.fromByteArray(mediaData);
	}
	else if(enmlHandler->hasPdf())
	{
		mediaSize = enmlHandler->extractRawResource(enmlHandler->getPdfAttributes(), mediaData, fileName);
		QPopplerSupport poppler(mediaData);
		QPixmap pdfPixmap(QThumbnailCreator::defaultThumbnailSize());
		pdfPixmap.fill();
		poppler.renderPage(0, pdfPixmap);
		thumbnailCreator.fromPixmap(pdfPixmap);
	}
	else
	{
		thumbnailCreator.fromHtml(enmlHandler->getHtml());
	}
}
