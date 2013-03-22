#include "QAboutDialog.h"
#ifdef Q_WS_MAEMO_5
#include "modest_interface_p.h"
#endif

QAboutDialog::QAboutDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	QObject::connect(ui.pbContact, SIGNAL(clicked()), SLOT(openContactMail()));
}

QAboutDialog::~QAboutDialog()
{

}

void QAboutDialog::openContactMail()
{
#ifdef Q_WS_MAEMO_5
	ModestInterface modestIf(ModestInterface::staticInterfaceName(), "/com/nokia/modest", QDBusConnection::sessionBus(), 0);
	modestIf.ComposeMail("burbulator@gmail.com", "", "", "Evernote", "", "");
#endif
	close();
}
