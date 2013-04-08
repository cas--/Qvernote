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
#ifndef QNOTEBOOKVIEW_H
#define QNOTEBOOKVIEW_H

#include <QtGui/QWidget>
#include "ui_QNotebookView.h"
#include "QvernoteAPI.h"
#include "QNotebookListItem.h"

namespace qvernote { namespace view {
using namespace api;

class QNotebookView : public QWidget
{
    Q_OBJECT

public:
    QNotebookView(boost::shared_ptr<QvernoteAPI> hEvernote, QWidget *parent = 0);
    ~QNotebookView();

private:
    void loadNotebookList();

private:
    Ui::QNotebookViewClass ui;
    boost::shared_ptr<QvernoteAPI> m_hEvernote;
};

}}
#endif // QNOTEBOOKVIEW_H
