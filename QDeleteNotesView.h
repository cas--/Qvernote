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
#ifndef QDELETENOTESVIEW_H
#define QDELETENOTESVIEW_H

#include <QtGui/QMainWindow>
#include "ui_QDeleteNotesView.h"
#include "QvernoteWindow.h"
#include "QvernoteAPI.h"



class QDeleteNotesView : public QvernoteWindow
{
    Q_OBJECT

public:
    QDeleteNotesView(const QString& notebookName, QWidget *parent = 0);
    ~QDeleteNotesView();

    void loadNotesIntoView();
    void showEvent(QShowEvent *) {
       	loadNotesIntoView();
    }

public slots:
	void setItemMark(QListWidgetItem *item);
	void deleteSelectedNotes();
	void copySelectedNotes();
	void moveSelectedNotes();
    void addSelectedCount() { m_lSelectedCount.setText(QString(trUtf8("%1 notes")).arg(++m_iSelectedCount)); }
    void subSelectedCount() { m_lSelectedCount.setText(QString(trUtf8("%1 notes")).arg(--m_iSelectedCount)); }

private:
    Ui::QDeleteNotesViewClass ui;
    QvernoteAPI* m_hEvernote;
    QString m_notebookName;
    QLabel	m_lSelectedCount;
    QPushButton m_pbDeleteSelected;
    QPushButton m_pbCopyNotes;
    QPushButton m_pbMoveNotes;
    int		m_iSelectedCount;
    QWidget* m_parent;
};

#endif // QDELETENOTESVIEW_H
