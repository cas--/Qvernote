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
#ifndef QSYNCPROGRESSDIALOG_H
#define QSYNCPROGRESSDIALOG_H

#include <QtGui/QDialog>
#include "ui_QSyncProgressDialog.h"

#define TITLE_STR "Synchronized %1 items"

class QSyncProgressDialog : public QDialog
{
    Q_OBJECT

public:
    QSyncProgressDialog(QWidget *parent = 0);
    ~QSyncProgressDialog();

    void showEvent(QShowEvent*) {
    	progressCount = 0;
    	this->setWindowTitle(QString(TITLE_STR).arg(progressCount));
    }

public slots:
	void updateProgressData(QString);

private:
    Ui::QSyncProgressDialogClass ui;
    int progressCount;
};

#endif // QSYNCPROGRESSDIALOG_H
