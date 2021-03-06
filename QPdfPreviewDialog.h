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
#ifndef QPDFPREVIEWDIALOG_H_
#define QPDFPREVIEWDIALOG_H_

#include <QtGui/QDialog>
#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QDesktopWidget>
#include "QvernoteAPI.h"
#include "QPopplerSupport.h"

class QPdfPreviewDialog : public QDialog
{
	Q_OBJECT

public:
	QPdfPreviewDialog(Resource res, QWidget* parent = 0) : QDialog(parent), currentPage(0) {
		setWindowTitle(trUtf8("Preview PDF resource ") + QString::fromStdString(res.attributes.fileName));
		layout = new QGridLayout(this);
		layout->setContentsMargins(8, 8, 8, 8);

		bPrev.setIcon(QIcon(":/IconAdd/general_back.png"));
		layout->addWidget(&bPrev, 0, 1);
		bNext.setIcon(QIcon(":/IconAdd/general_forward.png"));
		layout->addWidget(&bNext, 0, 2);
		pageNoLabel.setFixedWidth(600);
		layout->addWidget(&pageNoLabel, 0, 3);

		layout->addWidget(&scrollArea, 1, 1, 1, 3, Qt::AlignCenter);
		scrollArea.setWidgetResizable(true);
		scrollArea.setWidget(&previewLabel);

		mediaData = QByteArray::fromRawData(res.data.body.c_str(), res.data.size);
		poppler.setData(mediaData);
		previewLabel.setAlignment(Qt::AlignHCenter);
		renderPage();
		QObject::connect(&bPrev, SIGNAL(clicked()), this, SLOT(renderPrevPage()));
		QObject::connect(&bNext, SIGNAL(clicked()), this, SLOT(renderNextPage()));
		connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(orientationChanged()));
		resize(QApplication::desktop()->width(), QApplication::desktop()->height());
	}

	QSize sizeHint() const {
		return QSize(800, 480);
	}

private:
	bool renderPage() {
		if(poppler.renderPage(currentPage, pdfPixmap))
		{
			previewLabel.setPixmap(pdfPixmap);
			pageNoLabel.setText(QString(trUtf8("Page %1")).arg(currentPage+1));
			return true;
		}

		return false;
	}

public slots:
	void renderPrevPage() {
		if(currentPage == 0)
			return;

		currentPage--;
		renderPage();
	}

	void renderNextPage() {
		currentPage++;
		if(renderPage() == false)
			currentPage--;
	}

	void orientationChanged() {
		resize(QApplication::desktop()->width(), QApplication::desktop()->height());
	}

private:
	QGridLayout* layout;
	QPushButton bNext;
	QPushButton bPrev;
	QLabel		pageNoLabel;
	QScrollArea scrollArea;
	QLabel		previewLabel;
	QByteArray mediaData;
	QPopplerSupport poppler;
	QPixmap pdfPixmap;
	int		currentPage;
};
#endif /* QPDFPREVIEWDIALOG_H_ */
