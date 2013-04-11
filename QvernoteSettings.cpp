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
#include "QvernoteSettings.h"

QvernoteSettings* QvernoteSettings::settings = NULL;

void QvernoteSettings::Load() {
	QSettings settings("qvernote");

	LOAD_VAR(String(), OAuthToken, "qvernote/oauthtoken", "");
	LOAD_VAR(Bool(), UseSsl, "qvernote/ssl", false);
	m_DisplayOrientation = (DisplayOrientation)settings.value("qvernote/display", DO_LANDSCAPE).toInt();
	LOAD_VAR(Int(), LastUpdateCount, "qvernote/lastupdatecount", 0);
	LOAD_VAR(ULongLong(), LastSyncTime, "qvernote/lastsynctime", 0);
	LOAD_VAR(Bool(), WorkOnline, "qvernote/online", true);

	m_NotesView = (NotesView)settings.value("qvernote/notesview", NV_LIST).toInt();
	m_NoteDisplayOrder = (NoteDisplayOrder)settings.value("qvernote/notedisplayorder", NDO_DESC).toInt();
	m_MapsProvider = (MapsProvider)settings.value("qvernote/mapsprovider", MP_GOOGLE).toInt();

	LOAD_VAR(String(), BlacklistedNotebooks, "qvernote/blacklisted_notebooks", "");
	LOAD_VAR(Int(), MaxNoteCount, "qvernote/max_note_count", 0);
	LOAD_VAR(Int(), SyncUpdateRate, "qvernote/sync_update_rate", 5)
}

void QvernoteSettings::Store() {
	QSettings settings("qvernote");

	STORE_VAR("qvernote/oauthtoken", OAuthToken);
	STORE_VAR("qvernote/ssl", UseSsl);
	STORE_VAR("qvernote/display", DisplayOrientation);
	STORE_VAR("qvernote/lastupdatecount", LastUpdateCount);
	STORE_VAR("qvernote/lastsynctime", LastSyncTime);
	STORE_VAR("qvernote/online", WorkOnline);
	STORE_VAR("qvernote/notesview", NotesView);
	STORE_VAR("qvernote/notedisplayorder", NoteDisplayOrder);
	STORE_VAR("qvernote/mapsprovider", MapsProvider);
	STORE_VAR("qvernote/blacklisted_notebooks", BlacklistedNotebooks);
	STORE_VAR("qvernote/max_note_count", MaxNoteCount);
	STORE_VAR("qvernote/sync_update_rate", SyncUpdateRate);
}


