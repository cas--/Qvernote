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
#include "QAboutDialog.h"
#ifdef Q_WS_MAEMO_5
#include "modest_interface_p.h"
#endif

QAboutDialog::QAboutDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	QObject::connect(ui.pbContact, SIGNAL(clicked()), SLOT(openContactMail()));
}

QAboutDialog::~QAboutDialog()
{

}

void QAboutDialog::openContactMail()
{
#ifdef Q_WS_MAEMO_5
	ModestInterface modestIf(ModestInterface::staticInterfaceName(), "/com/nokia/modest", QDBusConnection::sessionBus(), 0);
	modestIf.ComposeMail("calumlind@gmail.com", "", "", "Qvernote", "", "");
#endif
	close();
}
