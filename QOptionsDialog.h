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
#ifndef QOPTIONSDIALOG_H
#define QOPTIONSDIALOG_H

#include <QtGui/QDialog>
#include "ui_QOptionsDialog.h"
#include <QMaemo5ListPickSelector>
#include "QvernoteSettings.h"
#include <QFutureWatcher>

class QOptionsDialog : public QDialog
{
	Q_OBJECT

public:
	QOptionsDialog(QWidget *parent = 0);
	~QOptionsDialog();

	void saveSettings();
	void dropDB();

public slots:
	void onSaveSettingsClick();
	void onDropDBClick();
	void onRevokeAuthClick();
	void onRequestAuthClick();
	void deleteThumbnails();
	void selectBlacklistedNotebooks();
	void onBlacklistedNotebookSelected(bool, QString, QString);

	void onDisplayError(QString, QString);
	void onDisplayInfo(QString);
	void onReloadNotebookList();
	void onInitView();
	void onClearNotebookList();

signals:
	void triggerDisplayError(QString, QString);
	void triggerDisplayInfo(QString);
	void triggerReloadNotebookList();
	void triggerInitView();
	void triggerClearNotebookList();

private:
	void configureSslProtocol(bool);
	void configureOnlineMode(bool);
	void configureOrientation(QvernoteSettings::DisplayOrientation);

private:
	Ui::QOptionsDialogClass ui;
	QMaemo5ListPickSelector *onlineSelector;
	QMaemo5ListPickSelector *displayOrientationSelector;
	QMaemo5ListPickSelector *protocolSelector;
	QMaemo5ListPickSelector *notesViewSelector;
	QMaemo5ListPickSelector *maxNoteCountSelector;
	QMaemo5ListPickSelector	*mapsProviderSelector;
	QMaemo5ListPickSelector *syncUpdateRateSelector;

	QPushButton pbRevokeAuth;
	QPushButton pbRequestAuth;
	QPushButton pbBlacklistedNotebooks;
	QPushButton	pbDeleteThumbs;
	QPushButton	pbDropDB;
	QFutureWatcher<void> watcher;
	QFuture<void> future;
	QStringList	m_slBlacklistedNotebooks;
};

#endif // QOPTIONSDIALOG_H
