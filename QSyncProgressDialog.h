#ifndef QSYNCPROGRESSDIALOG_H
#define QSYNCPROGRESSDIALOG_H

#include <QtGui/QDialog>
#include "ui_QSyncProgressDialog.h"

#define TITLE_STR "Synchronized %1 items"

class QSyncProgressDialog : public QDialog
{
    Q_OBJECT

public:
    QSyncProgressDialog(QWidget *parent = 0);
    ~QSyncProgressDialog();

    void showEvent(QShowEvent*) {
    	progressCount = 0;
    	this->setWindowTitle(QString(TITLE_STR).arg(progressCount));
    }

public slots:
	void updateProgressData(QString);

private:
    Ui::QSyncProgressDialogClass ui;
    int progressCount;
};

#endif // QSYNCPROGRESSDIALOG_H
