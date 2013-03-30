/*
 * QvernoteSettings.cpp
 *
 *  Created on: May 12, 2010
 *      Author: alexr
 */

#include "QvernoteSettings.h"

QvernoteSettings* QvernoteSettings::settings = NULL;

void QvernoteSettings::Load() {
	QSettings settings("qvernote");

	LOAD_VAR(String(), Username, "qvernote/username", "");
	LOAD_VAR(String(), Password, "qvernote/password", "");
	LOAD_VAR(String(), OAuthToken, "qvernote/oauthtoken", "");
	LOAD_VAR(Bool(), UseSsl, "qvernote/ssl", false);
	m_DisplayOrientation = (DisplayOrientation)settings.value("qvernote/display", DO_LANDSCAPE).toInt();
	LOAD_VAR(Int(), lastUpdateCount, "qvernote/lastupdatecount", 0);
	LOAD_VAR(ULongLong(), lastSyncTime, "qvernote/lastsynctime", 0);
	LOAD_VAR(Bool(), WorkOnline, "qvernote/online", true);

	m_NotesView = (NotesView)settings.value("qvernote/notesview", NV_LIST).toInt();
	m_NoteDisplayOrder = (NoteDisplayOrder)settings.value("qvernote/notedisplayorder", NDO_DESC).toInt();
	m_MapsProvider = (MapsProvider)settings.value("qvernote/mapsprovider", MP_GOOGLE).toInt();

	LOAD_VAR(String(), BlacklistedNotebooks, "qvernote/blacklisted_notebooks", "");
	LOAD_VAR(Int(), MaxNoteCount, "qvernote/max_note_count", 0);
}

void QvernoteSettings::Store() {
	QSettings settings("qvernote");

	STORE_VAR("qvernote/username", Username);
	STORE_VAR("qvernote/password", Password);
	STORE_VAR("qvernote/oauthtoken", OAuthToken);
	STORE_VAR("qvernote/ssl", UseSsl);
	STORE_VAR("qvernote/display", DisplayOrientation);
	STORE_VAR("qvernote/lastupdatecount", lastUpdateCount);
	STORE_VAR("qvernote/lastsynctime", lastSyncTime);
	STORE_VAR("qvernote/online", WorkOnline);
	STORE_VAR("qvernote/notesview", NotesView);
	STORE_VAR("qvernote/notedisplayorder", NoteDisplayOrder);
	STORE_VAR("qvernote/mapsprovider", MapsProvider);
	STORE_VAR("qvernote/blacklisted_notebooks", BlacklistedNotebooks);
	STORE_VAR("qvernote/max_note_count", MaxNoteCount);
}


