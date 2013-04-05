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
#ifndef QPOPPLERSUPPORT_H_
#define QPOPPLERSUPPORT_H_

#include <poppler-qt4.h>
#include <QApplication>
#include <QPainter>
#include <QDebug>

class QPopplerSupport
{
public:
	QPopplerSupport() : document(NULL) { }
	QPopplerSupport(QByteArray& pdfData) {
		document = Poppler::Document::loadFromData(pdfData);

		if(!document || document->isLocked()) {
			qDebug() << "Error loading PDF document";
			delete document;
		}

		documentPages = document->numPages();
	}

	~QPopplerSupport() {
		delete document;
	}

	void setData(QByteArray& pdfData) {
		document = Poppler::Document::loadFromData(pdfData);

		if(!document || document->isLocked()) {
			qDebug() << "Error loading PDF document";
			delete document;
		}

		documentPages = document->numPages();
	}

	bool renderPage(int pageNumber, QPixmap& pixmap) {
		if(!document || pageNumber+1 > documentPages) {
			renderNullPage(pixmap);
			return false;
		}

		Poppler::Page* pdfPage = document->page(pageNumber);
		if(pdfPage == NULL) {
			renderNullPage(pixmap);
			return false;
		}

		pixmap = QPixmap::fromImage(pdfPage->renderToImage());
		return true;
	}

private:
	void renderNullPage(QPixmap& pixmap) {
		QString invalidPdf = QApplication::translate("QPopplerSupport", "Invalid PDF", 0, QCoreApplication::UnicodeUTF8);
		QPainter p(&pixmap);
		int strWidth = p.fontMetrics().width(invalidPdf);

		p.drawPixmap(pixmap.width() / 2 - 24, 10, QPixmap(":/IconAdd/file-pdf.png"));
		p.drawText(pixmap.width() / 2 - strWidth / 2, 80, invalidPdf);
	}

private:
	Poppler::Document* document;
	int		documentPages;
};
#endif /* QPOPPLERSUPPORT_H_ */
