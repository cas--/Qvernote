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
#include "QAttachmentWidget.h"
#include <QBuffer>
#include <QPixmap>

QAttachmentWidget::QAttachmentWidget(QFileInfo file, QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);

	ui.lFileName->setText(file.fileName());
	ui.lFileSize->setText(QString("%1 KB").arg(file.size()/1024));
}

QAttachmentWidget::QAttachmentWidget(QString fileName, qint64 fileSize, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.lFileName->setText(QString::fromLocal8Bit(fileName.toStdString().c_str(), fileName.length()));
	ui.lFileSize->setText(QString("%1 KB").arg(fileSize/1024));
}

QAttachmentWidget::~QAttachmentWidget()
{

}

void QAttachmentWidget::setAudioType()
{
	ui.lAttachmentIcon->setPixmap(QPixmap(":/IconAdd/file-audio.png"));
}

void QAttachmentWidget::setPdfType()
{
	ui.lAttachmentIcon->setPixmap(QPixmap(":/IconAdd/file-pdf.png"));
}

QByteArray QAttachmentWidget::toPng(QAttachmentWidget* widget)
{
	QPixmap p = QPixmap::grabWidget(widget);
	QByteArray bytes;
	QBuffer buffer(&bytes);
	buffer.open(QIODevice::WriteOnly);
	p.save(&buffer, "PNG");

	return bytes;
}
