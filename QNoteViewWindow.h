/***********************************************************************
Copyright (C) 2010 Alex R <burbulator@gmail.com>

This file is part of Qvernote.

Qvernote is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Qvernote is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Qvernote.  If not, see <http://www.gnu.org/licenses/>.

***********************************************************************/
#ifndef QNOTEVIEWWINDOW_H
#define QNOTEVIEWWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_QNoteViewWindow.h"
#include "QvernoteWindow.h"
#include "QvernoteAPI.h"
#include "QEnmlReader.h"
#include "QClickableLabel.h"
#include "QNoteEditWindow.h"

class QNoteViewWindow : public QvernoteWindow
{
    Q_OBJECT

public:
    QNoteViewWindow(const Note& note, QWidget *parent = 0);
    ~QNoteViewWindow();

    void showEvent ( QShowEvent * );
    Note& getNote() { return m_Note; }
    //void mousePressEvent(QMouseEvent* e);
    //void mouseReleaseEvent(QMouseEvent* e);
    //void mouseMoveEvent(QMouseEvent* e);

public slots:
    void startEditor();
    void resizeWebView();
    void openLink(QUrl);

    void zoominView();
    void zoomoutView();
    void emailNote();
    void loadNextNote();
    void loadPrevNote();
    void showNoteMap();

    bool loadNote();
    void displayNote();

    void displayNavigationControls();
    void hideNavigationControls();

//#ifdef Q_WS_MAEMO_5
    void toggleFullScreen();
//#endif

    void onFavoriteClicked();
    void backgroundLoad();
    void startBackgroundTask() { m_pBackgroundJob->start(); }
    void noteLoadFinished();

    /* JS API functions */
    void handleResource(QString);

    bool backgroundJobCallback() { return loadNote(); }
    void jobDoneCallback() { displayNote(); }
    void jobFailedCallback() { displayError(trUtf8("Unable to load note"), QString(m_hEvernote->getLastErrorString().c_str())); }

signals:
    void startEditorSignal(Note&, QEnmlReader*);

private:
	void clearNote();
	void initNavigationControls();
	void updateHeader();

private:
    Ui::QNoteViewWindowClass ui;
    Note m_Note;
    QvernoteAPI* m_hEvernote;
    boost::shared_ptr<QEnmlReader> enmlReader;
    QClickableLabel	*clZoomin, *clZoomout, *clNext, *clPrev, *clFullscreen, *clEdit, *clEmail, *clMap;
    QTimer* navigationControlTimer;
    QPoint	mousePos;
    bool isMousePressed;
    bool isNavControlsVisible;
    QPropertyAnimation* webViewSlide;
    QRect	savedWebViewGeometry;
    bool	slideLeft;
    boost::shared_ptr<QNoteEditWindow> m_pNoteEditWindow;
#ifdef Q_WS_MAEMO_5
    QMaemoTextSelectionSupressor* supressor;
#endif
};

class QWebViewEventFilter : public QObject
{
	Q_OBJECT

public:
	QWebViewEventFilter(QWebView* v, QNoteViewWindow* w)
	: QObject(v), view(v), wnd(w), mousePressed(false) {
		Q_ASSERT(view);
		Q_ASSERT(wnd);

		//view->installEventFilter(this);
	}

protected:
	inline bool eventFilter(QObject *, QEvent *e);

private:
	QWebView *view;
	QNoteViewWindow* wnd;
	bool enabled;
	bool mousePressed;
	QPoint mousePos;
};

bool QWebViewEventFilter::eventFilter(QObject *, QEvent *e)
{
 switch (e->type()) {
 case QEvent::MouseButtonPress:
	 if (static_cast<QMouseEvent *>(e)->button() == Qt::LeftButton)
	 {
		 mousePressed = true;
		 mousePos = static_cast<QMouseEvent *>(e)->pos();
	 }
	 break;
 case QEvent::MouseButtonRelease:
	 if (static_cast<QMouseEvent *>(e)->button() == Qt::LeftButton)
	 {
		 if(mousePressed)
		 {
			 mousePressed = false;
			 wnd->displayNavigationControls();
		 }
	 }
	 break;
 case QEvent::MouseMove:
	 {
		 QRect r(mousePos, static_cast<QMouseEvent *>(e)->pos());
		 if(abs(r.height()) > 20 || abs(r.width()) > 20)
			 mousePressed = false;
	 }
	 break;
 default:
	 break;
 }
 return false;
};

class QResourceHandlerDialog : public QDialog
{
	Q_OBJECT

public:
	QResourceHandlerDialog(Note& note, QString resourceHash, QWidget* parent = 0);

public slots:
	void storeResource();
    void previewResource();

private:
    Note m_Note;
    QString m_resourceHash;
	QGridLayout* layout;
	QPushButton bSave, bPreview;
};
#endif // QNOTEVIEWWINDOW_H
