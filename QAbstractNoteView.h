/*
 * QAbstractNoteView.h
 *
 *  Created on: Jul 1, 2010
 *      Author: alex
 */

#ifndef QABSTRACTNOTEVIEW_H_
#define QABSTRACTNOTEVIEW_H_

#include <QWidget>
#include "QvernoteAPI.h"

class QAbstractNoteView
{
public:
	QAbstractNoteView(const Guid& notebookGuid = "") : m_notebookGuid(notebookGuid) { }

	void setFilter(QString filterString) {
		m_sFilterString = filterString;
	}

	virtual void redrawNotes(QString filterString = "") = 0;
	virtual bool nextNote(Note& nextNote) = 0;
	virtual bool prevNote(Note& prevNote) = 0;
protected:
	QString m_sFilterString;
	Guid	m_notebookGuid;
};

#endif /* QABSTRACTNOTEVIEW_H_ */
