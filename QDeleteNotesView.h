#ifndef QDELETENOTESVIEW_H
#define QDELETENOTESVIEW_H

#include <QtGui/QMainWindow>
#include "ui_QDeleteNotesView.h"
#include "QvernoteWindow.h"
#include "QvernoteAPI.h"



class QDeleteNotesView : public QvernoteWindow
{
    Q_OBJECT

public:
    QDeleteNotesView(const QString& notebookName, QWidget *parent = 0);
    ~QDeleteNotesView();

    void loadNotesIntoView();
    void showEvent(QShowEvent *) {
       	loadNotesIntoView();
    }

public slots:
	void setItemMark(QListWidgetItem *item);
	void deleteSelectedNotes();
	void copySelectedNotes();
	void moveSelectedNotes();
    void addSelectedCount() { m_lSelectedCount.setText(QString(trUtf8("%1 notes")).arg(++m_iSelectedCount)); }
    void subSelectedCount() { m_lSelectedCount.setText(QString(trUtf8("%1 notes")).arg(--m_iSelectedCount)); }

private:
    Ui::QDeleteNotesViewClass ui;
    QvernoteAPI* m_hEvernote;
    QString m_notebookName;
    QLabel	m_lSelectedCount;
    QPushButton m_pbDeleteSelected;
    QPushButton m_pbCopyNotes;
    QPushButton m_pbMoveNotes;
    int		m_iSelectedCount;
    QWidget* m_parent;
};

#endif // QDELETENOTESVIEW_H
