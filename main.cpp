#include <QtGui>
#include <QApplication>
#include <QDebug>
#include "QvernoteAPI.h"
#include "Qvernote.h"
#include "QvernoteView.h"
#include "logger/qslog.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setApplicationName("Qvernote");
	QTranslator aTranslator;
	aTranslator.load("po/qvernote_" + QLocale::system().name());
	a.installTranslator(&aTranslator);

	// Setup the QLOG functions for debugging & messages
	QsLogging::Logger& logger = QsLogging::Logger::instance();
	logger.setLoggingLevel(QsLogging::DebugLevel);
	const QString sLogPath("/home/maemo/qvernote/log");

	QsLogging::DestinationPtr fileDestination(
		QsLogging::DestinationFactory::MakeFileDestination(sLogPath) );
	QsLogging::DestinationPtr debugDestination(
		QsLogging::DestinationFactory::MakeDebugOutputDestination() );
	logger.addDestination(debugDestination.get());
	logger.addDestination(fileDestination.get());

	// Show Qt version.  This is useful for debugging
	QLOG_INFO() << "Built with Qt" << QT_VERSION_STR << "running on" << qVersion();

	Qvernote wndLoader;
	wndLoader.Init();
	return a.exec();
}
