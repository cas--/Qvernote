#ifndef QNOTELISTITEMWIDGET_H
#define QNOTELISTITEMWIDGET_H

#include <QtGui/QWidget>
#include "ui_QNoteListItemWidget.h"
#include <QWebView>
#include "QvernoteAPI.h"
#include "QEnmlReader.h"
#include <QDebug>
#include "QDeleteNotesView.h"



class QNoteListItemWidget : public QWidget
{
    Q_OBJECT

public:
    QNoteListItemWidget(const Note& note, bool isDeleteMode = false, QWidget *parent = 0);
    ~QNoteListItemWidget();

    Note& getNote() { return m_Note; }
    //QString getHtml() { return enmlReader->getHtml(); }
    bool	isItemMarked() { return m_bItemMarked; }

    void mark();
    /*
    void mouseMoveEvent(QMouseEvent* event)
    {
    	//m_bMouseMoved = true;
    }

    void mousePressEvent ( QMouseEvent *event )
    {
    	//mousePosSave = event->globalPos();
    	m_bMouseMoved = false;
    	QWidget::mousePressEvent(event);
    }

    void mouseReleaseEvent( QMouseEvent *event )
    {
    	if(m_bDeleteMode && !m_bMouseMoved)
    	{
    		//qDebug() << "In delete mode";
    		if(m_bItemMarked == false)
    		{
    			m_bItemMarked = true;
    			ui.lDeleteMark->setPixmap(QPixmap(":/IconAdd/widgets_tickmark_list.png"));
    			emit noteMarked();
    		}
    		else
    		{
    			m_bItemMarked = false;
    			ui.lDeleteMark->clear();
    			emit noteUnmarked();
    		}
    	}
    	else
    	{
    		QWidget::mouseReleaseEvent(event);
    	}
    }
	*/

public slots:
    //void setNoteSnapshot();

signals:
	void noteMarked();
	void noteUnmarked();

protected:
	/*
	inline bool eventFilter(QObject* obj, QEvent* e)
	{
		if(e->type() == QEvent::MouseMove)
		{
			m_bMouseMoved = true;
			return false;
		}

		return false;
	}
	*/

private:
    Ui::QNoteListItemWidgetClass ui;
    //QWebView	m_WebView;
    //QPixmap		m_Snapshot;
    Note		m_Note;
    //shared_ptr<QEnmlReader> enmlReader;
    bool	m_bDeleteMode;
    bool	m_bItemMarked;
    bool	m_bMouseMoved;
    //QPoint	mousePosSave;
    //QGraphicsScene snapshotScene;
    //QGraphicsPixmapItem snapshotItem;
};

#endif // QNOTELISTITEMWIDGET_H
