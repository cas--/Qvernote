/*
 * QNoteViewNavigationWidget.h
 *
 *  Created on: Jul 10, 2010
 *      Author: alex
 */

#ifndef QNOTEVIEWNAVIGATIONWIDGET_H_
#define QNOTEVIEWNAVIGATIONWIDGET_H_

#include <QWidget>
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include "QClickableLabel.h"

class QNoteViewNavigationWidget : public QWidget
{
	Q_OBJECT

public:
	QNoteViewNavigationWidget(QWidget* parent) : QWidget(parent), m_Parent(parent) {
		qDebug() << __FUNCTION__ << size();
		qDebug() << __FUNCTION__ << baseSize();
		qDebug() << __FUNCTION__ << sizeHint();
		setAttribute(Qt::WA_TransparentForMouseEvents);
		clZoomin = new QClickableLabel(parent);
		clZoomin->setBorderEnabled(false);
		clZoomin->setForegroundRole(QPalette::Window);
		clZoomin->setPixmap(QPixmap(":/IconAdd/note_zoomin.png"));
		//clZoomin->setGeometry(parent->width() - 65, parent->height() / 2 - 50, 48, 48);
		//clZoomin->show();

		clZoomout = new QClickableLabel(parent);
		clZoomout->setBorderEnabled(false);
		clZoomout->setForegroundRole(QPalette::Window);
		clZoomout->setPixmap(QPixmap(":/IconAdd/note_zoomout.png"));
		//clZoomout->setGeometry(parent->width() - 65, parent->height() / 2 + 50, 48, 48);
		//clZoomout->show();

		clPrev = new QClickableLabel(parent);
		clPrev->setBorderEnabled(false);
		clPrev->setForegroundRole(QPalette::Window);
		clPrev->setPixmap(QPixmap(":/IconAdd/general_back.png"));
		//clPrev->setGeometry(parent->width() / 2 - 50, parent->height() - 65, 48, 48);
		//clPrev->show();

		clNext = new QClickableLabel(parent);
		clNext->setBorderEnabled(false);
		clNext->setForegroundRole(QPalette::Window);
		clNext->setPixmap(QPixmap(":/IconAdd/general_forward.png"));
		//clNext->setGeometry(parent->width() / 2 + 50, parent->height() - 65, 48, 48);
		//clNext->show();

		clFullscreen = new QClickableLabel(parent);
		clFullscreen->setBorderEnabled(false);
		clFullscreen->setForegroundRole(QPalette::Window);
		clFullscreen->setPixmap(QPixmap(":/IconAdd/general_fullsize_b.png"));
		//clFullscreen->setGeometry(parent->width() - 65, parent->height() - 65, 48, 48);

		//setAttribute(Qt::WA_TransparentForMouseEvents);
	}

	void showControls() {
		clZoomin->setGeometry(m_Parent->width() - 65, m_Parent->height() / 2 - 50, 48, 48);
		clZoomout->setGeometry(m_Parent->width() - 65, m_Parent->height() / 2 + 50, 48, 48);
		clPrev->setGeometry(m_Parent->width() / 2 - 50, m_Parent->height() - 65, 48, 48);
		clNext->setGeometry(m_Parent->width() / 2 + 50, m_Parent->height() - 65, 48, 48);
		clFullscreen->setGeometry(m_Parent->width() - 65, m_Parent->height() - 65, 48, 48);

		show();
	}
private:
	QClickableLabel	*clZoomin, *clZoomout, *clNext, *clPrev, *clFullscreen;
	QWidget* m_Parent;
};
#endif /* QNOTEVIEWNAVIGATIONWIDGET_H_ */
