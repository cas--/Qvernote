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
