/*
 * Qvernote.cpp
 *
 *  Created on: May 12, 2010
 *      Author: alex
 */

#include "Qvernote.h"
#include "qvernotelogindialog.h"
#include <QDebug>
#include <QDialog>
#include <QMessageBox>
#include "oauth/oauthwindow.h"
#include "oauth/oauthtokenizer.h"

#ifdef Q_WS_MAEMO_5
#include <QMaemo5InformationBox>
#endif



void InitThread::run()
{
	QLOG_DEBUG() << "InitThread::run";

	if(m_pQvernote->m_hEvernote->checkVersion() == true) {
		if(m_pQvernote->m_hEvernote->checkAuthenticateToken())
		{
			if(m_pQvernote->m_hEvernote->initNoteStore() == true)
			{
				m_pQvernote->m_hEvernote->initLocalStore();
				emit m_pQvernote->noteStoreInitialized();
				return;
			}
		}
	}

	emit m_pQvernote->noteStoreFailed(QString(m_pQvernote->m_hEvernote->getLastErrorString().c_str()));
}

Qvernote::Qvernote()
: initThread(NULL)
{
	settings = QvernoteSettings::Instance();
	m_hEvernote = QvernoteAPI::Instance();
	m_pWnd = new QvernoteView();
	m_pWnd->show();

	QObject::connect(this, SIGNAL(noteStoreInitialized()), m_pWnd, SLOT(initView()));
	QObject::connect(this, SIGNAL(noteStoreFailed(QString)), this, SLOT(qvernoteShutdown(QString)));

}

Qvernote::~Qvernote() {
	QvernoteSettings::Instance()->Store();

	if(initThread)
		delete initThread;
}


void Qvernote::Init() {
	QLOG_DEBUG() << "Init";

	if(settings->getOAuthToken().length() == 0) {
		OAuthWindow window;
		window.exec();
		if (window.error) {
			QLOG_DEBUG() << "Error: " << window.errorMessage;
			return;
		}
		if (window.response == "")
			return;

		settings->setOAuthToken(window.response);
	}

	if(settings->getWorkOnline() == false)
	{
		m_hEvernote->setOnline(false);
	}

	if(m_hEvernote->isOnline())
	{
		initThread = new InitThread(this);
		initThread->start();
	}
	else
	{
		m_hEvernote->initLocalStore();
		emit noteStoreInitialized();
	}
}

void Qvernote::qvernoteShutdown(QString error)
{
#ifdef Q_WS_MAEMO_5
	QMaemo5InformationBox::information(0, QString(trUtf8("Error:  ")) + QString(error));
#else
	QMessageBox::critical(0, trUtf8("Error"), error);
#endif
	//m_hEvernote->reInitUserStore();
	Init();
	//exit(1);
}
