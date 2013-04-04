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
