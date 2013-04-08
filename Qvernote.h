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
#ifndef QVERNOTE_H_
#define QVERNOTE_H_

#include "QvernoteView.h"
#include "QvernoteSettings.h"
#include "QvernoteAPI.h"

class Qvernote;
class InitThread : public QThread
{
	Q_OBJECT

public:
	InitThread(Qvernote* parent) : m_pQvernote(parent) {};

protected:
	void run();

private:
	Qvernote* m_pQvernote;
};

class Qvernote : public QObject {
friend class InitThread;
	Q_OBJECT

public:
	Qvernote();
	virtual ~Qvernote();

	void Init();
public slots:
	void qvernoteShutdown(QString);

signals:
	void noteStoreInitialized();
	void noteStoreFailed(QString);

private:
	QvernoteSettings* settings;
	QvernoteView* m_pWnd;
	QvernoteAPI* m_hEvernote;
	InitThread* initThread;
};

#endif /* QVERNOTE_H_ */
