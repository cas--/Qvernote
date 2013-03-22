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
