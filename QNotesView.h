#ifndef QNOTESVIEW_H
#define QNOTESVIEW_H

#include <QtGui/QMainWindow>
#include "QvernoteWindow.h"
#include <QGraphicsWidget>
#include <QGraphicsGridLayout>
#include "ui_QNotesView.h"
#include "QvernoteAPI.h"
#include "QNotesViewGraphicsWidget.h"
#include "QNoteListItemWidget.h"
#include <QThread>
#include "QAbstractNoteView.h"
#include "QNoteViewWindow.h"
#include "QNoteEditWindow.h"
#include "QDeleteNotesView.h"
#include "QKeyGrabber.h"

class QNotesView : public QvernoteWindow
{
    Q_OBJECT

public:

    QNotesView(const Notebook& notebook, QWidget *parent = 0);
    QNotesView(QString windowTitle, QWidget *parent = 0);
    ~QNotesView();

    Notebook getNotebook() { return m_Notebook; }

    void getNextNote(Note& nextNote);
    void getPrevNote(Note& prevNote);
    void clearNote(Note& m_Note);

    void keyPressEvent(QKeyEvent* event);
public slots:
	void reloadNotes();
	void loadNotesIntoView(QString noteFilter = "");
	void openNewNoteWindow();
	void openNoteViewer(Note&);
	void loadDeleteNotesView();
	void notesViewShutdown();
	void redrawNoteList(QString searchString);
	void onNoteLoadProgress();
	void onNoteLoadDone();
	void setNoteDisplayOrderAsc();
	void setNoteDisplayOrderDesc();

	void orientationChanged();

	bool backgroundJobCallback() { return queryNotes(); }
    void jobDoneCallback() { loadNotesIntoView(); }
    void jobFailedCallback() { notesViewShutdown(); }

private:
	bool queryNotes();

private:
    Ui::QNotesViewClass ui;
    QvernoteAPI* m_hEvernote;
    Notebook	m_Notebook;
    bool	isSearchShown;
    bool 	isSynchronizing;
    QAbstractNoteView* m_pNoteListViewWidget;
    shared_ptr<QNoteViewWindow> 	m_pNoteViewWindow;
    shared_ptr<QNoteEditWindow> 	m_pNoteEditWindow;
    shared_ptr<QDeleteNotesView> 	m_pDeleteNotesView;
    shared_ptr<QKeyGrabber>			m_pKeyGrabber;
};

#endif // QNOTESVIEW_H
