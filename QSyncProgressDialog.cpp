#include "QSyncProgressDialog.h"

QSyncProgressDialog::QSyncProgressDialog(QWidget *parent)
    : QDialog(parent), progressCount(0)
{
	ui.setupUi(this);

	this->setWindowTitle(QString(TITLE_STR).arg(progressCount));
#ifdef Q_WS_MAEMO_5
	setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif
}

QSyncProgressDialog::~QSyncProgressDialog()
{

}

void QSyncProgressDialog::updateProgressData(QString data)
{
	ui.lSyncNote->setText(data);
	this->setWindowTitle(QString(TITLE_STR).arg(++progressCount));
}
