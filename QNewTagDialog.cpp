#include "QNewTagDialog.h"

QNewTagDialog::QNewTagDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	QObject::connect(ui.pbAddTag, SIGNAL(clicked()), SLOT(setTagName()));
}

QNewTagDialog::~QNewTagDialog()
{

}

void QNewTagDialog::setTagName()
{
	m_NewTagName = ui.leNewTagName->text();
	close();
}
