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
#include "QNoteWidget.h"
#include "QNoteEditWindow.h"
#include "QNotesView.h"

#include <qsizepolicy.h>
#include <qdatetime.h>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QMainWindow>
#include <QMessageBox>

using namespace qvernote::api;
using namespace qvernote::xml;

QNoteWidget::QNoteWidget(Note& note, QWidget* topWidget, QWidget *parent)
    : QWidget(parent), m_Note(note), m_topWidget(topWidget), isMouseDown(false)
{
	ui.setupUi(this);
	ui.gridLayout->setMargin(10);
	enmlReader = shared_ptr<QEnmlReader>(new QEnmlReader(m_Note));
	enmlReader->parse();

	m_WebView.setHtml(enmlReader->getHtml());

	snapshotScene = shared_ptr<QNoteSnapshotScene>(new QNoteSnapshotScene(this));

	ui.gvNoteSnapshot->setScene(snapshotScene.get());

	ui.gvNoteSnapshot->setFixedWidth(width());
	ui.gvNoteSnapshot->setFixedHeight(width() * 9 / 16);

	qDebug() << "Scale to " << ui.gvNoteSnapshot->width() << ui.gvNoteSnapshot->height();

	m_Snapshot = QPixmap::grabWidget(&m_WebView).scaled(ui.gvNoteSnapshot->width()+50, ui.gvNoteSnapshot->height()+50, Qt::KeepAspectRatioByExpanding);
	scenePixmap = shared_ptr<QGraphicsPixmapItem>(new QGraphicsPixmapItem());
	scenePixmap->setPixmap(m_Snapshot);
	snapshotScene->addItem(scenePixmap.get());

#ifdef Q_WS_MAEMO_5
	ui.lNoteTitle->setText("<font size=\"3\" color=\"white\">" + QString(note.title.c_str()) + "</font>");
	ui.lNoteDate->setText("<font size=\"3\" color=\"white\">" + QDateTime::fromTime_t(note.created / 1000).toString("M/d/yy h:m AP") + "</font>");
#else
	ui.lNoteTitle->setText("<font size=\"3\" color=\"black\">" + QString(note.title.c_str()) + "</font>");
	ui.lNoteDate->setText("<font size=\"3\" color=\"black\">" + QDateTime::fromTime_t(note.created / 1000).toString("M/d/yy h:m AP") + "</font>");
#endif

	QObject::connect(snapshotScene.get(), SIGNAL(mouseReleased()), this, SLOT(openNoteEditor()));
	QObject::connect(ui.lDeleteNote, SIGNAL(mousePressed()), this, SLOT(deleteSelectedNote()));
}

QNoteWidget::~QNoteWidget()
{

}

void QNoteWidget::openNoteEditor()
{
	QNoteEditWindow* pNoteEditWindow = new QNoteEditWindow(m_Note, enmlReader->getHtml(), ((QNotesView*)m_topWidget)->getHandle(), m_topWidget);
	pNoteEditWindow->show();
}

void QNoteWidget::deleteSelectedNote()
{
	if(QMessageBox::question(m_topWidget, "Delete note", "Are you sure?", QMessageBox::Ok|QMessageBox::Cancel) == QMessageBox::Ok)
	{
		//((QNotesView*)m_topWidget)->deleteSelectedNote(this);
	}
}

void QNoteWidget::paintEvent ( QPaintEvent * )
{
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing, true);
	QPen pen(Qt::SolidLine);


	if(snapshotScene->getMouseDown())
	{
		pen.setWidth(4);
		pen.setColor(Qt::white);
	}
	else
	{
		pen.setWidthF(0.5);
		pen.setColor(Qt::gray);
	}


	p.setPen(pen);
	p.drawRoundedRect(QRect(4, 4, width()-6, height()-6), 20, 20);
}


void QNoteWidget::mousePressEvent( QMouseEvent *e )
{
	if(e->x() > ui.gvNoteSnapshot->x() &&
			e->y() > ui.gvNoteSnapshot->y() &&
			e->x() < ui.gvNoteSnapshot->x() + ui.gvNoteSnapshot->width() &&
			e->y() < ui.gvNoteSnapshot->y() + ui.gvNoteSnapshot->height())
	{
		snapshotScene->setMouseDown(true);
		update();
		//qDebug() << "Mouse down";
	}
}

/*
void QNoteWidget::mouseReleaseEvent( QMouseEvent * )
{
	isMouseDown = false;
	qDebug() << "Mouse up";
}
*/

