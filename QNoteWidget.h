#ifndef QNOTE_H
#define QNOTE_H

#include <QtGui/QWidget>
#include <QtWebKit/QWebView>
#include <QMouseEvent>
#include "ui_QNoteWidget.h"
#include "QvernoteAPI.h"
#include "QEnmlReader.h"
#include <QDebug>
#include <QGraphicsView>
using namespace qvernote::api;

class QNoteSnapshotScene : public QGraphicsScene
{
	Q_OBJECT

public:
	QNoteSnapshotScene(QWidget* parent): isMouseDown(false), m_parent(parent) {};
	~QNoteSnapshotScene() {};

	/*
	void	mousePressEvent ( QGraphicsSceneMouseEvent *  )
	{
		//emit mousePressed();
		//qDebug() << "Mouse pressed";
	}
	*/
	void mouseReleaseEvent( QGraphicsSceneMouseEvent * )
	{
		isMouseDown = false;
		m_parent->update();
		//qDebug() << "Mouse released";
		emit mouseReleased();
	}

	void setMouseDown(bool state) { isMouseDown = state; }
	bool getMouseDown() { return isMouseDown; }

signals:
	void mouseReleased();

private:
	bool isMouseDown;
	QWidget* m_parent;
};

class QNoteWidget : public QWidget
{
    Q_OBJECT

public:
    QNoteWidget(Note& note, QWidget* topWidget, QWidget *parent = 0);
    ~QNoteWidget();

	Note& getNote() { return m_Note; }

	void paintEvent ( QPaintEvent * );
	void mousePressEvent( QMouseEvent *e );
	//void mouseReleaseEvent( QMouseEvent * );

public slots:
    void openNoteEditor();
    void deleteSelectedNote();

private:
    Ui::QNoteClass ui;
    QWebView	m_WebView;
    QPixmap		m_Snapshot;
    Note		m_Note;
    shared_ptr<QEnmlReader> enmlReader;
    QWidget*	m_topWidget;
    shared_ptr<QNoteSnapshotScene> snapshotScene;
    shared_ptr<QGraphicsPixmapItem> scenePixmap;
    bool isMouseDown;
};

#endif // QNOTE_H
