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
#ifndef QTRASHWINDOW_H
#define QTRASHWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_QTrashWindow.h"
#include "QvernoteWindow.h"
#include "QvernoteAPI.h"



class QTrashWindow : public QvernoteWindow
{
    Q_OBJECT

public:
    QTrashWindow(QWidget *parent = 0);
    ~QTrashWindow();

    void loadNotesIntoView();
    void showEvent(QShowEvent*) {
    	loadNotesIntoView();
    }

public slots:
	void setItemMark(QListWidgetItem *item);
	void deleteSelectedNotes();
	void restoreSelectedNotes();
	void addSelectedCount() { m_lSelectedCount.setText(QString(trUtf8("%1 notes")).arg(++m_iSelectedCount)); }
	void subSelectedCount() { m_lSelectedCount.setText(QString(trUtf8("%1 notes")).arg(--m_iSelectedCount)); }

private:
	Ui::QTrashWindowClass ui;
	QvernoteAPI* m_hEvernote;
	QLabel	m_lSelectedCount;
	QPushButton m_pbDeleteSelected;
	QPushButton m_pbRestoreSelected;
	int		m_iSelectedCount;
	QWidget* m_parent;
};

#endif // QTRASHWINDOW_H
