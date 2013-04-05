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
#ifndef QNOTEEDITWINDOW_H
#define QNOTEEDITWINDOW_H

#include <QtGui/QMainWindow>
#include "QvernoteWindow.h"
#include "ui_QNoteEditWindow.h"
#include "QvernoteAPI.h"
#include "QEnmlWriter.h"
#include "QEnmlReader.h"
#include <QWebElement>
#ifdef Q_WS_MAEMO_5
#include "QMaemoTextSelectionSupressor.h"
#include <QGeoPositionInfoSource>
QTM_USE_NAMESPACE
#endif


class QNoteEditWindow : public QvernoteWindow
{
    Q_OBJECT

public:
    QNoteEditWindow(Note& note, bool bIsNewNote, QWidget *parent = 0);
    ~QNoteEditWindow();

    void hideEvent(QHideEvent*);

signals:
    void noteListUpdated();
    void noteUpdated();

public slots:
    void saveNote();
    void emailNote();
    void toggleBoldStyle();
    void toggleItalicStyle();
    void toggleUnderlineStyle();
    void addBullet();
    void attachFile();
    void resizeWebView();
    void openTagSelectorDialog();
    void setTag(bool, Tag);
    void loadResource(QUrl);
    void insertTodo();
    void startGpsLocationUpdate();
#ifdef Q_WS_MAEMO_5
    void positionUpdated(const QGeoPositionInfo&);
#endif
    //void zoominView();
    //void zoomoutView();

    void setDirtyData() { hasDirtyData = true; }

    bool backgroundJobCallback() { return createNote(); }
    void jobDoneCallback() { noteSaveSuccess(); }
    void jobFailedCallback() { noteSaveFailure(); }

    /* JS API slots */
    void setChecked(int tagId);

private:
    bool createNote();
    void noteSaveFailure();
    void noteSaveSuccess();

private:
    Ui::QNoteEditWindowClass ui;
    Note m_Note;
    QString m_HtmlData;
    bool	isNewNote;
    QvernoteAPI* m_hEvernote;
    shared_ptr<QEnmlReader> enmlReader;
    shared_ptr<QEnmlWriter> enmlWriter;
    int nTodoTagCount;
    QAction* actGps;
    bool	hasDirtyData;
#ifdef Q_WS_MAEMO_5
	QMaemoTextSelectionSupressor* supressor;
	QGeoPositionInfoSource *source;
#endif
};

class QWebViewKeyPressFilter : public QObject
{
	Q_OBJECT

public:
	QWebViewKeyPressFilter(QNoteEditWindow* w)
	: wnd(w) {
		Q_ASSERT(wnd);
	}

protected:
	inline bool eventFilter(QObject *, QEvent *e) {
		 switch (e->type()) {
		  case QEvent::KeyPress:
			 wnd->setDirtyData();
			 break;
		 default:
			 break;
		 }
		 return false;
	}

private:
	QNoteEditWindow* wnd;
};
#endif // QNOTEEDITWINDOW_H
