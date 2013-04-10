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
#include "QNotesView.h"
#include "QNoteEditWindow.h"
#include <QPropertyAnimation>
#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QPixmap>
#include <QMessageBox>
#include <QGraphicsProxyWidget>
#include <QGraphicsGridLayout>
#include "QNoteListItemWidget.h"
#include "QDeleteNotesView.h"
#include "QNoteViewWindow.h"
#include "QKeyGrabber.h"
#include "QNoteListView.h"
#include "QNoteGridView.h"

using namespace boost;

QNotesView::QNotesView(const Notebook& notebook, QWidget *parent)
    : QvernoteWindow(parent), m_hEvernote(QvernoteAPI::Instance()), m_Notebook(notebook), isSearchShown(false), isSynchronizing(false)
{
	ui.setupUi(this);

	this->setWindowTitle(QString(notebook.name.c_str()));

	switch(QvernoteSettings::Instance()->getNotesView()) {
	case QvernoteSettings::NV_LIST:
		m_pNoteListViewWidget = new QNoteListView(this);
		break;
	case QvernoteSettings::NV_GRID:
		m_pNoteListViewWidget = new QNoteGridView(m_Notebook.guid, this);
		break;
	default:
		break;
	}

	ui.gridLayout_2->addWidget(dynamic_cast<QWidget*>(m_pNoteListViewWidget), 1, 0, 1, 2);
	ui.gridLayout_2->setAlignment(Qt::AlignTop);

	QActionGroup *noteSortGroup = new QActionGroup(this);
	noteSortGroup->setExclusive(true);
	QAction* actAscending = new QAction(trUtf8("Oldest first"), noteSortGroup);
	actAscending->setCheckable(true);

	actAscending->setChecked((QvernoteSettings::Instance()->getNoteDisplayOrder() == QvernoteSettings::NDO_ASC));
	QAction* actDescending = new QAction(trUtf8("Newest first"), noteSortGroup);
	actDescending->setCheckable(true);
	actDescending->setChecked((QvernoteSettings::Instance()->getNoteDisplayOrder() == QvernoteSettings::NDO_DESC));

	QObject::connect(actAscending, SIGNAL(triggered()), this, SLOT(setNoteDisplayOrderAsc()));
	QObject::connect(actDescending, SIGNAL(triggered()), this, SLOT(setNoteDisplayOrderDesc()));

	ui.menuBar->addActions(noteSortGroup->actions());

	QObject::connect(ui.actionReload, SIGNAL(triggered(bool)), this, SLOT(reloadNotes()));
	QObject::connect(ui.pbNewNote, SIGNAL(clicked()), this, SLOT(openNewNoteWindow()));

	QObject::connect(ui.actionDelete_notes, SIGNAL(triggered(bool)), this, SLOT(loadDeleteNotesView()));

	m_pKeyGrabber = shared_ptr<QKeyGrabber>(new QKeyGrabber("", this));
	m_pKeyGrabber->setVisible(false);
	QObject::connect(m_pKeyGrabber.get(), SIGNAL(redrawParentContents(QString)), this, SLOT(redrawNoteList(QString)));
	ui.gridLayout->addWidget(m_pKeyGrabber.get(), 2, 0);

	setProgressIndicator(true);
	reloadNotes();
	displayInformationBox(trUtf8("Loading notes. Please wait..."));
}


QNotesView::QNotesView(QString windowTitle, QWidget *parent)
: QvernoteWindow(parent), m_hEvernote(QvernoteAPI::Instance()), isSearchShown(false), isSynchronizing(false)
{
	ui.setupUi(this);

	this->setWindowTitle(windowTitle);
	//ui.pbNewNote->setVisible(false);
	m_hEvernote->getDefaultNotebook(m_Notebook);

	switch(QvernoteSettings::Instance()->getNotesView()) {
	case QvernoteSettings::NV_LIST:
		m_pNoteListViewWidget = new QNoteListView(this);
		break;
	case QvernoteSettings::NV_GRID:
		m_pNoteListViewWidget = new QNoteGridView(m_Notebook.guid, this);
		break;
	default:
		break;
	}

	ui.gridLayout_2->addWidget(dynamic_cast<QWidget*>(m_pNoteListViewWidget), 1, 0, 1, 2);
	ui.gridLayout_2->setAlignment(Qt::AlignTop);
	QObject::connect(ui.actionDelete_notes, SIGNAL(triggered(bool)), this, SLOT(loadDeleteNotesView()));
	QObject::connect(ui.pbNewNote, SIGNAL(clicked()), this, SLOT(openNewNoteWindow()));

	m_pKeyGrabber = shared_ptr<QKeyGrabber>(new QKeyGrabber("", this));
	m_pKeyGrabber->setVisible(false);
	QObject::connect(m_pKeyGrabber.get(), SIGNAL(redrawParentContents(QString)), this, SLOT(redrawNoteList(QString)));
	ui.gridLayout->addWidget(m_pKeyGrabber.get(), 2, 0);

	setProgressIndicator(true);
	displayInformationBox(trUtf8("Loading notes. Please wait..."));
	loadNotesIntoView();
}

QNotesView::~QNotesView()
{

}

void QNotesView::reloadNotes()
{
	ui.menuBar->setEnabled(false);
	m_pBackgroundJob->start();
}

void QNotesView::loadNotesIntoView(QString noteFilter)
{
	isSynchronizing = true;
	m_pNoteListViewWidget->redrawNotes(noteFilter);
}

void QNotesView::openNewNoteWindow()
{
	Note newNote;// = new Note();
	newNote.notebookGuid = m_Notebook.guid;

	m_pNoteEditWindow = shared_ptr<QNoteEditWindow>(new QNoteEditWindow(newNote, true, this));
	QObject::connect(m_pNoteEditWindow.get(), SIGNAL(noteUpdated()), this, SLOT(reloadNotes()));
	m_pNoteEditWindow->show();
}

void QNotesView::openNoteViewer(Note& note)
{
	if(!m_pNoteViewWindow.get() || m_pNoteViewWindow->getNote().guid != note.guid)
		m_pNoteViewWindow = shared_ptr<QNoteViewWindow>(new QNoteViewWindow(note, this));

	m_pNoteViewWindow->show();
}

void QNotesView::getNextNote(Note& nextNote)
{
	if(m_pNoteListViewWidget->nextNote(nextNote) == false)
	{
		displayInformationBox(trUtf8("This is the last note"));
	}
}

void QNotesView::getPrevNote(Note& prevNote)
{
	if(m_pNoteListViewWidget->prevNote(prevNote) == false)
	{
		displayInformationBox(trUtf8("This is the first note"));
	}
}

void QNotesView::loadDeleteNotesView()
{
	m_pDeleteNotesView = shared_ptr<QDeleteNotesView>(new QDeleteNotesView(QString(m_Notebook.name.c_str()), this));
	m_pDeleteNotesView->show();
}

void QNotesView::notesViewShutdown()
{
	displayError(trUtf8("Error loading notebook"), QString(m_hEvernote->getLastErrorString().c_str()));
	close();
}

bool QNotesView::queryNotes()
{
	bool retVal;
	isSynchronizing = true;
	retVal = m_hEvernote->loadNotes(100000, m_Notebook);
	return retVal;
}

void QNotesView::clearNote(Note& m_Note)
{
	//qDebug() << trUtf8("Delete note content");

	m_Note.content.clear();
	m_Note.content.reserve(0);
	m_Note.tagNames.clear();
	m_Note.tagGuids.clear();
	for(vector<Resource>::iterator i = m_Note.resources.begin(); i != m_Note.resources.end(); i++)
	{
		(*i).data.body.clear();
		(*i).data.body.reserve(0);
		(*i).recognition.body.clear();
		(*i).recognition.body.reserve(0);
		(*i).alternateData.body.clear();
		(*i).alternateData.body.reserve(0);
	}
}

void QNotesView::keyPressEvent(QKeyEvent* event)
{
	if(!isSearchShown && !isSynchronizing && !event->text().isEmpty())
	{
		isSearchShown = true;
		m_pKeyGrabber->setText(event->text());
		m_pKeyGrabber->setVisible(true);
	}

	QvernoteWindow::keyPressEvent(event);
}

void QNotesView::redrawNoteList(QString searchString)
{
	//qDebug() << __FUNCTION__ << searchString;
	if(searchString.isEmpty())
		isSearchShown = false;

	loadNotesIntoView(searchString);
}

void QNotesView::onNoteLoadProgress()
{
	ui.scrollArea->ensureVisible(0, ui.scrollAreaWidgetContents->geometry().height() + 100);
}

void QNotesView::onNoteLoadDone()
{
	ui.scrollArea->ensureVisible(0, 0);
	setProgressIndicator(false);
	isSynchronizing = false;
	ui.menuBar->setEnabled(true);
}

void QNotesView::orientationChanged()
{
	if(!isSynchronizing)
		loadNotesIntoView();
}

void QNotesView::setNoteDisplayOrderAsc()
{
	setProgressIndicator(true);
	QvernoteSettings::Instance()->setNoteDisplayOrder(QvernoteSettings::NDO_ASC);
	reloadNotes();
}

void QNotesView::setNoteDisplayOrderDesc()
{
	setProgressIndicator(true);
	QvernoteSettings::Instance()->setNoteDisplayOrder(QvernoteSettings::NDO_DESC);
	reloadNotes();
}
