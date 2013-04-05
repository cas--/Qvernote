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
#include "QSyncProgressDialog.h"

QSyncProgressDialog::QSyncProgressDialog(QWidget *parent)
    : QDialog(parent), progressCount(0)
{
	ui.setupUi(this);

	this->setWindowTitle(QString(TITLE_STR).arg(progressCount));
#ifdef Q_WS_MAEMO_5
	setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif
}

QSyncProgressDialog::~QSyncProgressDialog()
{

}

void QSyncProgressDialog::updateProgressData(QString data)
{
	ui.lSyncNote->setText(data);
	this->setWindowTitle(QString(TITLE_STR).arg(++progressCount));
}
