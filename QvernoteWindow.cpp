/*
 * QvernoteWindow.cpp
 *
 *  Created on: Jun 3, 2010
 *      Author: alexr
 */

#include "QvernoteWindow.h"

void BackgroundJob::run()
{
	if(QvernoteWindow::callback(m_pWnd))
		emit jobSucceeded();
	else
		emit jobFailed();
}
