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
#include "QEmailNoteDialog.h"

#ifdef Q_WS_MAEMO_5
#include "modest_interface_p.h"
#include <QMaemo5InformationBox>
#else
#include <QMessageBox>
#endif

QEmailNoteDialog::QEmailNoteDialog(Note& note, QvernoteAPI* hEvernote, QWidget *parent)
    : QDialog(parent), m_Note(note), m_hEvernote(hEvernote)
{
	ui.setupUi(this);

	QObject::connect(ui.pbOk, SIGNAL(clicked()), SLOT(sendEmail()));
	QObject::connect(ui.pbSelectContact, SIGNAL(clicked()), SLOT(showContactList()));
}

QEmailNoteDialog::~QEmailNoteDialog()
{

}

void QEmailNoteDialog::sendEmail()
{
	if(m_hEvernote->isOnline())
	{
		if(m_hEvernote->emailNote(m_Note, ui.leEmailAddress->text().toStdString()) == false)
		{
#ifdef Q_WS_MAEMO_5
			QMaemo5InformationBox::information(this->parentWidget(), QString(trUtf8("Sending failed: ")) + QString::fromStdString(m_hEvernote->getLastErrorString()), QMaemo5InformationBox::DefaultTimeout);
#else
			QMessageBox::critical(this, trUtf8("Sending failed"),  QString::fromStdString(m_hEvernote->getLastErrorString()));
#endif
		}
		else
		{
#ifdef Q_WS_MAEMO_5
			QMaemo5InformationBox::information(this->parentWidget(), trUtf8("Email sent"), QMaemo5InformationBox::DefaultTimeout);
#endif
		}
	}
	else
	{
#ifdef Q_WS_MAEMO_5
		ModestInterface modestIf(ModestInterface::staticInterfaceName(), "/com/nokia/modest", QDBusConnection::sessionBus(), 0);
		modestIf.ComposeMail(ui.leEmailAddress->text(), "", "", QString::fromStdString(m_Note.title), QString::fromStdString(m_Note.content), "");
#endif
	}

	close();
}

void QEmailNoteDialog::showContactList()
{
#ifdef Q_WS_MAEMO_5
	setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
	QEmailContactList* ecl = new QEmailContactList(this);
	setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
	ecl->exec();
	ui.leEmailAddress->setText(ecl->getSelectedEmail());
#else
	QMessageBox::information(this, trUtf8("Cannot open contact list"), trUtf8("Maemo specific code"));
#endif
}
