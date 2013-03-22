#include "QTrashWindow.h"
#include "QvernoteView.h"
#include "QNoteListItemWidget.h"

QTrashWindow::QTrashWindow(QWidget *parent)
    : QvernoteWindow(parent), m_hEvernote(QvernoteAPI::Instance()), m_iSelectedCount(0), m_parent(parent)
{
	ui.setupUi(this);

	m_pbDeleteSelected.setText(trUtf8("Delete"));
	m_pbRestoreSelected.setText(trUtf8("Restore"));
	ui.gridLayout_2->setAlignment(ui.lwTrashList, Qt::AlignTop);

	//m_pbDeleteSelected.setIcon(QIcon(":/IconAdd/general_stop.png"));
	//m_lSelectedCount.setText(trUtf8("0 selected"));
	//ui.statusBar->setLayoutDirection(Qt::RightToLeft);
	//ui.statusbar->addWidget(&m_pbDeleteSelected);
	ui.statusbar->addWidget(&m_pbRestoreSelected);
	ui.statusbar->addWidget(&m_lSelectedCount);
	QObject::connect(&m_pbDeleteSelected, SIGNAL(clicked()), this, SLOT(deleteSelectedNotes()));
	QObject::connect(&m_pbRestoreSelected, SIGNAL(clicked()), this, SLOT(restoreSelectedNotes()));
	QObject::connect(ui.lwTrashList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(setItemMark(QListWidgetItem *)));
	setProgressIndicator(true);
}

QTrashWindow::~QTrashWindow()
{

}

void QTrashWindow::loadNotesIntoView()
{
	Note note;
	bool hasMoreNotes = true;
	QNoteListItemWidget* listItem;

	setProgressIndicator(true);

	ui.lwTrashList->setListItemHeight(70);

	vector<Note> trashNoteList;
	m_hEvernote->loadTrashNoteList(trashNoteList);
	for(vector<Note>::iterator i = trashNoteList.begin(); i != trashNoteList.end(); i++)
	{
		QListWidgetItem* iw = new QListWidgetItem(ui.lwTrashList);
		iw->setSizeHint(QSize(0, 70));
		listItem = new QNoteListItemWidget(*i, true, this);
		connect(listItem, SIGNAL(noteMarked()), this, SLOT(addSelectedCount()));
		connect(listItem, SIGNAL(noteUnmarked()), this, SLOT(subSelectedCount()));
		ui.lwTrashList->setItemWidget(iw, listItem);
	}

	setProgressIndicator(false);
}

void QTrashWindow::setItemMark(QListWidgetItem *item)
{
	QNoteListItemWidget* w = (QNoteListItemWidget*)ui.lwTrashList->itemWidget(item);
	w->mark();
}

void QTrashWindow::deleteSelectedNotes()
{
	QNoteListItemWidget* currItem;
	QListWidgetItem* listWidgetItem;

	if(m_iSelectedCount == 0 ||
	       QMessageBox::question(this,
	    		   trUtf8("Confirm"),
	    		   QString(trUtf8("Ready to permanently delete %1 notes?")).arg(m_iSelectedCount),
	    		   QMessageBox::Yes|QMessageBox::No,
	    		   QMessageBox::Yes) == QMessageBox::No)
			return;

	setProgressIndicator(true);
	m_pbDeleteSelected.setEnabled(false);

	for(int i = 0; i < ui.lwTrashList->count(); i++)
	{
		listWidgetItem = ui.lwTrashList->item(i);
		currItem = (QNoteListItemWidget*)ui.lwTrashList->itemWidget(listWidgetItem);

		if(currItem->isItemMarked())
		{
			if(m_hEvernote->expungeNote(currItem->getNote().guid) == false)
			{
				displayError(trUtf8("Unable to expunge note"), QString(m_hEvernote->getLastErrorString().c_str()));
			}
			else
			{
				currItem->close();
				ui.lwTrashList->removeItemWidget(listWidgetItem);
				subSelectedCount();
			}
		}
	}

	setProgressIndicator(false);
	m_pbDeleteSelected.setEnabled(true);

	ui.lwTrashList->clear();
	loadNotesIntoView();
}

void QTrashWindow::restoreSelectedNotes()
{
	QNoteListItemWidget* currItem;
	QListWidgetItem* listWidgetItem;

	if(m_iSelectedCount == 0 ||
		   QMessageBox::question(this,
				   trUtf8("Confirm"),
				   QString(trUtf8("Ready to restore %1 notes?")).arg(m_iSelectedCount),
				   QMessageBox::Yes|QMessageBox::No,
				   QMessageBox::Yes) == QMessageBox::No)
			return;

	setProgressIndicator(true);
	m_pbRestoreSelected.setEnabled(false);

	for(int i = 0; i < ui.lwTrashList->count(); i++)
	{
		listWidgetItem = ui.lwTrashList->item(i);
		currItem = (QNoteListItemWidget*)ui.lwTrashList->itemWidget(listWidgetItem);

		if(currItem->isItemMarked())
		{
			if(m_hEvernote->undeleteNote(currItem->getNote()) == false)
			{
				displayError(trUtf8("Unable to restore note"), QString(m_hEvernote->getLastErrorString().c_str()));
			}
			else
			{
				currItem->close();
				ui.lwTrashList->removeItemWidget(listWidgetItem);
				subSelectedCount();
			}
		}
	}

	setProgressIndicator(false);
	m_pbDeleteSelected.setEnabled(true);

	ui.lwTrashList->clear();
	loadNotesIntoView();

	((QvernoteView*)m_parent)->reloadNotebookList();
}
