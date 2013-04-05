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
#ifndef QNOTEGRIDVIEW_H_
#define QNOTEGRIDVIEW_H_

#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QWebView>
#include <QGraphicsGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QWebFrame>
#include <QImage>
#include <QApplication>
#include <QDesktopWidget>
#include <QGraphicsSceneMouseEvent>
#include <QGridLayout>
#include "QvernoteAPI.h"
#include "QEnmlReader.h"
#include "QAbstractNoteView.h"
#include "QNotesView.h"
#include "QClickableLabel.h"
#include <QDebug>

class QNoteSnapshotWidget : public QWidget
{
	Q_OBJECT

public:
	QNoteSnapshotWidget(QWidget* parent = 0) : QWidget(parent), m_SnapshotImage(NULL), m_loadProgress(0), hasAttachment(false) {

	}

	~QNoteSnapshotWidget() {
		//qDebug() << trUtf8("Deleting image");
		//delete m_SnapshotImage;
		//m_SnapshotImage = NULL;
	}

	QSize sizeHint () const {
		return QSize(133, 100);
	}

	void paintEvent(QPaintEvent *) {
		//painter->drawPixmap(QPoint(0, 0), m_Snapshot);
		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing);

		if(m_SnapshotImage == NULL) {
			painter.setFont(QFont("Nokia Sans", 13));
			painter.drawText(QRect(QPoint(0, 0), size()), Qt::AlignCenter, QString(trUtf8("Loading %1%")).arg(m_loadProgress));
		} else {
			painter.drawPixmap(QPoint(0, 0), *m_SnapshotImage);
			if(hasAttachment)
				painter.drawPixmap(QRect(QPoint(size().width() - 32, 0), QSize(32, 32)), QPixmap(":/IconAdd/email_attachment.png"));
		}

		painter.setPen(QPen(QBrush(palette().color(QPalette::Background)), 8));
		painter.drawRoundedRect(QRectF(QPointF(0, 0), size()), 8, 8);
	}

	void render(Guid guid) {
		Note note;
		note.guid = guid;
		QvernoteAPI::Instance()->getNoteContent(note);
		enmlReader = new QEnmlReader(note);
		enmlReader->setLoadExternalReferences(false);
		enmlReader->parse();
		QObject::connect(enmlReader->getThumbnailCreator(), SIGNAL(snapshotReady()), this, SLOT(setNoteSnapshot()), Qt::QueuedConnection);
		QObject::connect(enmlReader->getThumbnailCreator(), SIGNAL(loadProgress(int)), this, SLOT(updateLoadProgress(int)));
		enmlReader->createThumbnail();
	}

	void render(QPixmap* thumbnail) {
		m_SnapshotImage = thumbnail;
		//update();
	}

	void setAttachment(bool attachment) {
		hasAttachment = attachment;
	}

	QPixmap* getSnapshotImage() { return m_SnapshotImage; }

public slots:
	void setNoteSnapshot()
	{
		m_SnapshotImage = new QPixmap(enmlReader->getThumbnailCreator()->getThumbnail());
		delete enmlReader;
		emit snapshotReady();
	}

	void updateLoadProgress(int p) {
		m_loadProgress = p;
		update();
	}

signals:
	void snapshotReady();

private:
	QWebView*	m_WebView;
	QPixmap*	m_SnapshotImage;
	int			m_loadProgress;
	bool		hasAttachment;
	QEnmlReader* enmlReader;
};

class QNoteWidget : public QClickableLabel
{
	Q_OBJECT

public:
	QNoteWidget(Note& note, QWidget* parent = 0) : QClickableLabel(parent), m_Note(note) {
		layout = new QGridLayout(this);
		layout->setContentsMargins(0, 4, 0, 0);
		layout->setSpacing(0);

		m_snapshot = new QNoteSnapshotWidget();
		if(note.resources.size() > 0)
			m_snapshot->setAttachment(true);

		layout->addWidget(m_snapshot, 0, 0);
		layout->setAlignment(m_snapshot, Qt::AlignCenter);
		lTitle.setStyleSheet("font: 18px Nokia Sans;");
		lTitle.setText(QString::fromUtf8(note.title.c_str()));
		lTitle.setFixedWidth(150);
		lTitle.setFixedHeight(30);
		lTitle.setContentsMargins(4, 0, 4, 0);

		if(lTitle.fontMetrics().boundingRect(lTitle.text()).width() < 150)
			lTitle.setAlignment(Qt::AlignHCenter);

		layout->addWidget(&lTitle, 1, 0);
		lDate.setStyleSheet("color: #37b4fc;font: 13px Nokia Sans;");
		lDate.setText(QDateTime::fromTime_t(note.created / 1000).toString("M/d/yy h:m AP"));
		lDate.setContentsMargins(4, 0, 4, 0);
		lDate.setAlignment(Qt::AlignHCenter);
		layout->addWidget(&lDate, 2, 0);
		//setAttribute(Qt::WA_AcceptTouchEvents);
		//setFlat(true);
		QObject::connect(this, SIGNAL(clicked()), SLOT(selectNote()));
		QObject::connect(m_snapshot, SIGNAL(snapshotReady()), this, SLOT(storeSnapshot()));
	}

	QSize sizeHint() const {
		return QSize(150, 150);
	}

	QSize	minimumSizeHint () const {
		return sizeHint();
	}

	QNoteSnapshotWidget* snapshot() {
		return m_snapshot;
	}

	/*void setThumbnail(QByteArray& thumbnail) {
		snapshot()->render(thumbnail);
		emit noteRenderDone();
	}*/

	void setThumbnail(QPixmap* thumbnail) {
		snapshot()->render(thumbnail);
		emit noteRenderDone();
	}

	void setThumbnail(Guid guid) {
		snapshot()->render(guid);
		emit noteRenderDone();
	}

	Note& getNote() {
		return m_Note;
	}

public slots:
	void selectNote() {
		emit noteSelected(this);
	}

	void storeSnapshot() {
		/*QByteArray ba;
		QBuffer buf(&ba);
		buf.open(QIODevice::WriteOnly);
		m_snapshot->getSnapshotImage()->save(&buf, trUtf8("PNG"));
		QvernoteStorage::Instance()->setNoteThumbnail(m_Note, ba);*/
		emit insertImageCache(m_Note, m_snapshot->getSnapshotImage());
		emit noteRenderDone();
	}

signals:
	void noteSelected(QNoteWidget*);
	void noteRenderDone();
	void insertImageCache(Note&, QPixmap*);

private:
	QGridLayout *layout;
	QNoteSnapshotWidget* m_snapshot;
	QLabel lTitle;
	QLabel lDate;
	Note	m_Note;
};

class QNoteGridView : public QWidget, public QAbstractNoteView
{
	Q_OBJECT

public:
	QNoteGridView(Guid& notebookGuid, QWidget* parent = 0)
	: QWidget(parent), QAbstractNoteView(notebookGuid), m_selectedItem(0), curCol(0), curRow(0) {
		noteGridLayout = new QGridLayout(this);
		noteGridLayout->setContentsMargins(4, 4, 4, 4);
		noteGridLayout->setSpacing(0);
		noteGridLayout->setSizeConstraint(QLayout::SetFixedSize);
		noteGridLayout->setAlignment(Qt::AlignTop);

		QObject::connect(this, SIGNAL(noteSelected(Note&)), dynamic_cast<QNotesView*>(parentWidget()), SLOT(openNoteViewer(Note&)));
		QObject::connect(this, SIGNAL(noteLoadDone()), dynamic_cast<QNotesView*>(parentWidget()), SLOT(onNoteLoadDone()));
		QObject::connect(this, SIGNAL(noteLoadProgress()), dynamic_cast<QNotesView*>(parentWidget()), SLOT(onNoteLoadProgress()));

		QObject::connect(this, SIGNAL(tryLoadNextNote()), SLOT(loadNextNote()), Qt::QueuedConnection);
	}

	~QNoteGridView() {
		foreach(QPixmap* pImage, m_snapshotCacheMap) {
			delete pImage;
		}

		m_snapshotCacheMap.clear();
	}

	void redrawNotes(QString filterString = "") {
		QvernoteAPI* api = QvernoteAPI::Instance();
		Note note;
		curCol = curRow = 0;

		QLayoutItem* i;
		while((i = noteGridLayout->itemAt(0)) != 0)
		{
			QWidget* w = i->widget();
			noteGridLayout->removeWidget(w);
			w->deleteLater();
		}

		QRect screenGeometry = QApplication::desktop()->screenGeometry();
		if(screenGeometry.width() > screenGeometry.height())
			maxCol = 5;
		else
			maxCol = 3;

		//t.start();

		m_filterString = filterString;
		if(api->getFirstNote(note))
		{
			createNoteWidget(note);
		}
		else
		{
			emit noteLoadDone();
		}
	}

	bool nextNote(Note& nextNote) {
		QLayoutItem* item = noteGridLayout->itemAt(++m_selectedItem);

		if(item == NULL)
		{
			m_selectedItem--;
			return false;
		}

		QNoteWidget* w = (QNoteWidget*)item->widget();

		nextNote = w->getNote();
		return true;
	}

	bool prevNote(Note& prevNote) {
		if(m_selectedItem == 0)
			return false;

		QLayoutItem* item = noteGridLayout->itemAt(--m_selectedItem);

		if(item == NULL)
		{
			m_selectedItem++;
			return false;
		}

		QNoteWidget* w = (QNoteWidget*)item->widget();
		prevNote = w->getNote();
		return true;
	}

public slots:
	void selectNote(QNoteWidget* noteWidget) {
		m_selectedItem = noteGridLayout->indexOf(noteWidget);
		emit noteSelected(noteWidget->getNote());
	}

	void loadNextNote() {
		QvernoteAPI* api = QvernoteAPI::Instance();
		Note note;

		if(api->getNextNote(note))
		{
			createNoteWidget(note);
			emit noteLoadProgress();
		}
		else
		{
			emit noteLoadDone();
		}
	}

	void onInsertImageCache(Note& note, QPixmap* pImage) {
		m_snapshotCacheMap.insert(QString::fromStdString(note.guid), pImage);
	}

signals:
	void noteSelected(Note&);
	void tryLoadNextNote();
	void noteLoadDone();
	void noteLoadProgress();
	void progressDataUpdated(QString);

private:
	void createNoteWidget(Note& note) {
		QByteArray thumbnail;
		QPixmap*	thumbnailImage;

		if(QString::fromStdString(note.title).startsWith(m_filterString, Qt::CaseInsensitive))
		{
			QNoteWidget* w = new QNoteWidget(note);
			QObject::connect(w, SIGNAL(noteRenderDone()), this, SLOT(loadNextNote()));

			if((thumbnailImage = m_snapshotCacheMap.value(QString::fromStdString(note.guid))) == NULL)
			{
				if(QvernoteStorage::Instance()->getNoteThumbnail(note.guid, thumbnail))
				{
					thumbnailImage = new QPixmap();
					thumbnailImage->loadFromData(thumbnail);
					m_snapshotCacheMap.insert(QString::fromStdString(note.guid), thumbnailImage);
					w->setThumbnail(thumbnailImage);
				}
				else
				{
					QObject::connect(w, SIGNAL(insertImageCache(Note&, QPixmap*)), this, SLOT(onInsertImageCache(Note&, QPixmap*)));
					w->snapshot()->render(note.guid);
				}
			}
			else
			{
				w->setThumbnail(thumbnailImage);
			}

			//w->resize(150, 150);
			noteGridLayout->addWidget(w, curRow, curCol++);

			if(curCol >= maxCol)
			{
				curCol = 0;
				curRow++;
			}

			QObject::connect(w, SIGNAL(noteSelected(QNoteWidget*)), this, SLOT(selectNote(QNoteWidget*)));
		}
		else
		{
			emit tryLoadNextNote();
		}
	}

private:
	QGridLayout* noteGridLayout;
	int m_selectedItem;
	QString m_filterString;
	int curCol;
	int curRow;
	int maxCol;
	QMap<QString, QPixmap*> m_snapshotCacheMap;
	QTime t;
};

#endif /* QNOTEGRIDVIEW_H_ */
