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
#ifndef QMAEMOMAPWIDGET_H_
#define QMAEMOMAPWIDGET_H_

#ifdef false

#include <qgeomappingmanager.h>
#include <qgeomapwidget.h>
#include <qgeoserviceprovider.h>
#include <QGraphicsView>
#include <QPixmap>
#include <QNetworkProxy>
#include <QNetworkProxyFactory>
#include <QMessageBox>

#include "QvernoteAPI.h"

QTM_USE_NAMESPACE

#define MARKER_HEIGHT 36
#define MARKER_WIDTH 25
#define MARKER_PIN_LEN 10

class QMaemoMapViewDialog : public QDialog
{
	Q_OBJECT

public:
    QMaemoMapViewDialog(Note& note) {
    	QNetworkProxyFactory::setUseSystemConfiguration(true);

    	setProvider("nokia");

		qgv = new QGraphicsView(this);
		qgv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		qgv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		qgv->setVisible(true);
		qgv->setGeometry(QRect(0, 0, width(), height()));
		qgv->setInteractive(true);

		QGraphicsScene* scene = new QGraphicsScene(0, 0, width(), height());
		qgv->setScene(scene);

		createMarkerIcon();

		m_mapWidget = new QGeoMapWidget(m_mapManager);
		qgv->scene()->addItem(m_mapWidget);
		m_mapWidget->setGeometry(0, 0, width(), height());
		//m_mapWidget->setZoomLevel(8);
		m_mapWidget->setCenter(QGeoCoordinate(note.attributes.latitude, note.attributes.longitude));

		setWindowTitle(tr("Note location"));

		QTimer::singleShot(0, this, SLOT(delayedInit()));
    }

    ~QMaemoMapViewDialog() {
    	delete m_serviceProvider;
    }


    void setProvider(QString providerId)
    {
		if (m_serviceProvider)
			delete m_serviceProvider ;

		m_serviceProvider = new QGeoServiceProvider(providerId);
		if (m_serviceProvider->error() != QGeoServiceProvider::NoError) {
			QMessageBox::information(this, tr("MapViewer Example"), tr(
										 "Unable to find the %1 geoservices plugin.").arg(providerId));
			qApp->quit();
			return;
		}

		m_mapManager = m_serviceProvider->mappingManager();
		m_routingManager = m_serviceProvider->routingManager();
    }

    void createMarkerIcon()
    {
        m_markerIcon = QPixmap(MARKER_WIDTH, MARKER_HEIGHT);
        m_markerIcon.fill(Qt::transparent);
        QPainter painter(&m_markerIcon);

        QPointF p1(MARKER_WIDTH / 2, MARKER_HEIGHT - 1);
        QPointF p2(MARKER_WIDTH / 2, MARKER_HEIGHT - 1 - MARKER_PIN_LEN);
        QPen pen(Qt::black);
        pen.setWidth(2);
        painter.setPen(pen);
        painter.drawLine(p1, p2);
        QRectF ellipse(0, 0, MARKER_WIDTH - 1, MARKER_WIDTH - 1);
        pen.setWidth(1);
        painter.setPen(pen);
        QColor color(Qt::green);
        color.setAlpha(127);
        QBrush brush(color);
        painter.setBrush(brush);
        painter.drawEllipse(ellipse);
    }

public slots:
    void delayedInit() {
		// TODO: remove this dirty, dirty hack
		m_mapWidget->setZoomLevel(m_mapWidget->zoomLevel());
		m_mapWidget->update();
	}

private:
    QGeoServiceProvider *m_serviceProvider;
    QGeoMappingManager *m_mapManager;
    QGeoMapWidget *m_mapWidget;
    QPixmap m_markerIcon;
    QGraphicsView* qgv;
};

#endif

#endif /* QMAEMOMAPWIDGET_H_ */
