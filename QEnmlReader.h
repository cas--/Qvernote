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
#ifndef QENMLREADER_H_
#define QENMLREADER_H_

#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QXmlDefaultHandler>
#include <Limits_constants.h>
#include "QvernoteAPI.h"
#include "QThumbnailCreator.h"
#include <QDebug>

class QEnmlContentHandler : public QXmlDefaultHandler
{
	enum EnmlElement {
		EN_NOTE,
		EN_MEDIA,
		EN_CRYPT,
		EN_TODO,
		EN_IMG,
		EN_NONE
	};

	enum EnmlContentType {
		CT_HTML,
		CT_MEDIA,
		CT_NONE
	};

public:
	enum EnmlMediaType {
		MT_IMAGE,
		MT_AUDIO,
		MT_PDF,
		MT_INK,
		MT_APPLICATION,
		MT_UNKNOWN = 100
	};

public:
	QEnmlContentHandler(const Note& note, int maxHtmlSize = 0)
	: m_Note(note),
	  currentEnmlContentType(CT_NONE),
	  isHtmlElementOpened(false),
	  m_hEvernote(QvernoteAPI::Instance()),
	  m_nTodoCount(0),
	  m_bHasImage(false),
	  m_bHasAudio(false),
	  m_bHasPdf(false),
	  m_maxHtmlSize(maxHtmlSize),
	  m_bLoadExternalReferences(true) { };

	~QEnmlContentHandler() { };

	bool startElement( const QString & namespaceURI, const QString & localName, const QString & , const QXmlAttributes & atts );
	bool endElement ( const QString & namespaceURI, const QString & localName, const QString &  );
	bool characters ( const QString & ch );
	bool startDocument () { m_sOutputHtml = "<html><body>";  return true; }
	bool endDocument () { m_sOutputHtml += "</body></html>"; /*qDebug() << __FUNCTION__ << m_sOutputHtml;*/ return true; }
	int	extractRawResource(const QXmlAttributes & atts, QByteArray& bodyData, QString& fileName);
	QString	getHtml() { return m_sOutputHtml; }
	int getTodoCount() { return m_nTodoCount; }
	void setMaxHtmlSize(int maxHtmlSize) { m_maxHtmlSize = maxHtmlSize; }

	bool hasImage() { return m_bHasImage; }
	bool hasAudio() { return m_bHasAudio; }
	bool hasPdf()	{ return m_bHasPdf; }

	QXmlAttributes& getImageAttributes() 	{ return imageAttributes; }
	QXmlAttributes& getPdfAttributes() 		{ return pdfAttributes; }

	Note& getNote() { return m_Note; }
	void setLoadExternalReferences(bool isLoadExternalReferences) { m_bLoadExternalReferences = isLoadExternalReferences; }
	static EnmlMediaType decodeEnmlMediaType(const QString mediaType);

private:
	int decodeResource(const QXmlAttributes & atts, QByteArray& bodyData, QString& fileName);
	EnmlElement decodeEnmlElement(const QString& elemName);
	EnmlMediaType decodeEnmlMediaType(const QXmlAttributes & atts, QString& htmlTag);

	void openHtmlElement(const QString& elemName, const QXmlAttributes & atts);
	void closeHtmlElement(const QString& elemName);
	void addMedia(const QXmlAttributes & atts);

	void openTodoElement(const QXmlAttributes & atts);

	void appendCharacterData(const QString& ch, QString& htmlData);


private:
	Note m_Note;
	QString m_sOutputHtml;
	EnmlElement	currentEnmlElement;
	EnmlContentType currentEnmlContentType;
	bool	isHtmlElementOpened;
	QvernoteAPI* m_hEvernote;
	int m_nTodoCount;
	bool m_bHasImage;
	QXmlAttributes imageAttributes;
	bool m_bHasAudio;
	bool m_bHasPdf;
	QXmlAttributes pdfAttributes;
	int m_maxHtmlSize;
	bool m_bLoadExternalReferences;
};

class QEnmlReader : public QObject, public QXmlSimpleReader
{
	Q_OBJECT

public:
	QEnmlReader(const Note& note);
	QEnmlReader() : m_bLoadExternalReferences(true) {}
	virtual ~QEnmlReader();

	bool parse(int maxHtmlSize = 0);
	bool parse(const Note& note, int maxHtmlSize = 0);
	void setLoadExternalReferences(bool isLoadExternalReferences) { m_bLoadExternalReferences = isLoadExternalReferences; }
	QString getHtml() { return ((QEnmlContentHandler*)contentHandler())->getHtml(); }
	int getTodoCount() { return ((QEnmlContentHandler*)contentHandler())->getTodoCount(); }
	QThumbnailCreator* getThumbnailCreator() { return &thumbnailCreator; }
	void createThumbnail(bool isAutoclean = false);

public slots:
	void destroy() { delete this; }

private:
	QXmlInputSource m_EnmlDocumentInputSource;
	shared_ptr<QEnmlContentHandler> m_EnmlContentHandler;
	bool m_bLoadExternalReferences;
	QThumbnailCreator thumbnailCreator;
};


#endif /* QENMLREADER_H_ */
