#include "qvernotelogindialog.h"
#include "QvernoteSettings.h"



QvernoteLoginDialog::QvernoteLoginDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	QObject::connect(ui.pbSignin, SIGNAL(clicked()), this, SLOT(evernoteLogin()));
	QObject::connect(ui.pbWorkOffline, SIGNAL(clicked()), this, SLOT(setWorkOffline()));
	ui.leUsername->setInputMethodHints(Qt::ImhPreferLowercase|Qt::ImhNoAutoUppercase);
	ui.lePassword->setInputMethodHints(Qt::ImhPreferLowercase|Qt::ImhNoAutoUppercase|Qt::ImhHiddenText);
}

QvernoteLoginDialog::~QvernoteLoginDialog()
{

}

void QvernoteLoginDialog::evernoteLogin()
{
	QvernoteSettings::Instance()->setUsername(ui.leUsername->text());
	QvernoteSettings::Instance()->setPassword(ui.lePassword->text());
	QvernoteSettings::Instance()->Store();
	this->done(QDialog::Accepted);
}

void QvernoteLoginDialog::setWorkOffline()
{
	QvernoteSettings::Instance()->setWorkOnline(false);
	QvernoteSettings::Instance()->Store();
	this->done(QDialog::Accepted);
}
