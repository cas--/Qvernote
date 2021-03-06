/***********************************************************************
Copyright (C) 2010 Alex R <burbulator@gmail.com>
Copyright (C) 2013 Calum Lind <calumlind@gmail.com>

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
#include "QOptionsDialog.h"

#include <QtMaemo5>
#include <QMaemo5ValueButton>
#include <QStandardItemModel>
#include "QvernoteSettings.h"
#include "QvernoteAPI.h"
#include "QvernoteView.h"
#include <QDebug>
#include <QMessageBox>
#include "QNotebookPickList.h"
#include "QvernoteOAuthDialog.h"

static void populateListModel(QStandardItemModel *model, QList<QString>& listValues);

QOptionsDialog::QOptionsDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	QvernoteSettings* settings = QvernoteSettings::Instance();
	QList<QString> listValues;
	QvernoteAPI* h = QvernoteAPI::Instance();

	if(h->checkAuthenticateToken()) {
		pbRevokeAuth.setText(trUtf8("Revoke Authentication"));
		QObject::connect(&pbRevokeAuth, SIGNAL(clicked()), this, SLOT(onRevokeAuthClick()));
		ui.gridLayout_2->addWidget(&pbRevokeAuth, 1, 0, 1, 2);
	} else {
		pbRequestAuth.setText(trUtf8("Authenticate with Evernote"));
		QObject::connect(&pbRequestAuth, SIGNAL(clicked()), this, SLOT(onRequestAuthClick()));
		ui.gridLayout_2->addWidget(&pbRequestAuth, 1, 0, 1, 2);
	}

	QStandardItemModel* onlineSelectionModel = new QStandardItemModel(0, 1);
	QMaemo5ValueButton* vbOnlineSelection = new QMaemo5ValueButton(trUtf8("Client mode"));
	vbOnlineSelection->setValueLayout(QMaemo5ValueButton::ValueBesideText);
	onlineSelector = new QMaemo5ListPickSelector;

	listValues.clear();
	listValues.append(trUtf8("Online"));
	listValues.append(trUtf8("Offline"));

	populateListModel(onlineSelectionModel, listValues);

	onlineSelector->setModel(onlineSelectionModel);
	onlineSelector->setCurrentIndex((settings->getWorkOnline())? 0 : 1);
	vbOnlineSelection->setPickSelector(onlineSelector);

	ui.gridLayout_2->addWidget(vbOnlineSelection, 4, 0, 1, 2);


	QStandardItemModel* displayOrientationModel = new QStandardItemModel(0, 1);
	QMaemo5ValueButton* vbDisplayOrientation = new QMaemo5ValueButton(trUtf8("Display orientation"));
	vbDisplayOrientation->setValueLayout(QMaemo5ValueButton::ValueBesideText);
	displayOrientationSelector = new QMaemo5ListPickSelector;

	listValues.clear();
	listValues.append(trUtf8("Portrait"));
	listValues.append(trUtf8("Landscape"));
	listValues.append(trUtf8("Auto"));

	populateListModel(displayOrientationModel, listValues);

	displayOrientationSelector->setModel(displayOrientationModel);
	displayOrientationSelector->setCurrentIndex(settings->getDisplayOrientation());
	vbDisplayOrientation->setPickSelector(displayOrientationSelector);

	ui.gridLayout_2->addWidget(vbDisplayOrientation, 5, 0, 1, 2);


	QStandardItemModel* protocolSelectionModel = new QStandardItemModel(0, 1);
	QMaemo5ValueButton* vbProtocolSelection = new QMaemo5ValueButton(trUtf8("Protocol"));
	vbProtocolSelection->setValueLayout(QMaemo5ValueButton::ValueBesideText);
	vbProtocolSelection->setEnabled(false);
	protocolSelector = new QMaemo5ListPickSelector;

	listValues.clear();
	listValues.append(trUtf8("HTTP"));
	listValues.append(trUtf8("HTTPS"));

	populateListModel(protocolSelectionModel, listValues);

	protocolSelector->setModel(protocolSelectionModel);
	protocolSelector->setCurrentIndex(true);
	vbProtocolSelection->setPickSelector(protocolSelector);

	ui.gridLayout_2->addWidget(vbProtocolSelection, 6, 0, 1, 2);

	QStandardItemModel* notesViewSelectionModel = new QStandardItemModel(0, 1);
	QMaemo5ValueButton* vbNotesViewSelection = new QMaemo5ValueButton(trUtf8("Notes view"));
	vbNotesViewSelection->setValueLayout(QMaemo5ValueButton::ValueBesideText);
	notesViewSelector = new QMaemo5ListPickSelector;

	listValues.clear();
	listValues.append(trUtf8("List"));
	listValues.append(trUtf8("Grid"));

	populateListModel(notesViewSelectionModel, listValues);

	notesViewSelector->setModel(notesViewSelectionModel);
	notesViewSelector->setCurrentIndex(settings->getNotesView());
	vbNotesViewSelection->setPickSelector(notesViewSelector);

	ui.gridLayout_2->addWidget(vbNotesViewSelection, 7, 0, 1, 2);

	QStandardItemModel* maxNoteCountModel = new QStandardItemModel(0, 1);
	QMaemo5ValueButton* vbMaxNoteCountSelection = new QMaemo5ValueButton(trUtf8("Note limit"));
	vbMaxNoteCountSelection->setValueLayout(QMaemo5ValueButton::ValueBesideText);
	maxNoteCountSelector = new QMaemo5ListPickSelector;

	listValues.clear();
	listValues.append(trUtf8("No limit"));
	listValues.append("50");
	listValues.append("100");
	listValues.append("150");
	listValues.append("200");
	listValues.append("250");
	listValues.append("300");

	populateListModel(maxNoteCountModel, listValues);

	maxNoteCountSelector->setModel(maxNoteCountModel);
	switch(settings->getMaxNoteCount()) {
	case 0:
		maxNoteCountSelector->setCurrentIndex(0);
		break;
	case 50:
		maxNoteCountSelector->setCurrentIndex(1);
		break;
	case 100:
		maxNoteCountSelector->setCurrentIndex(2);
		break;
	case 150:
		maxNoteCountSelector->setCurrentIndex(3);
		break;
	case 200:
		maxNoteCountSelector->setCurrentIndex(4);
		break;
	case 250:
		maxNoteCountSelector->setCurrentIndex(5);
		break;
	case 300:
		maxNoteCountSelector->setCurrentIndex(6);
		break;
	}
	vbMaxNoteCountSelection->setPickSelector(maxNoteCountSelector);

	ui.gridLayout_2->addWidget(vbMaxNoteCountSelection, 8, 0, 1, 2);

	QStandardItemModel* syncUpdateRateModel = new QStandardItemModel(0, 1);
	QMaemo5ValueButton* vbsyncUpdateRateSelection = new QMaemo5ValueButton(trUtf8("Sync interval (minutes)"));
	vbsyncUpdateRateSelection->setValueLayout(QMaemo5ValueButton::ValueBesideText);
	syncUpdateRateSelector = new QMaemo5ListPickSelector;

	listValues.clear();
	listValues.append(trUtf8("5"));
	listValues.append("15");
	listValues.append("30");
	listValues.append("60");
	listValues.append("120");

	populateListModel(syncUpdateRateModel, listValues);

	syncUpdateRateSelector->setModel(syncUpdateRateModel);
	switch(settings->getSyncUpdateRate()) {
	case 5:
		syncUpdateRateSelector->setCurrentIndex(0);
		break;
	case 15:
		syncUpdateRateSelector->setCurrentIndex(1);
		break;
	case 30:
		syncUpdateRateSelector->setCurrentIndex(2);
		break;
	case 60:
		syncUpdateRateSelector->setCurrentIndex(3);
		break;
	case 120:
		syncUpdateRateSelector->setCurrentIndex(4);
		break;
	}
	vbsyncUpdateRateSelection->setPickSelector(syncUpdateRateSelector);

	ui.gridLayout_2->addWidget(vbsyncUpdateRateSelection, 9, 0, 1, 2);

	QStandardItemModel* mapsProviderSelectionModel = new QStandardItemModel(0, 1);
	QMaemo5ValueButton* vbMapsProviderSelection = new QMaemo5ValueButton(trUtf8("Maps provider"));
	vbMapsProviderSelection->setValueLayout(QMaemo5ValueButton::ValueBesideText);
	mapsProviderSelector = new QMaemo5ListPickSelector;

	listValues.clear();
	listValues.append(trUtf8("Ovi maps"));
	listValues.append(trUtf8("Google maps"));

	populateListModel(mapsProviderSelectionModel, listValues);

	mapsProviderSelector->setModel(mapsProviderSelectionModel);
	mapsProviderSelector->setCurrentIndex(settings->getMapsProvider());
	vbMapsProviderSelection->setPickSelector(mapsProviderSelector);

	ui.gridLayout_2->addWidget(vbMapsProviderSelection, 10, 0, 1, 2);

	pbBlacklistedNotebooks.setText(trUtf8("Blacklisted notebooks"));
	m_slBlacklistedNotebooks = settings->getBlacklistedNotebooks().split(',');
	QObject::connect(&pbBlacklistedNotebooks, SIGNAL(clicked()), this, SLOT(selectBlacklistedNotebooks()));
	ui.gridLayout_2->addWidget(&pbBlacklistedNotebooks, 11, 0, 1, 2);

	pbDeleteThumbs.setText(trUtf8("Clear thumbnails"));
	QObject::connect(&pbDeleteThumbs, SIGNAL(clicked()), this, SLOT(deleteThumbnails()));
	ui.gridLayout_2->addWidget(&pbDeleteThumbs, 12, 0, 1, 2);
	pbDropDB.setText(trUtf8("Delete offline database"));
	QObject::connect(&pbDropDB, SIGNAL(clicked()), this, SLOT(onDropDBClick()));
	ui.gridLayout_2->addWidget(&pbDropDB, 13, 0, 1, 2);

	QObject::connect(ui.pbSave, SIGNAL(clicked()), SLOT(onSaveSettingsClick()));
	QObject::connect(this, SIGNAL(triggerDisplayError(QString, QString)), SLOT(onDisplayError(QString, QString)));
	QObject::connect(this, SIGNAL(triggerDisplayInfo(QString)), SLOT(onDisplayInfo(QString)));
	QObject::connect(this, SIGNAL(triggerReloadNotebookList()), SLOT(onReloadNotebookList()));
	QObject::connect(this, SIGNAL(triggerInitView()), SLOT(onInitView()));
	QObject::connect(this, SIGNAL(triggerClearNotebookList()), SLOT(onClearNotebookList()));
}

QOptionsDialog::~QOptionsDialog()
{

}

void QOptionsDialog::onSaveSettingsClick()
{
	setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
	extern void saveSettingsCallback(QOptionsDialog* dlg);
	connect(&watcher, SIGNAL(finished()), this, SLOT(close()));
	future = QtConcurrent::run(saveSettingsCallback, this);
	watcher.setFuture(future);
}

void QOptionsDialog::onRevokeAuthClick()
{
	if(QMessageBox::question(this, trUtf8("Delete Qvernote Authentication with Evernote"), trUtf8("Are you sure?"),
			QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
	{
		QvernoteSettings* settings = QvernoteSettings::Instance();
		QvernoteAPI* h = QvernoteAPI::Instance();

		settings->setOAuthToken("");
		if(h->isOnline())
			//currently missing from evernote 1.23 api
			//h->revokeAuthenticationToken();
			h->setOnline(false);
			settings->setWorkOnline(false);
	}
}

void QOptionsDialog::onRequestAuthClick()
{
	QvernoteAPI* h = QvernoteAPI::Instance();
	QvernoteSettings* settings = QvernoteSettings::Instance();
	QvernoteOAuthDialog oauthDialog;
	if (oauthDialog.exec() == QDialog::Rejected) {
		if (oauthDialog.response == "") {
			qDebug() << "Auth cancelled by user going offline";
			h->setOnline(false);
			settings->setWorkOnline(false);
		} else {
			// the dialog is sending rejected signal even when successful
			qDebug() << "auth token received";
			settings->setOAuthToken(oauthDialog.response);
		}
	} else {
		if (oauthDialog.error or oauthDialog.response == "") {
			qDebug() << "Auth Error going offline: " << oauthDialog.errorMessage;
			h->setOnline(false);
			settings->setWorkOnline(false);
		} else	{
			qDebug() << "auth token received";
			settings->setOAuthToken(oauthDialog.response);
		}
	}
}

void QOptionsDialog::onDropDBClick()
{
	extern void dropDBCallback(QOptionsDialog* dlg);
	if(QMessageBox::question(this, trUtf8("Delete offline database"), trUtf8("Your online store will be intact. Are you sure?"),
					QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
	{
		setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
		QFuture<void> future = QtConcurrent::run(dropDBCallback, this);
		future.waitForFinished();
		setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
	}
}

void QOptionsDialog::onDisplayError(QString errorTitle, QString errorString)
{
	dynamic_cast<QvernoteView*>(parentWidget())->displayError(errorTitle, errorString);
}

void QOptionsDialog::onDisplayInfo(QString infoString)
{
	dynamic_cast<QvernoteView*>(parentWidget())->displayInformationBox(infoString);
}

void QOptionsDialog::onReloadNotebookList()
{
	dynamic_cast<QvernoteView*>(parentWidget())->reloadNotebookList();
}

void QOptionsDialog::onInitView()
{
	dynamic_cast<QvernoteView*>(parentWidget())->initView();
}

void QOptionsDialog::onClearNotebookList()
{
	dynamic_cast<QvernoteView*>(parentWidget())->clearNotebookList();
}

void QOptionsDialog::saveSettings()
{
	QvernoteSettings* settings = QvernoteSettings::Instance();
	//setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);

	if(!onlineSelector->currentIndex() != settings->getWorkOnline())
	{
		settings->setWorkOnline(!onlineSelector->currentIndex());
		configureOnlineMode(settings->getWorkOnline());
	}


	if(displayOrientationSelector->currentIndex() != (int)settings->getDisplayOrientation())
	{
		settings->setDisplayOrientation((QvernoteSettings::DisplayOrientation)displayOrientationSelector->currentIndex());
		configureOrientation(settings->getDisplayOrientation());
	}


	if(protocolSelector->currentIndex() != settings->getUseSsl())
	{
		settings->setUseSsl(protocolSelector->currentIndex());
		configureSslProtocol(settings->getUseSsl());
	}

	settings->setNotesView((QvernoteSettings::NotesView)notesViewSelector->currentIndex());
	settings->setMaxNoteCount(maxNoteCountSelector->currentValueText().toInt());
	settings->setSyncUpdateRate(syncUpdateRateSelector->currentValueText().toInt());
	settings->setMapsProvider((QvernoteSettings::MapsProvider)mapsProviderSelector->currentIndex());

	if(settings->getBlacklistedNotebooks() != m_slBlacklistedNotebooks.join(","))
	{
		settings->setBlacklistedNotebooks(m_slBlacklistedNotebooks.join(","));
		emit triggerReloadNotebookList();
	}
	//close();
}


void QOptionsDialog::configureSslProtocol(bool checked) {
	QvernoteAPI* h = QvernoteAPI::Instance();
	if(h->reInitNoteStore() == false) {
		emit triggerDisplayError(trUtf8("Failed to configure SSL"), QString::fromStdString(h->getLastErrorString()));
	} else {
		emit triggerReloadNotebookList();
	}
}


void QOptionsDialog::configureOnlineMode(bool checked) {
	QvernoteAPI* h = QvernoteAPI::Instance();
	if(h->setOnline(checked) == false)
	{
		emit triggerDisplayError(QString(trUtf8("Failed to go %1").arg((checked)? trUtf8("online") : trUtf8("offline"))), QString::fromStdString(h->getLastErrorString()));
		//QvernoteSettings* settings = QvernoteSettings::Instance();
		//settings->setWorkOnline(!checked);
	} else {
		emit triggerDisplayInfo(QString(trUtf8("Working %1").arg((checked)? trUtf8("online") : trUtf8("offline"))));
		if(h->isOnline())
			emit triggerInitView();
	}
}

void QOptionsDialog::configureOrientation(QvernoteSettings::DisplayOrientation o) {
	dynamic_cast<QvernoteView*>(parentWidget())->setDisplayOrientation(o);
}

void QOptionsDialog::selectBlacklistedNotebooks()
{
	QNotebookPickList dlg("", this);
	dlg.setMultipleSelection(true);
	QObject::connect(&dlg, SIGNAL(notebookSelected(bool, QString, QString)), this, SLOT(onBlacklistedNotebookSelected(bool, QString, QString)));
	m_slBlacklistedNotebooks = QvernoteSettings::Instance()->getBlacklistedNotebooks().split(',');
	dlg.loadNotebooks(m_slBlacklistedNotebooks);
	dlg.exec();
}

void QOptionsDialog::onBlacklistedNotebookSelected(bool checked, QString notebookName, QString notebookGuid)
{
	if(checked)
		m_slBlacklistedNotebooks.append(notebookGuid);
	else
		m_slBlacklistedNotebooks.removeAll(notebookGuid);
}

void QOptionsDialog::deleteThumbnails()
{
	if(QMessageBox::question(this, trUtf8("Delete thumbnails"), trUtf8("Are you sure?"),
			QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
	{
		QvernoteStorage::Instance()->clearThumbnails();
	}
}

void QOptionsDialog::dropDB()
{
	QvernoteStorage::Instance()->clearDatabase();
	QvernoteSettings::Instance()->setLastUpdateCount(0);
	emit triggerClearNotebookList();
}

void populateListModel(QStandardItemModel *model, QList<QString>& listValues)
 {
     for (int i = 0; i < listValues.size(); ++i) {
         QStandardItem *item = new QStandardItem(listValues[i]);
         item->setTextAlignment(Qt::AlignCenter); // the Maemo 5 design spec recommends this.
         item->setEditable(false); // prevent editing of the item
         model->appendRow(item);
     }
 }

void saveSettingsCallback(QOptionsDialog* dlg)
{
	dlg->saveSettings();
}

void dropDBCallback(QOptionsDialog* dlg)
{
	dlg->dropDB();
}
