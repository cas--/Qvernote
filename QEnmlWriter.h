/*
 * QEnmlWriter.h
 *
 *  Created on: May 20, 2010
 *      Author: alexr
 */

#ifndef QENMLWRITER_H_
#define QENMLWRITER_H_

//#include <QString>
#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QXmlDefaultHandler>
#include <Limits_constants.h>
#include "QvernoteAPI.h"
#include <QDebug>



class QHtmlContentHandler : public QXmlDefaultHandler {
public:
	enum HtmlElement {
		HTML_IMG = 0,
		HTML_AUDIO = 1,
		HTML_PDF = 2,
		HTML_INK = 3,
		HTML_APP = 4,
		HTML_CRYPT,
		HTML_TODO,
		HTML_STANDARD
	};

	/*
	enum EnmlMediaType {
		MT_IMAGE = 0,
		MT_AUDIO = 3,
		MT_APPLICATION = 6,
		MT_UNKNOWN = 100
	};

	enum EnmlContentType {
		CT_HTML,
		CT_MEDIA,
		CT_NONE
	};
	*/

public:
	QHtmlContentHandler(Note* note)
	: m_Note(note), m_bEnmlTagOpened(false) { };
	//~QHtmlContentHandler() {};

	bool startElement( const QString & namespaceURI, const QString & localName, const QString & , const QXmlAttributes & atts );
	bool endElement ( const QString & namespaceURI, const QString & localName, const QString &  );
	bool characters ( const QString & ch ) { m_sCharBuffer += ch; return true; }
	bool startDocument ();
	bool endDocument () {
		//m_sOutputEnml += "</en-note>";
		//qDebug() << __FUNCTION__ << m_sOutputEnml << "\n";
		return true;
	}

	bool fatalError ( const QXmlParseException & exception ) {
		qDebug() << __FUNCTION__ << exception.message() << exception.lineNumber();
		return false;
	}
	QString	getEnml() { return m_sOutputEnml; }

	static HtmlElement getHtmlTag(const QString& fileName, QString& htmlTag);
	static QString getContentType(const QString& contentSource);
	static QByteArray getStoredContent(const QString& contentSource, QString& contentType);

private:
	HtmlElement decodeHtmlElement(const QString& elemName);
	//EnmlMediaType decodeEnmlMediaType(const QXmlAttributes & atts);
	int decodeResource(const QXmlAttributes & atts, QByteArray& bodyData);

	bool openHtmlTag(const QString& elemName, const QXmlAttributes & atts);
	bool openCryptTag(const QXmlAttributes & atts);
	bool openTodoTag(const QXmlAttributes & atts);
	bool closeTodoTag();
	bool openMediaTag(const QXmlAttributes & atts);
	bool closeMediaTag(const QString& elemName);

	bool findResource(const QString& mediaContentHash);
	QByteArray getMediaContent(const QString& contentSource, QString& contentType);
	QByteArray getInlineContent(const QString& contentSource, QString& contentType);

	bool closeEnmlTag(const QString& elemName);
	void addMedia(QString sMimeType, const QByteArray& baMediaContent, const QXmlAttributes& atts);
	void flushBuffer() {
		m_sOutputEnml += m_sCharBuffer;
		m_sCharBuffer.clear();
	}

	QByteArray	byteArrayToMd5Array(const QByteArray& data);

private:
	Note* m_Note;
	//QString m_sHtmlDocument;
	QString m_sOutputEnml;
	QString m_sCharBuffer;
	QString m_currentTag;
	bool	m_bEnmlTagOpened;
	//bool	m_bMediaTagOpened;
	//bool	m_bTodoTagOpened;
	//bool	m_bCryptTagOpened;
};

class QEnmlWriter : public QXmlSimpleReader {
public:
	QEnmlWriter(Note* note, const QString& htmlDocument);
	virtual ~QEnmlWriter() {};

	void createNote() {
		QString enmlDoc;
		QXmlSimpleReader::parse(&m_HtmlDocumentInputSource);

		enmlDoc = getEnmlDocument();

		/* This is a very bad hack ! */
		/* Im using it only because QT XmlParser is shit */
		if(!(enmlDoc.endsWith("en-note>")))
				enmlDoc.append("</en-note>");

		m_Note->content = enmlDoc.toStdString();
		//else
		//	qDebug() << "Parse error" << errorHandler()->errorString();
	}
	void setNoteTitle(QString sTitle) { m_Note->title = sTitle.toStdString(); }
	//QString addMedia(QString sMimeType, const QByteArray& baMediaContent);
	QString getEnmlDocument() { return ((QHtmlContentHandler*)contentHandler())->getEnml();/*writeDoctype(); return  m_sEnmlDocument;*/ }
	Note* getNote() {
		//qDebug() << __FUNCTION__ << getEnmlDocument();
		m_Note->content = getEnmlDocument().toStdString();
		return m_Note;
	}

	Note* getNotePtr() { return m_Note; }
private:
	QString byteArrayToMd5String(const QByteArray& data);

private:
	Note*	m_Note;
	QString m_sHtmlDocument;
	QXmlInputSource m_HtmlDocumentInputSource;
	shared_ptr<QHtmlContentHandler> m_HtmlContentHandler;
};

#endif /* QENMLWRITER_H_ */
