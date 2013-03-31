/*
 * Qvernote.h
 *
 *  Created on: May 12, 2010
 *      Author: alex
 */

#ifndef QVERNOTE_H_
#define QVERNOTE_H_

#include "QvernoteView.h"
#include "QvernoteSettings.h"
#include "QvernoteAPI.h"
#include <QThread>


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
