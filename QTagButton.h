/*
 * QTagButton.h
 *
 *  Created on: Jun 1, 2010
 *      Author: alex
 */

#ifndef QTAGBUTTON_H_
#define QTAGBUTTON_H_

#include <QPushButton>
#include "QvernoteAPI.h"



class QTagButton : public QPushButton
{
	Q_OBJECT

public:
	QTagButton(const Tag& tag, QWidget* parent = 0);
	virtual ~QTagButton();

	void adjustGeometry(QRect& geom);
	//QRect getNextGeometry();

public slots:
	void buttonChecked(bool);

signals:
	void tagToggled(bool, Tag);

private:
	QSize buttonSize;
	int	  parentWidth;
	Tag	  m_tag;
	QWidget* m_parent;
};

#endif /* QTAGBUTTON_H_ */
