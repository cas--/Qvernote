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
#ifndef QNOTEBOOKLISTITEM_H_
#define QNOTEBOOKLISTITEM_H_

#include <QtGui/QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include "QvernoteAPI.h"

using namespace qvernote::api;
namespace qvernote { namespace view {


class QNotebookListItem : public QWidget {

	Q_OBJECT

public:
	QNotebookListItem(const Notebook& notebook, QWidget* parent = 0);
	virtual ~QNotebookListItem();

	void paintEvent(QPaintEvent * event);
	Notebook getNotebook() { return m_notebook; }

private:
	Notebook m_notebook;
};

}}

#endif /* QNOTEBOOKLISTITEM_H_ */
