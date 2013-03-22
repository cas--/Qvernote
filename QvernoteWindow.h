/*
 * QvernoteWindow.h
 *
 *  Created on: May 27, 2010
 *      Author: alexr
 */

#ifndef QVERNOTEWINDOW_H_
#define QVERNOTEWINDOW_H_

#include <QtGui>
#include <QtGui/QMainWindow>
#include "QvernoteSettings.h"
#include <QMessageBox>
#include <QThread>
#include <boost/shared_ptr.hpp>

#ifdef Q_WS_MAEMO_5
#include <QMaemo5InformationBox>
#endif

using namespace boost;

class QvernoteWindow;
//bool QvernoteWindow::callback(QvernoteWindow* pWnd);

class BackgroundJob : public QThread
{
	Q_OBJECT

public:
	BackgroundJob(QvernoteWindow* pWnd) : m_pWnd(pWnd) {
		//QObject::connect(this, SIGNAL(jobSucceeded()), pWnd, SLOT(jobDoneCallback()));
	}
	BackgroundJob(const BackgroundJob& c): QThread() { m_pWnd = c.m_pWnd; }
	void operator=(const BackgroundJob& c) { m_pWnd = c.m_pWnd; }
	void run();

signals:
	void backgroundJobStart();
	void jobSucceeded();
	void jobFailed();

private:
	QvernoteWindow* m_pWnd;
};

class QvernoteWindow : public QMainWindow
{
	friend class BackgroundJob;

	Q_OBJECT

public:
	QvernoteWindow(QWidget* parent = 0)
	: QMainWindow(parent),m_settings(QvernoteSettings::Instance())
	{
#ifdef Q_WS_MAEMO_5
		connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(orientationChanged()));
		setAttribute(Qt::WA_Maemo5StackedWindow);
		//setAttribute(Qt::WA_PaintOnScreen);
		setDisplayOrientation(m_settings->getDisplayOrientation());
#endif

		m_pBackgroundJob = shared_ptr<BackgroundJob>(new BackgroundJob(this));

		connect(m_pBackgroundJob.get(), SIGNAL(jobSucceeded()), this, SLOT(jobDoneCallback()));
		connect(m_pBackgroundJob.get(), SIGNAL(jobFailed()), this, SLOT(jobFailedCallback()));
		//connect(m_pBackgroundJob.get(), SIGNAL(backgroundJobStart()), this, SLOT(backgroundJobCallback()));

	}

	~QvernoteWindow() {}

	void setProgressIndicator(bool status)
	{
#ifdef Q_WS_MAEMO_5
		setAttribute(Qt::WA_Maemo5ShowProgressIndicator, status);
#endif
	}

	void displayError(const QString& title, const QString& errorString)
	{
#ifdef Q_WS_MAEMO_5
		QMaemo5InformationBox::information(this, title + ": " + errorString, QMaemo5InformationBox::DefaultTimeout);
#else
		QMessageBox::critical(this, title, errorString);
#endif
	}

	static void displayError(QWidget* parent, const QString& title, const QString& errorString)
	{
#ifdef Q_WS_MAEMO_5
		QMaemo5InformationBox::information(parent, title + ": " + errorString, QMaemo5InformationBox::DefaultTimeout);
#else
		QMessageBox::critical(parent, title, errorString);
#endif
	}

	void setDisplayOrientation(QvernoteSettings::DisplayOrientation displayOrientation)
	{
#ifdef Q_WS_MAEMO_5
		switch(displayOrientation) {
		case QvernoteSettings::DO_PORTRAIT:
			setAttribute(Qt::WA_Maemo5PortraitOrientation, true);
			break;
		case QvernoteSettings::DO_LANDSCAPE:
			setAttribute(Qt::WA_Maemo5LandscapeOrientation, true);
			break;
		case QvernoteSettings::DO_AUTO:
			setAttribute(Qt::WA_Maemo5AutoOrientation, true);
			break;
		}

		m_settings->setDisplayOrientation(displayOrientation);
#endif
	}

	void displayInformationBox(QString text)
	{
#ifdef Q_WS_MAEMO_5
		QMaemo5InformationBox::information(this, "<center>" + text + "</center>", QMaemo5InformationBox::DefaultTimeout);
#endif
	}

	virtual bool backgroundJobCallback() { return true; }

public slots:
	virtual void orientationChanged() {};
	virtual void jobDoneCallback() {};
	virtual void jobFailedCallback() {};

private:
	static bool callback(QvernoteWindow* pWnd) {
		return pWnd->backgroundJobCallback();
	}

protected:
	QvernoteSettings* m_settings;
	shared_ptr<BackgroundJob>	  m_pBackgroundJob;

};
#endif /* QVERNOTEWINDOW_H_ */
