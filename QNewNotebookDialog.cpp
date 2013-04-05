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
#include "QNewNotebookDialog.h"
#include <QMessageBox>

QNewNotebookDialog::QNewNotebookDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	QObject::connect(ui.pbOk, SIGNAL(clicked()), this, SLOT(createNewNotebook()));
	QObject::connect(this, SIGNAL(reloadNotebookList()), parent, SLOT(reloadNotebookList()));
}

QNewNotebookDialog::~QNewNotebookDialog()
{

}

void QNewNotebookDialog::createNewNotebook()
{
	if(ui.leNotebookName->text().isEmpty())
	{
		QMessageBox::critical(this, trUtf8("Error"), trUtf8("Notebook title is empty"));
	}
	else
	{
#ifdef Q_WS_MAEMO_5
		setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif

		if(QvernoteAPI::Instance()->createNewNotebook(
				ui.leNotebookName->text().toStdString(),
				ui.cbDefault->isChecked(),
				ui.cbPublic->isChecked()) == false)
		{
			QMessageBox::critical(this, trUtf8("Error"), trUtf8("Unable to create notebook"));
		}
		else
		{
			emit reloadNotebookList();
		}
		this->close();
	}
}

