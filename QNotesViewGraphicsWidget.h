/*
 * QNotesViewGraphicsWIdget.h
 *
 *  Created on: May 19, 2010
 *      Author: alex
 */

#ifndef QNOTESVIEWGRAPHICSWIDGET_H_
#define QNOTESVIEWGRAPHICSWIDGET_H_

#ifdef Q_WS_MAEMO_5

#include <QGraphicsWidget>
#include <QAbstractKineticScroller>
#include <QDebug>

namespace qvernote { namespace view {


class QNotesViewGraphicsWidget : public QGraphicsWidget, public QAbstractKineticScroller
{
	Q_OBJECT

public:
	QNotesViewGraphicsWidget()
		: QGraphicsWidget(), QAbstractKineticScroller(), m_Widget(0)
	{
		setFlag(QGraphicsItem::ItemHasNoContents, true);
		setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
		setFlag(QGraphicsItem::ItemClipsToShape, true);
		setAttribute(Qt::WA_OpaquePaintEvent, true);
	}

	virtual ~QNotesViewGraphicsWidget() {};

	void setWidget(QGraphicsWidget *widget)
	 {
		 if (m_Widget) {
			 m_Widget->setParentItem(0);
			 delete m_Widget;
		 }
		 m_Widget = widget;
		 if (m_Widget) {
			 m_Widget->setParentItem(this);
			 m_Widget->setAttribute(Qt::WA_OpaquePaintEvent, true);
			 //connect(m_Widget, SIGNAL(loadFinished(bool)), this, SLOT(resizeWidgetToFrame()));
			 resizeWidgetToFrame();
		 }
	 }

protected:
	bool sceneEventFilter(QGraphicsItem *i, QEvent *e)
	 {
		qDebug() << "Event" << i << e;
		bool res = false;
		 if (i && (i == m_Widget) && m_Widget->isEnabled()) {
			 qDebug() << "Mouse event";
			 switch (e->type()) {
			 case QEvent::GraphicsSceneMousePress:
			 case QEvent::GraphicsSceneMouseMove:
			 case QEvent::GraphicsSceneMouseRelease:
			 case QEvent::GraphicsSceneMouseDoubleClick: {
				 res = handleMouseEvent((QGraphicsSceneMouseEvent *)e);
				 break;
			 }
			 default:
				 break;
			 }
		 }
		 // prevent text selection and image dragging
		 if (e->type() == QEvent::GraphicsSceneMouseMove)
			 return true;
		 return res ? true : QGraphicsWidget::sceneEventFilter(i, e);
	 }

	 QSize viewportSize() const
	 {
		 return size().toSize();
	 }

	 QPoint maximumScrollPosition() const
	 {
		 QSizeF s = m_Widget ? m_Widget->size() - size() : QSize(0, 0);
		 return QPoint(qMax(0, int(s.width())), qMax(0, int(s.height())));
	 }

	 QPoint scrollPosition() const
	 {
		 return m_Widget ? -m_Widget->pos().toPoint() + m_overShoot : QPoint();
	 }

	 void setScrollPosition(const QPoint &p, const QPoint &overShoot)
	 {
		 m_overShoot = overShoot;
		 if (m_Widget)
			 m_Widget->setPos(-p + m_overShoot);
	 }

private slots:
	 void resizeWidgetToFrame()
	 {
		 QSizeF s = m_Widget->size();
		 qDebug() << s;
		 s = s.expandedTo(size());
		 qDebug() << s;
		 m_Widget->setGeometry(QRectF(m_Widget->geometry().topLeft(), s));
      }
private:
	 QGraphicsWidget* m_Widget;
	 QPoint m_overShoot;
};

}}

#endif
#endif /* QNOTESVIEWGRAPHICSWIDGET_H_ */
