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
#ifndef QCLICKABLELABEL_H_
#define QCLICKABLELABEL_H_

#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QGraphicsBlurEffect>
#include <QPropertyAnimation>
#include <QDebug>

class QClickableLabel : public QLabel
{
	Q_OBJECT

public:
	QClickableLabel(QWidget* parent = 0): QLabel(parent), isMousePressed(false), isBorderEnabled(true), isClickAnimEnabled(true) {
		//setFrameStyle(QFrame::NoFrame);
		setScaledContents(true);
		//setAttribute(Qt::WA_TranslucentBackground);
		anim = new QPropertyAnimation(this, "geometry");
	}

	~QClickableLabel() { delete anim; }

	void setBorderEnabled(bool borderEnabled) { isBorderEnabled = borderEnabled; }
	void setClickAnimEnabled(bool clickAnimEnabled) { isClickAnimEnabled = clickAnimEnabled; }
	void mousePressEvent(QMouseEvent* e)
	{
		//this->setFrameStyle(QFrame::Box);
		//this->setGraphicsEffect(new QGraphicsBlurEffect(this));
		if(isClickAnimEnabled)
			setGeometry(QRect(QPoint(geometry().x(), geometry().y()), size() - QSize(2,2)));
		mousePos = e->pos();
		isMousePressed = true;
		e->accept();
		emit mousePressed();
		QLabel::mousePressEvent(e);
	}

	void mouseReleaseEvent(QMouseEvent* e)
	{
		//this->setFrameStyle(QFrame::NoFrame);
		//setFixedSize(size() + QSize(2,2));
		if(isClickAnimEnabled)
			setGeometry(QRect(QPoint(geometry().x(), geometry().y()), size() + QSize(2,2)));
		//this->setGraphicsEffect(NULL);
		emit mouseReleased();

		if(isMousePressed)
		{
			isMousePressed = false;
			emit clicked();
		}

		QLabel::mouseReleaseEvent(e);
	}

	void mouseMoveEvent(QMouseEvent* e)
	{
		//qDebug() << geometry() << e->pos() << e->globalPos() << e->x() << e->y();
		//if(!geometry().contains(e->globalPos()))
		QRect r(mousePos, e->pos());
		//qDebug() << r.height() << r.width();
		if(abs(r.height()) > 20 || abs(r.width()) > 20)
			isMousePressed = false;

		QLabel::mouseMoveEvent(e);
	}


	void paintEvent(QPaintEvent* e) {

		if(isBorderEnabled)
		{
			QPainter painter(this);

			if(isMousePressed) {
				//qDebug() << "Paint!";
				//painter.setBrush(QBrush(QColor("#37b4fc")));
				painter.setRenderHint(QPainter::Antialiasing);
				painter.setPen(QPen(QColor("#37b4fc"), 2));
				painter.drawRoundedRect(QRect(QPoint(1, 1), size() - QSize(2,2)), 8, 8);
			} else {
				//painter.setPen(QPen(QBrush(palette().color(QPalette::Background)), 2));
			}
			//painter.drawRoundedRect(QRect(QPoint(0, 0), size()), 2, 2);
		}

		QLabel::paintEvent(e);
	}

	void animHide() {
		//qDebug() << __FUNCTION__ << width() << height() << geometry() << size();
		QObject::connect(anim, SIGNAL(finished()), this, SLOT(hide()), Qt::QueuedConnection);
		anim->setStartValue(QRect(QPoint(geometry().x(), geometry().y()), size()));
		anim->setEndValue(QRect(geometry().x() + size().width() / 2, geometry().y() + size().height() / 2, 0, 0));
		anim->setDuration(150);
		anim->start();
	}

	void animShow(const QSize& finalSize = QSize(48, 48)) {
		setGeometry(QRect(QPoint(geometry().x(), geometry().y()), QSize(0,0)));
		//qDebug() << __FUNCTION__ << width() << height() << geometry() << size();
		show();
		QObject::disconnect(anim, SIGNAL(finished()), this, SLOT(hide()));
		anim->setStartValue(QRect(geometry().x() + finalSize.width() / 2, geometry().y() + finalSize.height() / 2, 0, 0));
		anim->setEndValue(QRect(QPoint(geometry().x(), geometry().y()), finalSize));
		anim->setDuration(150);
		anim->start();
	}

signals:
	void mousePressed();
	void mouseReleased();
	void clicked();
private:
	QPoint	mousePos;
	bool isMousePressed;
	bool isBorderEnabled;
	bool isClickAnimEnabled;
	QPropertyAnimation* anim;

};

#endif /* QCLICKABLELABEL_H_ */
