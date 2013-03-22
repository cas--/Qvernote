/*
 * QNotebookListItem.cpp
 *
 *  Created on: May 10, 2010
 *      Author: alexr
 */

#include "QNotebookListItem.h"
#include <QPainter>
#include <QPaintDevice>
#include <QDebug>
using namespace qvernote;
using namespace qvernote::view;

QNotebookListItem::QNotebookListItem(const Notebook& notebook, QWidget* parent)
: 	QWidget(parent),
	m_notebook(notebook)
{
	//setGeometry(QRect(0, 0, parent->width(), 550));
	setMinimumSize(0, 50);

	//qDebug() << parent->width();
}

QNotebookListItem::~QNotebookListItem() {
	// TODO Auto-generated destructor stub
}

void QNotebookListItem::paintEvent(QPaintEvent * event)
{
	QPainter p(this);
	p.setRenderHint(QPainter::TextAntialiasing);
	p.setRenderHint(QPainter::HighQualityAntialiasing);

	//p.setViewport(0, 0, 500, 50);
	if(m_notebook.defaultNotebook) {
		//p.setBrush(QBrush(Qt::darkGreen));
		//p.fillRect(0, 0, 800, 50, p.brush());
		//p.setPen(Qt::black);
		//p.drawRoundedRect(QRect(0, 0, 775, 50), 20, 20);
		//p.drawImage(QRect(0, 0, 800, 50), QImage(":/IconAdd/tile_img.png"));
		//p.drawRoundRect(QRect(0, 0, 795, 45), 5, 5);

		//p.setPen(Qt::white);
		p.drawText(20, 20, QString(m_notebook.name.c_str()));
		p.setFont(QFont("Tahoma", 5, 5, true));
		p.drawText(20, 40, QString("default"));
	} else {
		p.drawText(20, 20, QString(m_notebook.name.c_str()));
//		setBackgroundColor(Qt::white);
//		setText(QString(m_notebook.name.c_str()));
	}


}
