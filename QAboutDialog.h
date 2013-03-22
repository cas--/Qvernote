#ifndef QABOUTDIALOG_H
#define QABOUTDIALOG_H

#include <QtGui/QDialog>
#include "ui_QAboutDialog.h"

class QAboutDialog : public QDialog
{
    Q_OBJECT

public:
    QAboutDialog(QWidget *parent = 0);
    ~QAboutDialog();

public slots:
	void openContactMail();

private:
    Ui::QAboutDIalogClass ui;
};

#endif // QABOUTDIALOG_H
