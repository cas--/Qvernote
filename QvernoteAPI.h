/*
 * QvernoteAPI.h
 *
 *  Created on: May 12, 2010
 *      Author: alexr
 */

#ifndef QVERNOTEAPI_H_
#define QVERNOTEAPI_H_

#include <QObject>
#include <QMutexLocker>
#include <transport/THttpClient.h>
#include <UserStore.h>
#include <UserStore_constants.h>
#include <NoteStore.h>
#include <boost/shared_ptr.hpp>
#include "QvernoteStorage.h"
#include "QvernoteSettings.h"
#include "oauthtokenizer.h"


#include <inttypes.h>
#include <iostream>
#include <netinet/in.h>
#include <stdint.h>

#include <protocol/TBinaryProtocol.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>
#include <transport/TSSLServerSocket.h>
#include <transport/TSSLSocket.h>
#include <server/TSimpleServer.h>

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace evernote;
using namespace evernote::edam;
using namespace evernote::limits;
using namespace boost;

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
	bool 	loadTrashNoteList(vector<Note>& noteList);
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
	bool sortTags(bool order);

	bool checkAuthenticateToken() { return getAuthenticationToken().length() > 0; }
	void revokeAuthenticationToken();
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
private:
	static QvernoteAPI* self;
	int m_nLastUpdateCount;
	qint64 m_nLastSyncTime;
	vector<Notebook>	m_NotebookList;
	vector<Notebook>::iterator m_NotebookIterator;
	map<string, vector<Tag> > m_TagsPerNotebook;
	vector<Tag>::iterator	m_TagsPerNotebookIterator;
	vector<Tag>	m_TagList;
	vector<Tag>::iterator m_TagIterator;

	shared_ptr<UserStoreClient> m_UserStoreClient;
	shared_ptr<NoteStoreClient> m_NoteStoreClient;
	QvernoteStorage* m_LocalStoreClient;

	shared_ptr<AuthenticationResult> m_AuthenticationResult;
	NoteList	m_NoteList;
	NoteCollectionCounts m_NoteCounts;
	vector<Note>::iterator m_NoteIterator;
	string	m_sLastError;
	int		m_nLastError;

	shared_ptr<THttpClient> userStoreHttpClient;
	shared_ptr<TTransport> noteStoreHttpClient;
	QMutex	dbOpMutex;
	QMutex  syncThreadMutex;
	AuthenticationThread* authThread;
	SynchronizationThread* syncThread;
	bool m_bIsOnline;
	User user;
};

#endif /* QVERNOTEAPI_H_ */
