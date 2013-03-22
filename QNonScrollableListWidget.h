/*
 * QNonScrollableListWidget.h
 *
 *  Created on: May 29, 2010
 *      Author: alex
 */

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
	void rowsInserted ( const QModelIndex  & parent, int start, int end )
	{
		//qDebug() << "Items height:" << count()*m_listItemHeight << "ListWidget height:" << height();
		if(count()*m_listItemHeight > height())
			setFixedHeight(height() + m_listItemHeight);
	}

private:
	int m_listItemHeight;
};
#endif /* QNONSCROLLABLELISTWIDGET_H_ */
