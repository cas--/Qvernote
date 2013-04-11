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
#ifndef QVERNOTESETTINGS_H_
#define QVERNOTESETTINGS_H_

#include <QSettings>

#define CONFIG_VAR(_t_, _n_) 				\
	private:								\
	_t_ m_##_n_;							\
	public:									\
	_t_ get##_n_() { return m_##_n_; }			\
	void set##_n_(_t_ _##_n_) { m_##_n_ = _##_n_; }

#define LOAD_VAR(_T_, _n_, _s_, _d_) \
		m_##_n_ = settings.value(_s_, _d_).to##_T_;

#define STORE_VAR(_s_, _n_) \
		settings.setValue(_s_, m_##_n_);

class QvernoteSettings {
public:

	enum DisplayOrientation {
		DO_PORTRAIT,
		DO_LANDSCAPE,
		DO_AUTO
	};

	enum NotesView {
		NV_LIST,
		NV_GRID
	};

	enum NoteDisplayOrder {
		NDO_ASC,
		NDO_DESC
	};

	enum MapsProvider {
		MP_OVI,
		MP_GOOGLE
	};

	static QvernoteSettings* Instance()
	{
		if(settings == NULL)
			settings = new QvernoteSettings();

		return settings;
	}


	CONFIG_VAR(QString, OAuthToken);
	CONFIG_VAR(bool, UseSsl);
	CONFIG_VAR(DisplayOrientation, DisplayOrientation);
	CONFIG_VAR(int, LastUpdateCount);
	CONFIG_VAR(qint64, LastSyncTime);
	CONFIG_VAR(bool, WorkOnline);
	CONFIG_VAR(NotesView, NotesView);
	CONFIG_VAR(NoteDisplayOrder, NoteDisplayOrder);
	CONFIG_VAR(MapsProvider, MapsProvider);
	CONFIG_VAR(QString, BlacklistedNotebooks);
	CONFIG_VAR(int, MaxNoteCount);
	CONFIG_VAR(int, SyncUpdateRate);

public:
	void 	Store();
	void	Load();

private:
	static QvernoteSettings* settings;

	QvernoteSettings() { Load(); };
	//QvernoteSettings(const QvernoteSettings&);
	QvernoteSettings& operator=(const QvernoteSettings&);
};

//extern QvernoteSettings g_QvernoteSettings;

#endif /* QVERNOTESETTINGS_H_ */
