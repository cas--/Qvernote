#ifndef QNEWTAGDIALOG_H
#define QNEWTAGDIALOG_H

#include <QtGui/QDialog>
#include "ui_QNewTagDialog.h"

class QNewTagDialog : public QDialog
{
    Q_OBJECT

public:
    QNewTagDialog(QWidget *parent = 0);
    ~QNewTagDialog();

    QString getTagName() { return m_NewTagName; }

public slots:
	void setTagName();

private:
    Ui::QNewTagDialogClass ui;
    QString m_NewTagName;
};

#endif // QNEWTAGDIALOG_H
