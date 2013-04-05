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
#ifndef QEMAILNOTEDIALOG_H
#define QEMAILNOTEDIALOG_H

#include <QtGui/QDialog>
#include "ui_QEmailNoteDialog.h"
#include "QvernoteAPI.h"

#ifdef Q_WS_MAEMO_5
#include "QMaemoContactEmailListWidget.h"
#endif

class QEmailNoteDialog : public QDialog
{
    Q_OBJECT

public:
    QEmailNoteDialog(Note& note, QvernoteAPI* hEvernote, QWidget *parent = 0);
    ~QEmailNoteDialog();

public slots:
	void sendEmail();
	void showContactList();

private:
    Ui::QEmailNoteDialogClass ui;
    Note m_Note;
    QvernoteAPI* m_hEvernote;
};

#endif // QEMAILNOTEDIALOG_H
