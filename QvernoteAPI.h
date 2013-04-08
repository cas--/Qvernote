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

#ifndef QVERNOTEAPI_H_
#define QVERNOTEAPI_H_

#include <UserStore.h>
#include <NoteStore.h>

#include <boost/shared_ptr.hpp>

#include "QvernoteStorage.h"
#include "QvernoteSettings.h"
#include "oauthtokenizer.h"

#define EDAM_CONSUMER_KEY "casnote"
#define EDAM_CONSUMER_SECRET "9515ef6237756747"
#define EVERNOTE_HOST	"sandbox.evernote.com"
#define EDAM_USER_STORE_PATH "/edam/user"
#define EDAM_NOTE_STORE_PATH "/edam/note"
#define EDAM_CLIENT_NAME "Qvernote QT/4.6;Linux;Nokia N900"
#define SYNC_UPDATE_RATE	5*60000 // 2 minutes

class QvernoteAPI;

class AuthenticationThread : public QThread {

	Q_OBJECT

public:
	AuthenticationThread(QvernoteAPI* hEvernote) : m_hEvernote(hEvernote), m_fTerminate(false) {}

	void setInitialSleep(qint64 initialSleep) {
		m_initialSleep = initialSleep;
	}

	void setTerminate(bool flag) { m_fTerminate = flag; }

	void run();
private:
	QvernoteAPI* m_hEvernote;
	qint64 m_initialSleep;
	bool m_fTerminate;
};

class SynchronizationThread : public QThread {

	Q_OBJECT

public:
	SynchronizationThread(QvernoteAPI* hEvernote) : m_hEvernote(hEvernote), m_fTerminate(false) {}

	void setTerminate(bool flag) { m_fTerminate = flag; }
	void run();
private:
	QvernoteAPI* m_hEvernote;
	bool m_fTerminate;
};

class QvernoteAPI : public QObject {

	Q_OBJECT

public:
	static QvernoteAPI* Instance() {
		if(self == NULL)
			self = new QvernoteAPI();

		return self;
	}

	bool initUserStore();
	void reInitUserStore();
	bool initNoteStore();
	bool reInitNoteStore();

	void initLocalStore();

	bool	loadNotes(int maxNotes, const Notebook& notebook);
	bool	getNote(Note& note);
	bool	getNoteContent(Note& note);
	bool	getFirstNote(Note& note);
	bool	getNextNote(Note& note);
	bool	createNewNote(Note& newNote);
	bool 	updateExistingNote(Note& existingNote);
	bool	deleteNote(Guid noteGuid);
	//bool 	searchNotes(const string& searchString, int maxResults);
	bool 	searchNotes(const NoteFilter& filter, int maxResults);

	bool	searchNoteList(vector<Note>::iterator& pos, Guid noteGuid);
	bool	expungeNote(const Guid noteGuid);
	bool 	copyNote(Guid noteGuid, Guid toNotebookGuid);
	bool 	moveNote(Guid noteGuid, Guid toNotebookGuid);
	bool	undeleteNote(Note& note);
	void	loadTrashNoteList(vector<Note>& noteList);
	bool	loadResource(Resource& loadedResource, Guid resourceGuid);
	bool	loadResourceByHash(Resource& loadedResource, Guid noteGuid, QString hexContentHash);

	bool loadNotebookList();
	bool getDefaultNotebook(Notebook& defaultNotebook);
	bool getNotebookByName(Notebook& notebook, string notebookName);
	bool getNotebookByGuid(Notebook& notebook, Guid notebookGuid);
	bool getFirstNotebook(Notebook& notebook);
	bool getNextNotebook(Notebook& notebook);
	bool createNewNotebook(const string& notebookName, bool isDefault, bool isPublished);
	bool deleteNotebook(Notebook& notebook);
	int	 getNotebookCount();
	int  getNotebookContentsCount(Notebook& notebook);
	int  getTrashCount();
	bool getFirstNotebookTag(const string& notebookName, Tag& tag);
	bool getNextNotebookTag(const string& notebookName, Tag& tag);

	bool addNewTag(const string& newTagName, Tag& result);
	int  updateTag(Tag& updatedTag);
	bool loadTagList();
	bool getFirstTag(Tag& tag);
	bool getNextTag(Tag& tag);
	bool addExistingTag(const Tag& tag, Note& note);
	bool removeExistingTag(const Tag& tag, Note& note);
	void sortTags(bool order);

	bool checkAuthenticateToken() { return getAuthenticationToken().length() > 0; }
	//currently missing from evernote 1.23 api
	//void revokeAuthenticationToken();
	string getAuthenticationToken() {
		OAuthTokenizer tokenizer;
		QString data = QvernoteSettings::Instance()->getOAuthToken();
		tokenizer.tokenize(data);
		return tokenizer.oauth_token.toStdString();
	}
	string getShardId() { return user.shardId; }

	bool getSyncState(SyncState& syncState);
	bool getSyncChunk(SyncChunk& syncChunk, int afterUSN, int maxEntries, bool isFullSync);

	string	getLastErrorString() { return m_sLastError; }
	int		getLastErrorCode() { return m_nLastError; }

	bool isOnline() { return m_bIsOnline; }
	bool setOnline(bool isOnline);

	bool emailNote(Note& note, string emailAddress);
	NoteList& getNoteList() { return m_NoteList; }

public slots:
	qint64 refreshAuthentication();
	bool synchronizeLocalStorage();

signals:
	void progressUpdated(QString);
	void noteContentUpdated(Guid);

private:
	QvernoteAPI();
	virtual ~QvernoteAPI();

	void	setError(string errorString, int errorCode) {
		m_sLastError = errorString;
		m_nLastError = errorCode;
	}

	void clearError() { m_sLastError.clear(); m_nLastError = 0; }

	bool synchronizeFull();
	bool synchronizeChanges();
	bool sendChanges();
	bool sendDirtyTags();
	bool sendDirtySearches();
	bool sendDirtyNotebooks();
	bool sendDirtyNotes();

	bool updateNotes(SyncChunk& syncChunk);
	bool updateNotebooks(SyncChunk& syncChunk);
	bool updateTags(SyncChunk& syncChunk);
	bool updateSearches(SyncChunk& syncChunk);
	bool updateResources(SyncChunk& syncChunk);

	bool updateDB(SyncChunk& syncChunk);

	static QvernoteAPI* self;
	int m_nLastUpdateCount;
	qint64 m_nLastSyncTime;
	vector<Notebook>	m_NotebookList;
	vector<Notebook>::iterator m_NotebookIterator;
	map<string, vector<Tag> > m_TagsPerNotebook;
	vector<Tag>::iterator	m_TagsPerNotebookIterator;
	vector<Tag>	m_TagList;
	vector<Tag>::iterator m_TagIterator;

	boost::shared_ptr<evernote::edam::UserStoreClient> m_UserStoreClient;
	boost::shared_ptr<evernote::edam::NoteStoreClient> m_NoteStoreClient;
	QvernoteStorage* m_LocalStoreClient;

	boost::shared_ptr<AuthenticationResult> m_AuthenticationResult;
	NoteList	m_NoteList;
	NoteCollectionCounts m_NoteCounts;
	vector<Note>::iterator m_NoteIterator;
	string	m_sLastError;
	int		m_nLastError;

	boost::shared_ptr<apache::thrift::transport::TTransport> userStoreHttpClient;
	boost::shared_ptr<apache::thrift::transport::TTransport> noteStoreHttpClient;
	QMutex	dbOpMutex;
	QMutex  syncThreadMutex;
	AuthenticationThread* authThread;
	SynchronizationThread* syncThread;
	bool m_bIsOnline;
	User user;
};

#endif /* QVERNOTEAPI_H_ */
