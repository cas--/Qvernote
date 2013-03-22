#include "QNotebookView.h"
#include "QNotebookListItem.h"
#include <QPropertyAnimation>
#include <QDebug>

using namespace qvernote;
using namespace qvernote::api;
using namespace qvernote::view;

QNotebookView::QNotebookView(shared_ptr<QvernoteAPI> hEvernote, QWidget *parent)
    : QWidget(parent), m_hEvernote(hEvernote)
{
	ui.setupUi(this);

	//m_ItemModel = new QNotebookItemModel();
	//ui.lvNotebookList->setModel(&m_ItemModel);
	loadNotebookList();

	QObject::connect(ui.lvNotebookList, SIGNAL(itemClicked(QListWidgetItem *)),  parent, SLOT(loadNotebook(QListWidgetItem* )));
}

QNotebookView::~QNotebookView()
{
}

void QNotebookView::loadNotebookList()
{
	Notebook defaultNotebook;
	m_hEvernote->getNotebookByName(defaultNotebook, m_hEvernote->getDefaultNotebookName());
	//m_ItemModel.addItem(new QNotebookListItem(defaultNotebook, ui.lvNotebookList));
	QListWidgetItem* iw = new QListWidgetItem(ui.lvNotebookList);
	iw->setSizeHint(QSize(0, 50));
	ui.lvNotebookList->setItemWidget(iw, new QNotebookListItem(defaultNotebook, ui.lvNotebookList));
}

