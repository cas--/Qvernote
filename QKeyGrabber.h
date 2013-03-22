/*
 * QKeyGrabber.h
 *
 *  Created on: Jun 30, 2010
 *      Author: alexr
 */

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
