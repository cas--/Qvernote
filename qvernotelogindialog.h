#ifndef QVERNOTELOGINDIALOG_H
#define QVERNOTELOGINDIALOG_H

#include <QtGui/QDialog>
#include "ui_qvernotelogindialog.h"

class QvernoteLoginDialog : public QDialog
{
    Q_OBJECT

public:
    QvernoteLoginDialog(QWidget *parent = 0);
    ~QvernoteLoginDialog();

public slots:
    void evernoteLogin();
    void setWorkOffline();

private:
    Ui::QvernoteLoginDialogClass ui;
};

#endif // QVERNOTELOGINDIALOG_H
