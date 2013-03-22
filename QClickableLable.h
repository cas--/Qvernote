/*
 * QClickableLable.h
 *
 *  Created on: May 25, 2010
 *      Author: alexr
 */

#ifndef QCLICKABLELABLE_H_
#define QCLICKABLELABLE_H_

#include <QLabel>

class QClickableLabel : public QLabel
{
	Q_OBJECT

public:
	QClickableLabel(QWidget* parent = 0): QLabel(parent) {}
	~QClickableLabel() {}

	void mousePressEvent(QMouseEvent*)
	{
		emit mousePressed();
	}

signals:
	void mousePressed();

};
#endif /* QCLICKABLELABLE_H_ */
