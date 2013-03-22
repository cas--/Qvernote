/*
 * QNonScrollableGraphicsView.h
 *
 *  Created on: May 29, 2010
 *      Author: alex
 */

#ifndef QNONSCROLLABLEGRAPHICSVIEW_H_
#define QNONSCROLLABLEGRAPHICSVIEW_H_

#include <QGraphicsView>
#include <QDebug>

class QNonScrollableGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	QNonScrollableGraphicsView(QWidget* parent = 0) : QGraphicsView(parent)
	{
		setFocusPolicy( Qt::NoFocus );
	}
	~QNonScrollableGraphicsView() {}

public:
	void resizeEvent ( QResizeEvent* )
	{
		qDebug() << height();
	}
};
#endif /* QNONSCROLLABLEGRAPHICSVIEW_H_ */
