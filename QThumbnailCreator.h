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
#ifndef QTHUMBNAILCREATOR_H_
#define QTHUMBNAILCREATOR_H_

#include <QWebView>
#include <QPixmap>
#include "QvernoteAPI.h"
#include <QDebug>

class QThumbnailCreator : public QObject
{
	Q_OBJECT

public:
	QThumbnailCreator() {
		QObject::connect(&webView, SIGNAL(loadFinished(bool)), this, SLOT(setNoteSnapshot()));
		QObject::connect(&webView, SIGNAL(loadProgress(int)), this, SLOT(updateLoadProgress(int)));
		webView.settings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, false);
	}

	void setGuid(Guid guid) {
		note.guid = guid;
	}

	void fromHtml(QString htmlData) {
		webView.setHtml(htmlData);
	}

	void fromByteArray(QByteArray& rawData) {
		snapshot.loadFromData(rawData);
		snapshot = snapshot.scaled(defaultThumbnailSize(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
		storeSnapshot();

		emit snapshotReady();
	}

	void fromPixmap(QPixmap& pixmapData) {
		snapshot = pixmapData.scaled(defaultThumbnailSize(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
		storeSnapshot();

		emit snapshotReady();
	}

	QPixmap& getThumbnail() {
		return snapshot;
	}

	static QSize defaultThumbnailSize() {
		return QSize(133, 100);
	}

public slots:
	void setNoteSnapshot() {
		snapshot = QPixmap::grabWidget(&webView);
		// 180x150
		snapshot = snapshot.scaled(QSize(180, 150), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

		storeSnapshot();
		emit snapshotReady();
   	}

	void updateLoadProgress(int p) {
		emit loadProgress(p);
	}

signals:
	void loadProgress(int);
	void snapshotReady();

private:
	void storeSnapshot() {
		QByteArray 	ba;
		QBuffer 	buf(&ba);

		buf.open(QIODevice::WriteOnly);
		snapshot.save(&buf, "PNG");
		QvernoteStorage::Instance()->setNoteThumbnail(note, ba);
	}

private:
	QWebView	webView;
	QPixmap 	snapshot;
	Note 		note;
};

#endif /* QTHUMBNAILCREATOR_H_ */
