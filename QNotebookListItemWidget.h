#ifndef QNOTEBOOKLISTITEMWIDGET_H
#define QNOTEBOOKLISTITEMWIDGET_H

#include <QtGui/QWidget>
#include "ui_QNotebookListItemWidget.h"
#include "QvernoteAPI.h"
#include <QGraphicsPixmapItem>



class QNotebookListItemWidget : public QWidget
{
    Q_OBJECT

public:
    QNotebookListItemWidget(Notebook& notebook, QvernoteAPI* hEvernote, int noteCount, bool isDeleteMode, QWidget *parent = 0);
    ~QNotebookListItemWidget();

    Notebook& getNotebook() { return m_Notebook; }

    bool	isItemMarked() { return m_bItemMarked; }
    void mark();


signals:
	void notebookMarked();
	void notebookUnmarked();

private:
    Ui::QNotebookListItemWidgetClass ui;
    Notebook m_Notebook;
    bool	m_bDeleteMode;
    bool    m_bItemMarked;
    bool	m_bMouseMoved;
};

#endif // QNOTEBOOKLISTITEMWIDGET_H
