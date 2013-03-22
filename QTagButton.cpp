/*
 * QTagButton.cpp
 *
 *  Created on: Jun 1, 2010
 *      Author: alex
 */

#include "QTagButton.h"
#include <QDebug>
#ifdef Q_WS_MAEMO_5
#include <QDesktopWidget>
#include <QApplication>
#endif

QTagButton::QTagButton(const Tag& tag, QWidget* parent)
: QPushButton(QString(tag.name.c_str()), parent), parentWidth(parent->width()), m_tag(tag), m_parent(parent)
{
	QString text(tag.name.c_str());
	setFlat(true);
	setCheckable(true);

	buttonSize = QSize(fontMetrics().width(text) + 25, height() + 25);

#ifdef Q_WS_MAEMO_5
	parentWidth = QApplication::desktop()->width();
#endif

	connect(this, SIGNAL(toggled(bool)), this, SLOT(buttonChecked(bool)));
}

QTagButton::~QTagButton() {
	// TODO Auto-generated destructor stub
}

void QTagButton::adjustGeometry(QRect& geom)
{
	//QRect g = geom;

	if(geom.x() + buttonSize.width() >= parentWidth)
	{
		geom.setX(0);
		geom.setY(geom.y() + height() + 25);
	}

	geom.setSize(buttonSize);

	if(m_parent->height() <= geom.bottom())
	{
		//qDebug() << "Resizing widget";
		m_parent->setFixedHeight(m_parent->height() + geom.height());
	}
	//qDebug() << "widget " << parentWidth << "label " << geom.x() << "right " << geom.right();
	setGeometry(geom);
	geom.setX(geom.right() + 15);
}

void QTagButton::buttonChecked(bool state)
{
	emit tagToggled(state, m_tag);
}

/*
QRect QTagButton::getNextGeometry()
{
	QRect geom = geometry();

	if(geom.right() >= width)
	{
		geom.setX(0);
		geom.setY(geom.y() + height() + 15);
		geom.setSize(buttonSize);
	}
	else
	{
		geom.setX(geom.x() + width() + 15);
	}

	return geom;
}
*/
