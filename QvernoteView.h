#ifndef QVERNOTE_H
#define QVERNOTE_H

#include <QtGui/QMainWindow>
#include "ui_QvernoteView.h"
#include "QvernoteWindow.h"
#include "QvernoteAPI.h"
#include "QNotesView.h"
#include "QNotebookListItemWidget.h"
#include "QSyncProgressDialog.h"
#include "QSyncProgressWidget.h"
#include "QDeleteNotebookView.h"
#include "QTrashWindow.h"
#include "QKeyGrabber.h"

class QvernoteView : public QvernoteWindow
{
    Q_OBJECT

public:
    QvernoteView(QWidget *parent = 0);
    ~QvernoteView();

    void keyPressEvent(QKeyEvent* event);

public slots:
	void initView();
	void loadNotebook(QListWidgetItem* p_notebookListItem);
    void createNewNotebook();
    void synchronizeNoteStore();
    void reloadNotebookList();
    void loadNotebookList(QString notebookFilter = "");
    void deleteSelectedNotebook(QNotebookListItemWidget*);
    void openSearchDialog();
    void searchNote(const QString& searchString, const std::vector<std::string>& tagGuids, const Guid& notebookGuid);
    void openDeleteNotebookWindow();
    void openAboutDialog();
    void openTrashWindow();
    void openFavoritesWindow();
    void openOptionsDialog();
    void redrawNotebookList(QString searchString);
    void clearNotebookList();
    void createNoteSnapshot(Guid guid);

    void orientationChanged();
    bool backgroundJobCallback() { return backgroundInit(); }
    void jobDoneCallback() { loadNotebookList(); }
    void jobFailedCallback() {
    	displayError(trUtf8("Unable to load notebook list"), QString(m_hEvernote->getLastErrorString().c_str()));
    	loadNotebookList();
    }

private:
	bool backgroundInit();

private:
    Ui::QvernoteClass ui;
    QvernoteAPI* m_hEvernote;
    shared_ptr<QNotesView>			m_pNotesView;
    shared_ptr<QDeleteNotebookView> m_pDeleteNotebookView;
    shared_ptr<QTrashWindow> 		m_pTrashWindow;
    shared_ptr<QKeyGrabber> 		m_pKeyGrabber;
    //QSyncProgressDialog syncDlg;
    QSyncProgressWidget syncWidget;
    bool	isSearchShown;
    bool 	isSynchronizing;
};

#endif // QvernoteView_H
