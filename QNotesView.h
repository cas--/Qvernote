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
#ifndef QNOTESVIEW_H
#define QNOTESVIEW_H

#include <QtGui/QMainWindow>
#include "QvernoteWindow.h"
#include <QGraphicsWidget>
#include <QGraphicsGridLayout>
#include "ui_QNotesView.h"
#include "QvernoteAPI.h"
#include "QNotesViewGraphicsWidget.h"
#include "QNoteListItemWidget.h"
#include <QThread>
#include "QAbstractNoteView.h"
#include "QNoteViewWindow.h"
#include "QNoteEditWindow.h"
#include "QDeleteNotesView.h"
#include "QKeyGrabber.h"

class QNotesView : public QvernoteWindow
{
    Q_OBJECT

public:

    QNotesView(const Notebook& notebook, QWidget *parent = 0);
    QNotesView(QString windowTitle, QWidget *parent = 0);
    ~QNotesView();

    Notebook getNotebook() { return m_Notebook; }

    void getNextNote(Note& nextNote);
    void getPrevNote(Note& prevNote);
    void clearNote(Note& m_Note);

    void keyPressEvent(QKeyEvent* event);
public slots:
	void reloadNotes();
	void loadNotesIntoView(QString noteFilter = "");
	void openNewNoteWindow();
	void openNoteViewer(Note&);
	void loadDeleteNotesView();
	void notesViewShutdown();
	void redrawNoteList(QString searchString);
	void onNoteLoadProgress();
	void onNoteLoadDone();
	void setNoteDisplayOrderAsc();
	void setNoteDisplayOrderDesc();

	void orientationChanged();

	bool backgroundJobCallback() { return queryNotes(); }
    void jobDoneCallback() { loadNotesIntoView(); }
    void jobFailedCallback() { notesViewShutdown(); }

private:
	bool queryNotes();

private:
    Ui::QNotesViewClass ui;
    QvernoteAPI* m_hEvernote;
    Notebook	m_Notebook;
    bool	isSearchShown;
    bool 	isSynchronizing;
    QAbstractNoteView* m_pNoteListViewWidget;
    shared_ptr<QNoteViewWindow> 	m_pNoteViewWindow;
    shared_ptr<QNoteEditWindow> 	m_pNoteEditWindow;
    shared_ptr<QDeleteNotesView> 	m_pDeleteNotesView;
    shared_ptr<QKeyGrabber>			m_pKeyGrabber;
};

#endif // QNOTESVIEW_H
