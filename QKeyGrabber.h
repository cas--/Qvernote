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
#ifndef QKEYGRABBER_H_
#define QKEYGRABBER_H_

#include <QWidget>
#include <QLineEdit>
#include <QGridLayout>
#include <QKeyEvent>
#include "QClickableLabel.h"

class QKeyGrabber : public QWidget
{
	Q_OBJECT

public:
	QKeyGrabber(QString initialKeyText, QWidget* parent = 0) : QWidget(parent) {
		QGridLayout* layout;
		QClickableLabel* closeLabel;
		this->setContentsMargins(0, 0, 0, 0);
		layout = new QGridLayout(this);
		layout->setContentsMargins(0, 0, 0, 0);
		//layout->setSpacing(0);
		closeLabel = new QClickableLabel();
		closeLabel->setBorderEnabled(false);
		closeLabel->setPixmap(QPixmap(":/IconAdd/general_stop.png"));
		//closeLabel->setMargin(0);
		searchInput = new QLineEdit();
		searchInput->setText(initialKeyText);
		layout->addWidget(searchInput, 0, 0);
		layout->addWidget(closeLabel, 0, 1);
		setWindowFlags(Qt::CustomizeWindowHint|Qt::Widget);
		//setAttribute(Qt::WA_DeleteOnClose);

		m_partialSearchString = initialKeyText;
		QObject::connect(closeLabel, SIGNAL(mouseReleased()), SLOT(resetSearch()));
		QObject::connect(searchInput, SIGNAL(textEdited(const QString&)), SLOT(searchLineChanged(const QString&)));
	}

	QSize sizeHint() const {
		return QSize(800, 1);
	}

	void showEvent(QShowEvent*) {
		emit redrawParentContents(m_partialSearchString);
		searchInput->setFocus();
	}

	void setText(const QString& text) {
		searchInput->setText(text);
		m_partialSearchString = text;
	}

public slots:
	void searchLineChanged(const QString& text) {
		m_partialSearchString = text;
		qDebug() << __FUNCTION__ << text;
		emit redrawParentContents(text);
	}

	void resetSearch() {
		if(!m_partialSearchString.isEmpty())
			emit redrawParentContents("");

		close();
	}

signals:
	void redrawParentContents(QString partialSearchString);

private:
	QLineEdit* searchInput;
	QString m_partialSearchString;
};
#endif /* QKEYGRABBER_H_ */
