#ifndef QEMAILNOTEDIALOG_H
#define QEMAILNOTEDIALOG_H

#include <QtGui/QDialog>
#include "ui_QEmailNoteDialog.h"
#include "QvernoteAPI.h"

#ifdef Q_WS_MAEMO_5
#include "QMaemoContactEmailListWidget.h"
#endif

class QEmailNoteDialog : public QDialog
{
    Q_OBJECT

public:
    QEmailNoteDialog(Note& note, QvernoteAPI* hEvernote, QWidget *parent = 0);
    ~QEmailNoteDialog();

public slots:
	void sendEmail();
	void showContactList();

private:
    Ui::QEmailNoteDialogClass ui;
    Note m_Note;
    QvernoteAPI* m_hEvernote;
};

#endif // QEMAILNOTEDIALOG_H
