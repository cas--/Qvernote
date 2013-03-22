/*
 * QThumbnailCreator.h
 *
 *  Created on: Jul 25, 2010
 *      Author: alexr
 */

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
