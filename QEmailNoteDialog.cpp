#include "QEmailNoteDialog.h"

#ifdef Q_WS_MAEMO_5
#include "modest_interface_p.h"
#include <QMaemo5InformationBox>
#else
#include <QMessageBox>
#endif

QEmailNoteDialog::QEmailNoteDialog(Note& note, QvernoteAPI* hEvernote, QWidget *parent)
    : QDialog(parent), m_Note(note), m_hEvernote(hEvernote)
{
	ui.setupUi(this);

	QObject::connect(ui.pbOk, SIGNAL(clicked()), SLOT(sendEmail()));
	QObject::connect(ui.pbSelectContact, SIGNAL(clicked()), SLOT(showContactList()));
}

QEmailNoteDialog::~QEmailNoteDialog()
{

}

void QEmailNoteDialog::sendEmail()
{
	if(m_hEvernote->isOnline())
	{
		if(m_hEvernote->emailNote(m_Note, ui.leEmailAddress->text().toStdString()) == false)
		{
#ifdef Q_WS_MAEMO_5
			QMaemo5InformationBox::information(this->parentWidget(), QString(trUtf8("Sending failed: ")) + QString::fromStdString(m_hEvernote->getLastErrorString()), QMaemo5InformationBox::DefaultTimeout);
#else
			QMessageBox::critical(this, trUtf8("Sending failed"),  QString::fromStdString(m_hEvernote->getLastErrorString()));
#endif
		}
		else
		{
#ifdef Q_WS_MAEMO_5
			QMaemo5InformationBox::information(this->parentWidget(), trUtf8("Email sent"), QMaemo5InformationBox::DefaultTimeout);
#endif
		}
	}
	else
	{
#ifdef Q_WS_MAEMO_5
		ModestInterface modestIf(ModestInterface::staticInterfaceName(), "/com/nokia/modest", QDBusConnection::sessionBus(), 0);
		modestIf.ComposeMail(ui.leEmailAddress->text(), "", "", QString::fromStdString(m_Note.title), QString::fromStdString(m_Note.content), "");
#endif
	}

	close();
}

void QEmailNoteDialog::showContactList()
{
#ifdef Q_WS_MAEMO_5
	setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
	QEmailContactList* ecl = new QEmailContactList(this);
	setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
	ecl->exec();
	ui.leEmailAddress->setText(ecl->getSelectedEmail());
#else
	QMessageBox::information(this, trUtf8("Cannot open contact list"), trUtf8("Maemo specific code"));
#endif
}
