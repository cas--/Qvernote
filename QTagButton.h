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
