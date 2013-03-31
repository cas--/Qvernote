#include <QtGui>
#include <QApplication>
#include <QDebug>
#include "QvernoteAPI.h"
#include "Qvernote.h"
#include "QvernoteView.h"


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
