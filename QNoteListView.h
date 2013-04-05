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
#ifndef QNOTELISTVIEW_H_
#define QNOTELISTVIEW_H_

#include "QNoteListItemWidget.h"
#include "QNonScrollableListWidget.h"
#include "QAbstractNoteView.h"
#include "QvernoteAPI.h"
#include "QNotesView.h"
#include <QListWidgetItem>

class QNoteListView : public QNonScrollableListWidget,  public QAbstractNoteView
{
	Q_OBJECT

public:
	QNoteListView(QWidget* parent = 0)
	: QNonScrollableListWidget(parent) {
		setFrameShape(QFrame::NoFrame);
		setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setAutoScroll(false);
		//redrawNotes();
		QObject::connect(this, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(selectNote(QListWidgetItem *)));
		QObject::connect(this, SIGNAL(noteSelected(Note&)), dynamic_cast<QNotesView*>(parentWidget()), SLOT(openNoteViewer(Note&)));
		QObject::connect(this, SIGNAL(noteLoadDone()), dynamic_cast<QNotesView*>(parentWidget()), SLOT(onNoteLoadDone()));
	}

	void redrawNotes(QString filterString = "") {
		QvernoteAPI* api = QvernoteAPI::Instance();
		Note note;
		bool hasMoreNotes = true;
		clear();
		setVisible(false);
		setListItemHeight(70);

		for(hasMoreNotes = api->getFirstNote(note); hasMoreNotes; hasMoreNotes = api->getNextNote(note))
		{
			if(QString::fromStdString(note.title).startsWith(filterString, Qt::CaseInsensitive))
			{
				QListWidgetItem* iw = new QListWidgetItem();
				iw->setSizeHint(QSize(0, 70));
				insertItem(0, iw);
				setItemWidget(iw, new QNoteListItemWidget(note, false, parentWidget()));
			}
		}

		setVisible(true);
		emit noteLoadDone();
	}

	bool nextNote(Note& nextNote) {
		QList<QListWidgetItem*> l = selectedItems();
		QListWidgetItem* nextItem = item(row(l[0]) + 1);
		if(nextItem == NULL)
			return false;

		setItemSelected(nextItem, true);
		nextNote = ((QNoteListItemWidget*)itemWidget(nextItem))->getNote();
		return true;
	}

	bool prevNote(Note& prevNote) {
		QList<QListWidgetItem*> l = selectedItems();
		QListWidgetItem* prevItem = item(row(l[0]) - 1);

		if(prevItem == NULL)
			return false;

		setItemSelected(prevItem, true);
		prevNote = ((QNoteListItemWidget*)itemWidget(prevItem))->getNote();
		return true;
	}

public slots:
	void selectNote(QListWidgetItem *item) {
		QNoteListItemWidget* w = (QNoteListItemWidget*)itemWidget(item);
		emit noteSelected(w->getNote());
	}

signals:
	void noteSelected(Note&);
	void noteLoadDone();
};
#endif /* QNOTELISTVIEW_H_ */
