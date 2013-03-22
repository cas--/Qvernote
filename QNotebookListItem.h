/*
 * QNotebookListItem.h
 *
 *  Created on: May 10, 2010
 *      Author: alexr
 */

#ifndef QNOTEBOOKLISTITEM_H_
#define QNOTEBOOKLISTITEM_H_

#include <QtGui/QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include "QvernoteAPI.h"

using namespace qvernote::api;
namespace qvernote { namespace view {


class QNotebookListItem : public QWidget {

	Q_OBJECT

public:
	QNotebookListItem(const Notebook& notebook, QWidget* parent = 0);
	virtual ~QNotebookListItem();

	void paintEvent(QPaintEvent * event);
	Notebook getNotebook() { return m_notebook; }

private:
	Notebook m_notebook;
};

}}

#endif /* QNOTEBOOKLISTITEM_H_ */
