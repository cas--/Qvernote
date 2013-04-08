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
#include <QtGui>
#include <QApplication>
#include <QDebug>
#include "Qvernote.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setApplicationName("Qvernote");
	QTranslator aTranslator;
	aTranslator.load("po/qvernote_" + QLocale::system().name());
	a.installTranslator(&aTranslator);

	// Show Qt version
	qDebug() << "Qvernote: built with Qt" << QT_VERSION_STR << "running on" << qVersion();

	Qvernote wndLoader;
	wndLoader.Init();
	return a.exec();
}
