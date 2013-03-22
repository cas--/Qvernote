#ifndef QNEWNOTEBOOKDIALOG_H
#define QNEWNOTEBOOKDIALOG_H

#include <QtGui/QDialog>
#include "ui_QNewNotebookDialog.h"
#include "QvernoteAPI.h"



class QNewNotebookDialog : public QDialog
{
    Q_OBJECT

public:
    QNewNotebookDialog(QWidget *parent = 0);
    ~QNewNotebookDialog();

public slots:
	void createNewNotebook();

signals:
	void reloadNotebookList();

private:
    Ui::QNewNotebookDialogClass ui;
};

#endif // QNEWNOTEBOOKDIALOG_H
