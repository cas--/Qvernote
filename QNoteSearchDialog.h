#ifndef QNOTESEARCHDIALOG_H
#define QNOTESEARCHDIALOG_H

#include <QtGui/QDialog>
#include "ui_QNoteSearchDialog.h"
#include "QvernoteAPI.h"

class QNoteSearchDialog : public QDialog
{
    Q_OBJECT

public:
    QNoteSearchDialog(QWidget *parent = 0);
    ~QNoteSearchDialog();

public slots:
	void searchNote();
	void openTagSelectionDialog();
	void openNotebookSelectionDialog();
	void setTag(bool state, Tag tag);

signals:
	void searchSubmit(QString searchString);

private:
    Ui::QNoteSearchDialogClass ui;
    QWidget*	m_parent;
    QList<QStringList>	selectedTagsList;
    QStringList			selectedNotebook;
};

#endif // QNOTESEARCHDIALOG_H
