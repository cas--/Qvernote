/***********************************************************************
Copyright (C) 2010 Alex R <burbulator@gmail.com>
Copyright (C) 2013 Calum Lind <calumlind@gmail.com>

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
#include "Qvernote.h"
#include <QDebug>
#include <QDialog>
#include <QMessageBox>

#ifdef Q_WS_MAEMO_5
#include <QMaemo5InformationBox>
#endif


void InitThread::run() {
	qDebug() << __FUNCTION__ << "Qvernote";
	if (m_pQvernote->m_hEvernote->initNoteStore()) {
		qDebug() << "notestore init'd now trying localstore";
		m_pQvernote->m_hEvernote->initLocalStore();
		emit m_pQvernote->noteStoreInitialized();
		return;
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
	qDebug() << __FUNCTION__;
	if(settings->getWorkOnline() == false)
		m_hEvernote->setOnline(false);
	if(m_hEvernote->isOnline()) {
		initThread = new InitThread(this);
		initThread->start();
	} else {
		m_hEvernote->initLocalStore();
		emit noteStoreInitialized();
	}
}


void Qvernote::qvernoteShutdown(QString error) {
#ifdef Q_WS_MAEMO_5
	QMaemo5InformationBox::information(0, QString(trUtf8("Error: ")) + QString(error));
#else
	QMessageBox::critical(0, trUtf8("Error"), error);
#endif
	//m_hEvernote->reInitUserStore();
	Init();
	//exit(1);
}
