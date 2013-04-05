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
#ifndef QNONSCROLLABLELISTWIDGET_H_
#define QNONSCROLLABLELISTWIDGET_H_

#include <QListWidget>
#include <QDebug>

class QNonScrollableListWidget : public QListWidget
{
	Q_OBJECT

public:
	QNonScrollableListWidget(QWidget* parent = 0) : QListWidget(parent), m_listItemHeight(70) {
		setFixedHeight(m_listItemHeight * 7);
	}
	~QNonScrollableListWidget() {}

	void setListItemHeight(int height)
	{
		m_listItemHeight = height;
	}
public slots:
	void rowsInserted ( const QModelIndex  & /*parent*/, int /*start*/, int /*end*/ )
	{
		//qDebug() << "Items height:" << count()*m_listItemHeight << "ListWidget height:" << height();
		if(count()*m_listItemHeight > height())
			setFixedHeight(height() + m_listItemHeight);
	}

private:
	int m_listItemHeight;
};
#endif /* QNONSCROLLABLELISTWIDGET_H_ */
