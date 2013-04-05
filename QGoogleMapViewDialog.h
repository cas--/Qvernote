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
#ifndef QGOOGLEMAPVIEWDIALOG_H_
#define QGOOGLEMAPVIEWDIALOG_H_

#include <QDialog>
#include <QWebView>
#include "QvernoteAPI.h"

class QGoogleMapViewDialog : public QDialog
{
	Q_OBJECT

public:
	QGoogleMapViewDialog(Note& note, QWidget* parent = 0) : QDialog(parent) {
#ifdef Q_WS_MAEMO_5
		setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif
		mapView = new QWebView(this);
		QObject::connect(mapView, SIGNAL(loadFinished(bool)), this, SLOT(mapLoadFinished()));
		//mapView->setGeometry(0, 0, width(), height());

		mapView->setHtml(getMapHtmlCode(note));
		setWindowTitle(QString::fromUtf8(note.title.c_str()));
		setContentsMargins(0, 0, 0, 0);
		resize(800, 480);
	}

	QSize sizeHint() const {
		return QSize(800, 480);
	}

public slots:
	void mapLoadFinished() {
#ifdef Q_WS_MAEMO_5
		setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
#endif
	}

private:
	QString getMapHtmlCode(Note& note) {
		if(note.attributes.latitude == 0 && note.attributes.longitude == 0)
		{
			return "<html><body><center>" + trUtf8("No GPS position info available") + "</center></body></html>";
		}

		return "<html>" \
		"<head>" \
		"<meta name=\"viewport\" content=\"initial-scale=1.0, user-scalable=no\" />" \
		"<script type=\"text/javascript\" src=\"http://maps.google.com/maps/api/js?sensor=false\">" \
		"</script>" \
		"<script type=\"text/javascript\">" \
		  "function initialize() {" \
		    "var latlng = new google.maps.LatLng(" + QString::number(note.attributes.latitude) + "," + QString::number(note.attributes.longitude) + ");" \
		    "var myOptions = {" \
		      "zoom: 14," \
		      "center: latlng," \
		      "disableDefaultUI: true," \
		      "mapTypeId: google.maps.MapTypeId.ROADMAP" \
		    "};" \
		    "var map = new google.maps.Map(document.getElementById(\"map_canvas\")," \
		        "myOptions);" \
		    "var markerOptions = { "\
		    "map: map, " \
		    "visible: true, "\
			"position: latlng" \
			"};" \
		    "var marker = new google.maps.Marker(markerOptions)" \
		  "}" \
		"</script>" \
		"</head>" \
		"<body onload=\"initialize()\">" \
		  "<div id=\"map_canvas\" style=\"width:800px; height:480px\"></div>" \
		"</body>" \
		"</html>";
	}

private:
	QWebView* mapView;
};
#endif /* QGOOGLEMAPVIEWDIALOG_H_ */
