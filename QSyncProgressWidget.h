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
#ifndef QSYNCPROGRESSWIDGET_H
#define QSYNCPROGRESSWIDGET_H

#include <QtGui/QLabel>
#include <QtGui/qpainter.h>
#include "QEnmlReader.h"

class QSyncProgressWidget : public QLabel
{
    Q_OBJECT

public:
    QSyncProgressWidget(QWidget* parent = 0)
		:QLabel(trUtf8("Please wait"), parent) {
    	setAlignment(Qt::AlignCenter);
    	setAttribute(Qt::WA_TranslucentBackground);
    	setWordWrap(true);
    };

    QSize sizeHint() const {
    	return QSize(500, 100);
    }

    QSize size() const {
    	return sizeHint();
    }

    void showEvent(QShowEvent*) {
		progressCount = 0;
		setText(QString(trUtf8("Please wait...")));
	}

public slots:
   	void updateProgressData(QString data) {
   		setText(QString(trUtf8("%1\n\n%2")).arg(++progressCount).arg(data));
   	}

protected:
	void paintEvent(QPaintEvent *event) {
		QPainter p(this);
		//p.setBrush(QColor(128, 128, 128, 128));
		p.setBrush(QColor(0, 0, 0, 128));
		p.setPen(Qt::NoPen);
		p.drawRoundedRect(rect(), 25, 25);
		setStyleSheet(trUtf8("color:#999999;font-size:26px"));
		p.end();
		QLabel::paintEvent(event);
    }

private:
    int progressCount;
};

#endif // QSYNCPROGRESSWIDGET_H
