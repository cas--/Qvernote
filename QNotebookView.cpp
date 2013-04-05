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

