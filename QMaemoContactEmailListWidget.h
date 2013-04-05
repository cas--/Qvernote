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
#ifndef QMAEMOCONTACTEMAILLISTWIDGET_H_
#define QMAEMOCONTACTEMAILLISTWIDGET_H_

#include <QtGui/QDialog>
#include <QtCore>
#include <QPointer>
// QtMobility
#include <QContactManager>
#include <QContact>
#include <QContactName>
//#include <qcontactdetailfilter>
//#include <qcontactphonenumber>
#include <QContactEmailAddress>
#include <QContactAvatar>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include "QNonScrollableListWidget.h"

QTM_USE_NAMESPACE

class QEmailContactListItemWidget : public QWidget
{
	Q_OBJECT

public:
	QEmailContactListItemWidget(QContact contact, QWidget* parent = 0) : QWidget(parent), m_contact(contact) {
		QHBoxLayout* l = new QHBoxLayout(this);
		QContactName name = contact.detail(QContactName::DefinitionName);
		QContactEmailAddress addr = contact.detail(QContactEmailAddress::DefinitionName);
		//qDebug() << __FUNCTION__ << name.firstName() + " " + name.lastName();
		l->addWidget(new QLabel(name.firstName() + " " + name.lastName()));
		l->addWidget(new QLabel(addr.emailAddress()));
		emailAddress = addr.emailAddress();
	}

	QString getContactEmail() { return emailAddress; }

private:
	QContact m_contact;
	QString emailAddress;
};

class QEmailContactList : public QDialog
{
	Q_OBJECT

public:
	QEmailContactList(QWidget* parent = 0) : QDialog(parent) {
		m_contactManager = new QContactManager("maemo5");

		mainLayout = new QGridLayout(this);
		layout = new QVBoxLayout(&areaWidget);
		layout->addWidget(&listWidget);
		area.setWidget(&areaWidget);
		area.setWidgetResizable(true);
		mainLayout->addWidget(&area);
		setWindowTitle(trUtf8("Choose contact"));

		QObject::connect(&listWidget, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(selectContactEmail(QListWidgetItem *)));

		setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
		QtConcurrent::run(loadContactListCallback, this);
		resize(800, 480);
	}

	QString getSelectedEmail() { return selectedEmail; }


	void loadContactList() {
		QContactSortOrder sortOrder;
		sortOrder.setDirection(Qt::AscendingOrder);
		sortOrder.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirstName);
		QContactFetchHint fetchHint;
		fetchHint.setDetailDefinitionsHint(QStringList(QContactEmailAddress::DefinitionName));
		fetchHint.setOptimizationHints(QContactFetchHint::NoBinaryBlobs);
		fetchHint.setOptimizationHints(QContactFetchHint::NoRelationships);
		QList<QContact> matchingContacts = m_contactManager->contacts(QList<QContactSortOrder>(sortOrder), fetchHint);

		qRegisterMetaType<QContact>("QContact");
		QObject::connect(this, SIGNAL(nextContactFound(QContact)), SLOT(onNextContactFound(QContact)), Qt::QueuedConnection);

		QListIterator<QContact> i(matchingContacts);
		while(i.hasNext()) {
			emit nextContactFound(i.next());
		}
		setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
	}

	static void loadContactListCallback(QEmailContactList* p) {
		p->loadContactList();
	}

public slots:
	void onNextContactFound(QContact nextContact) {
		QContactEmailAddress addr = nextContact.detail(QContactEmailAddress::DefinitionName);
		if(!addr.emailAddress().isEmpty())
		{
			QEmailContactListItemWidget* iw = new QEmailContactListItemWidget(nextContact);
			QListWidgetItem* i = new QListWidgetItem(&listWidget);
			listWidget.setItemWidget(i, iw);
		}
	}

	void selectContactEmail(QListWidgetItem *item) {
		QEmailContactListItemWidget* w = (QEmailContactListItemWidget*)listWidget.itemWidget(item);
		selectedEmail = w->getContactEmail();
		close();
	}

signals:
	void nextContactFound(QContact);

private:
    QPointer<QContactManager> m_contactManager;
    QGridLayout* mainLayout;
    QVBoxLayout* layout;
    QScrollArea area;
    QWidget	areaWidget;
    QNonScrollableListWidget listWidget;
    QString selectedEmail;
};

#endif /* QMAEMOCONTACTEMAILLISTWIDGET_H_ */
