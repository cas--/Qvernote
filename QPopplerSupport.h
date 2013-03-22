/*
 * QPopplerSupport.h
 *
 *  Created on: Aug 2, 2010
 *      Author: alexr
 */

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
