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
#include "QDeleteNotesView.h"
#include "QNoteListItemWidget.h"
#include "QNotesView.h"
#include "QNotebookPickList.h"

QDeleteNotesView::QDeleteNotesView(const QString& notebookName, QWidget *parent)
    : QvernoteWindow(parent), m_hEvernote(QvernoteAPI::Instance()), m_notebookName(notebookName), m_iSelectedCount(0), m_parent(parent)
{
	ui.setupUi(this);

	this->setWindowTitle(notebookName);
	m_pbDeleteSelected.setText(trUtf8("Delete"));
	m_pbCopyNotes.setText(trUtf8("Copy"));
	m_pbMoveNotes.setText(trUtf8("Move"));
	ui.gridLayout_2->setAlignment(ui.lwDeleteNoteList, Qt::AlignTop);
	ui.gridLayout_2->setAlignment(Qt::AlignTop);

	//m_pbDeleteSelected.setIcon(QIcon(":/IconAdd/general_stop.png"));
	//m_lSelectedCount.setText(trUtf8("0 selected"));
	//ui.statusBar->setLayoutDirection(Qt::RightToLeft);
	ui.statusBar->addWidget(&m_pbDeleteSelected);
	ui.statusBar->addWidget(&m_pbCopyNotes);
	ui.statusBar->addWidget(&m_pbMoveNotes);
	ui.statusBar->addWidget(&m_lSelectedCount);
	QObject::connect(&m_pbDeleteSelected, SIGNAL(clicked()), this, SLOT(deleteSelectedNotes()));
	QObject::connect(&m_pbCopyNotes, SIGNAL(clicked()), this, SLOT(copySelectedNotes()));
	QObject::connect(&m_pbMoveNotes, SIGNAL(clicked()), this, SLOT(moveSelectedNotes()));

	QObject::connect(ui.lwDeleteNoteList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(setItemMark(QListWidgetItem *)));
	setProgressIndicator(true);
}

QDeleteNotesView::~QDeleteNotesView()
{

}

void QDeleteNotesView::loadNotesIntoView()
{
	Note note;
	bool hasMoreNotes = true;
	QNoteListItemWidget* listItem;

	setProgressIndicator(true);

	ui.lwDeleteNoteList->setListItemHeight(70);

	for(hasMoreNotes = m_hEvernote->getFirstNote(note); hasMoreNotes; hasMoreNotes = m_hEvernote->getNextNote(note))
	{
		QListWidgetItem* iw = new QListWidgetItem(ui.lwDeleteNoteList);
		iw->setSizeHint(QSize(0, 70));
		listItem = new QNoteListItemWidget(note, true, this);
		connect(listItem, SIGNAL(noteMarked()), this, SLOT(addSelectedCount()));
		connect(listItem, SIGNAL(noteUnmarked()), this, SLOT(subSelectedCount()));
		ui.lwDeleteNoteList->setItemWidget(iw, listItem);
	}

	ui.lwDeleteNoteList->scrollToTop();
	setProgressIndicator(false);
}

void QDeleteNotesView::setItemMark(QListWidgetItem *item)
{
	QNoteListItemWidget* w = (QNoteListItemWidget*)ui.lwDeleteNoteList->itemWidget(item);
	w->mark();
}

void QDeleteNotesView::deleteSelectedNotes()
{
	QNoteListItemWidget* currItem;
	QListWidgetItem* listWidgetItem;

	if(m_iSelectedCount == 0 ||
	       QMessageBox::question(this,
	    		   trUtf8("Confirm"),
	    		   QString(trUtf8("Ready to delete %1 notes?")).arg(m_iSelectedCount),
	    		   QMessageBox::Yes|QMessageBox::No,
	    		   QMessageBox::Yes) == QMessageBox::No)
			return;

	setProgressIndicator(true);
	m_pbDeleteSelected.setEnabled(false);

	for(int i = 0; i < ui.lwDeleteNoteList->count(); i++)
	{
		listWidgetItem = ui.lwDeleteNoteList->item(i);
		currItem = (QNoteListItemWidget*)ui.lwDeleteNoteList->itemWidget(listWidgetItem);

		if(currItem->isItemMarked())
		{
			if(m_hEvernote->deleteNote(currItem->getNote().guid) == false)
			{
				displayError(trUtf8("Unable to delete note"), QString(m_hEvernote->getLastErrorString().c_str()));
			}
			else
			{
				currItem->close();
				ui.lwDeleteNoteList->removeItemWidget(listWidgetItem);
				subSelectedCount();
			}
		}
	}

	setProgressIndicator(false);
	m_pbDeleteSelected.setEnabled(true);

	ui.lwDeleteNoteList->clear();
	loadNotesIntoView();

	((QNotesView*)m_parent)->loadNotesIntoView();
}

void QDeleteNotesView::copySelectedNotes()
{
	QNotebookPickList list(m_notebookName, this);
	list.loadNotebooks();
	if(list.exec() == QDialog::Rejected)
		return;

	QNoteListItemWidget* currItem;
	QListWidgetItem* listWidgetItem;

	if(m_iSelectedCount == 0 ||
		   QMessageBox::question(this,
				   trUtf8("Confirm"),
				   QString(trUtf8("Ready to copy %1 notes to %2 notebook?")).arg(m_iSelectedCount).arg(list.getSelectedName()),
				   QMessageBox::Yes|QMessageBox::No,
				   QMessageBox::Yes) == QMessageBox::No)
			return;

	setProgressIndicator(true);
	m_pbDeleteSelected.setEnabled(false);
	m_pbCopyNotes.setEnabled(false);
	m_pbMoveNotes.setEnabled(false);

	for(int i = 0; i < ui.lwDeleteNoteList->count(); i++)
	{
		listWidgetItem = ui.lwDeleteNoteList->item(i);
		currItem = (QNoteListItemWidget*)ui.lwDeleteNoteList->itemWidget(listWidgetItem);

		if(currItem->isItemMarked())
		{
			if(m_hEvernote->copyNote(currItem->getNote().guid, list.getSelectedGuid().toStdString()) == false)
			{
				displayError(trUtf8("Unable to copy note"), QString(m_hEvernote->getLastErrorString().c_str()));
			}
			else
			{
				currItem->close();
				ui.lwDeleteNoteList->removeItemWidget(listWidgetItem);
				subSelectedCount();
			}
		}
	}

	setProgressIndicator(false);
	m_pbDeleteSelected.setEnabled(true);
	m_pbCopyNotes.setEnabled(true);
	m_pbMoveNotes.setEnabled(true);

	ui.lwDeleteNoteList->clear();
	loadNotesIntoView();

	((QNotesView*)m_parent)->loadNotesIntoView();
}

void QDeleteNotesView::moveSelectedNotes()
{
	QNotebookPickList list(m_notebookName, this);
	list.loadNotebooks();
	if(list.exec() == QDialog::Rejected)
		return;

	QNoteListItemWidget* currItem;
	QListWidgetItem* listWidgetItem;

	if(m_iSelectedCount == 0 ||
		   QMessageBox::question(this,
				   trUtf8("Confirm"),
				   QString(trUtf8("Ready to move %1 notes to %2 notebook?")).arg(m_iSelectedCount).arg(list.getSelectedName()),
				   QMessageBox::Yes|QMessageBox::No,
				   QMessageBox::Yes) == QMessageBox::No)
			return;

	setProgressIndicator(true);
	m_pbDeleteSelected.setEnabled(false);
	m_pbCopyNotes.setEnabled(false);
	m_pbMoveNotes.setEnabled(false);

	for(int i = 0; i < ui.lwDeleteNoteList->count(); i++)
	{
		listWidgetItem = ui.lwDeleteNoteList->item(i);
		currItem = (QNoteListItemWidget*)ui.lwDeleteNoteList->itemWidget(listWidgetItem);

		if(currItem->isItemMarked())
		{
			if(m_hEvernote->moveNote(currItem->getNote().guid, list.getSelectedGuid().toStdString()) == false)
			{
				displayError(trUtf8("Unable to copy note"), QString(m_hEvernote->getLastErrorString().c_str()));
			}
			else
			{
				currItem->close();
				ui.lwDeleteNoteList->removeItemWidget(listWidgetItem);
				subSelectedCount();
			}
		}
	}

	setProgressIndicator(false);
	m_pbDeleteSelected.setEnabled(true);
	m_pbCopyNotes.setEnabled(true);
	m_pbMoveNotes.setEnabled(true);

	ui.lwDeleteNoteList->clear();
	loadNotesIntoView();

	((QNotesView*)m_parent)->loadNotesIntoView();
}
