#ifndef QTRASHWINDOW_H
#define QTRASHWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_QTrashWindow.h"
#include "QvernoteWindow.h"
#include "QvernoteAPI.h"



class QTrashWindow : public QvernoteWindow
{
    Q_OBJECT

public:
    QTrashWindow(QWidget *parent = 0);
    ~QTrashWindow();

    void loadNotesIntoView();
    void showEvent(QShowEvent*) {
    	loadNotesIntoView();
    }

public slots:
	void setItemMark(QListWidgetItem *item);
	void deleteSelectedNotes();
	void restoreSelectedNotes();
	void addSelectedCount() { m_lSelectedCount.setText(QString(trUtf8("%1 notes")).arg(++m_iSelectedCount)); }
	void subSelectedCount() { m_lSelectedCount.setText(QString(trUtf8("%1 notes")).arg(--m_iSelectedCount)); }

private:
	Ui::QTrashWindowClass ui;
	QvernoteAPI* m_hEvernote;
	QLabel	m_lSelectedCount;
	QPushButton m_pbDeleteSelected;
	QPushButton m_pbRestoreSelected;
	int		m_iSelectedCount;
	QWidget* m_parent;
};

#endif // QTRASHWINDOW_H
