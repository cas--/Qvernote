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
#ifndef QATTACHMENTWIDGET_H
#define QATTACHMENTWIDGET_H

#include <QtGui/QWidget>
#include "ui_QAttachmentWidget.h"
#include <QFileInfo>

class QAttachmentWidget : public QWidget
{
    Q_OBJECT

public:
    QAttachmentWidget(QFileInfo file, QWidget *parent = 0);
    QAttachmentWidget(QString fileName, qint64 fileSize, QWidget *parent = 0);
    ~QAttachmentWidget();

    void setAudioType();
    void setPdfType();

    static QByteArray toPng(QAttachmentWidget* widget);
private:
    Ui::QAttachmentWidgetClass ui;
};

#endif // QATTACHMENTWIDGET_H
