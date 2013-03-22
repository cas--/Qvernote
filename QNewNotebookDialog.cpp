#include "QNewNotebookDialog.h"
#include <QMessageBox>

QNewNotebookDialog::QNewNotebookDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	QObject::connect(ui.pbOk, SIGNAL(clicked()), this, SLOT(createNewNotebook()));
	QObject::connect(this, SIGNAL(reloadNotebookList()), parent, SLOT(reloadNotebookList()));
}

QNewNotebookDialog::~QNewNotebookDialog()
{

}

void QNewNotebookDialog::createNewNotebook()
{
	if(ui.leNotebookName->text().isEmpty())
	{
		QMessageBox::critical(this, trUtf8("Error"), trUtf8("Notebook title is empty"));
	}
	else
	{
#ifdef Q_WS_MAEMO_5
		setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif

		if(QvernoteAPI::Instance()->createNewNotebook(
				ui.leNotebookName->text().toStdString(),
				ui.cbDefault->isChecked(),
				ui.cbPublic->isChecked()) == false)
		{
			QMessageBox::critical(this, trUtf8("Error"), trUtf8("Unable to create notebook"));
		}
		else
		{
			emit reloadNotebookList();
		}
		this->close();
	}
}

