#ifndef QDELETENOTEBOOKVIEW_H
#define QDELETENOTEBOOKVIEW_H

#include <QtGui/QMainWindow>
#include "ui_QDeleteNotebookView.h"
#include "QvernoteWindow.h"
#include "QvernoteAPI.h"



class QDeleteNotebookView : public QvernoteWindow
{
    Q_OBJECT

public:
    QDeleteNotebookView(QWidget *parent = 0);
    ~QDeleteNotebookView();

    void loadNotebooksIntoView();
    void showEvent(QShowEvent *) {
    	loadNotebooksIntoView();
    }

public slots:
	void setItemMark(QListWidgetItem *item);
   	void deleteSelectedNotebooks();
    void addSelectedCount() { m_lSelectedCount.setText(QString(trUtf8("%1 notebooks")).arg(++m_iSelectedCount)); }
    void subSelectedCount() { m_lSelectedCount.setText(QString(trUtf8("%1 notebooks")).arg(--m_iSelectedCount)); }

private:
    Ui::QDeleteNotebookViewClass ui;
    QvernoteAPI* m_hEvernote;
    QLabel	m_lSelectedCount;
    QPushButton m_pbDeleteSelected;
    int		m_iSelectedCount;
    QWidget* m_parent;
};

#endif // QDELETENOTEBOOKVIEW_H
