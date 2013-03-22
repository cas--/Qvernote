#include "QDeleteNotebookView.h"
#include "QNotebookListItemWidget.h"
#include "QvernoteView.h"

QDeleteNotebookView::QDeleteNotebookView(QWidget *parent)
    : QvernoteWindow(parent),  m_hEvernote(QvernoteAPI::Instance()), m_iSelectedCount(0), m_parent(parent)
{
	ui.setupUi(this);

	m_pbDeleteSelected.setText("Delete");
	ui.gridLayout_2->setAlignment(ui.lwNotebookList, Qt::AlignTop);

	ui.statusbar->addWidget(&m_pbDeleteSelected);
	ui.statusbar->addWidget(&m_lSelectedCount);
	QObject::connect(&m_pbDeleteSelected, SIGNAL(clicked()), this, SLOT(deleteSelectedNotebooks()));
	QObject::connect(ui.lwNotebookList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(setItemMark(QListWidgetItem *)));
	setProgressIndicator(true);
}

QDeleteNotebookView::~QDeleteNotebookView()
{

}

void QDeleteNotebookView::loadNotebooksIntoView()
{
	Notebook notebook;
	bool hasNextNotebook = false;
	QNotebookListItemWidget* listItem;

	ui.lwNotebookList->clear();

	for(hasNextNotebook = m_hEvernote->getFirstNotebook(notebook); hasNextNotebook; hasNextNotebook = m_hEvernote->getNextNotebook(notebook))
	{
		//Notebook* newNotebook = new Notebook(notebook);
		QListWidgetItem* iw = new QListWidgetItem(ui.lwNotebookList);
		iw->setSizeHint(QSize(0, 70));
		listItem = new QNotebookListItemWidget(notebook, m_hEvernote, -1, true, ui.lwNotebookList);
		connect(listItem, SIGNAL(notebookMarked()), this, SLOT(addSelectedCount()));
		connect(listItem, SIGNAL(notebookUnmarked()), this, SLOT(subSelectedCount()));
		ui.lwNotebookList->setItemWidget(iw, listItem);
	}

	setProgressIndicator(false);
}

void QDeleteNotebookView::setItemMark(QListWidgetItem *item)
{
	QNotebookListItemWidget* w = (QNotebookListItemWidget*)ui.lwNotebookList->itemWidget(item);
	w->mark();
	//item->setSelected(!item->isSelected());
}

void QDeleteNotebookView::deleteSelectedNotebooks()
{
	QNotebookListItemWidget* currItem;
	QListWidgetItem* listWidgetItem;

	if(m_iSelectedCount == 0 ||
       QMessageBox::question(this,
    		   "Confirm",
    		   QString("Ready to delete %1 notebooks?").arg(m_iSelectedCount),
    		   QMessageBox::Yes|QMessageBox::No,
    		   QMessageBox::Yes) == QMessageBox::No)
		return;

	m_pbDeleteSelected.setEnabled(false);
	setProgressIndicator(true);
	for(int i = 0; i < ui.lwNotebookList->count(); i++)
	{
		listWidgetItem = ui.lwNotebookList->item(i);
		currItem = (QNotebookListItemWidget*)ui.lwNotebookList->itemWidget(listWidgetItem);

		if(currItem->isItemMarked())
		{
			if(m_hEvernote->deleteNotebook(currItem->getNotebook()) == false)
			{
				displayError("Unable to delete notebook", QString(m_hEvernote->getLastErrorString().c_str()));
			}
			else
			{
				currItem->close();
				ui.lwNotebookList->removeItemWidget(listWidgetItem);
				subSelectedCount();
			}
		}
	}

	m_hEvernote->loadNotebookList();
	setProgressIndicator(false);
	loadNotebooksIntoView();
	m_pbDeleteSelected.setEnabled(true);


	((QvernoteView*)m_parent)->loadNotebookList();
}
