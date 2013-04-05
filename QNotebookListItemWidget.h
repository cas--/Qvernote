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
#ifndef QNOTEBOOKLISTITEMWIDGET_H
#define QNOTEBOOKLISTITEMWIDGET_H

#include <QtGui/QWidget>
#include "ui_QNotebookListItemWidget.h"
#include "QvernoteAPI.h"
#include <QGraphicsPixmapItem>



class QNotebookListItemWidget : public QWidget
{
    Q_OBJECT

public:
    QNotebookListItemWidget(Notebook& notebook, QvernoteAPI* hEvernote, int noteCount, bool isDeleteMode, QWidget *parent = 0);
    ~QNotebookListItemWidget();

    Notebook& getNotebook() { return m_Notebook; }

    bool	isItemMarked() { return m_bItemMarked; }
    void mark();


signals:
	void notebookMarked();
	void notebookUnmarked();

private:
    Ui::QNotebookListItemWidgetClass ui;
    Notebook m_Notebook;
    bool	m_bDeleteMode;
    bool    m_bItemMarked;
    bool	m_bMouseMoved;
};

#endif // QNOTEBOOKLISTITEMWIDGET_H
