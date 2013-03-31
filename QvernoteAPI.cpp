/*
 * QvernoteAPI.cpp
 *
 *  Created on: May 12, 2010
 *      Author: alexr
 */


#include "QvernoteAPI.h"
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <transport/THttpClient.h>
#include <transport/TSSLSocket.h>
#include <protocol/TBinaryProtocol.h>
#include <Thrift.h>
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace boost;

QvernoteAPI* QvernoteAPI::self = NULL;

void AuthenticationThread::run() {
	while(true) {
		msleep(m_initialSleep);
		if(m_fTerminate)
			return;
		m_initialSleep = m_hEvernote->refreshAuthentication();
	}
}

void SynchronizationThread::run() {
	while(true) {
		msleep(SYNC_UPDATE_RATE);
		if(m_fTerminate)
			return;
		m_hEvernote->synchronizeLocalStorage();
	}
}

QvernoteAPI::QvernoteAPI() : QObject() {
	if(QvernoteSettings::Instance()->getWorkOnline())
		m_bIsOnline = initUserStore();
	else
		m_bIsOnline = false;

	authThread = new AuthenticationThread(this);
	syncThread = new SynchronizationThread(this);
}

QvernoteAPI::~QvernoteAPI() {
	// TODO Auto-generated destructor stub
}

bool QvernoteAPI::setOnline(bool isOnline) {

	if(isOnline)
	{
		m_bIsOnline = initUserStore();
		if(m_bIsOnline)
		{
			if(checkVersion() == true) {
				if(checkAuthenticateToken())
				{
					reinitNoteStore();
				}
				else
				{
					return false;
				}
			}
			qDebug() << "Starting threads";

			authThread->setTerminate(false);
			syncThread->setTerminate(false);
			if(!authThread->isRunning())
				authThread->start();

			if(!syncThread->isRunning())
				syncThread->start();
		}
		else
		{
			qDebug() << "Failed to go online";
			return false;
		}
	}
	else
	{
		qDebug() << "Terminating threads";

		authThread->setTerminate(true);
		syncThread->setTerminate(true);
	}

	m_bIsOnline = isOnline;
	return true;
}

bool QvernoteAPI::initUserStore() {
	try {
		qDebug() << __FUNCTION__ << "init userstore";
		QvernoteSettings* settings = QvernoteSettings::Instance();
		if(settings->getUseSsl() == true)
		{
			shared_ptr<TSSLSocketFactory> sslSocketFactory(new TSSLSocketFactory());
			QString pgmDir = qApp->applicationDirPath() + "/certs/verisign_certs.pem";
			qDebug() << "userstore dir:" << pgmDir;
			sslSocketFactory->loadTrustedCertificates(pgmDir.toStdString().c_str());
			sslSocketFactory->authenticate(false);

			shared_ptr<TSocket> sslSocket = sslSocketFactory->createSocket(EVERNOTE_HOST, 443);
			shared_ptr<TBufferedTransport> bufferedTransport(new TBufferedTransport(sslSocket));
			userStoreHttpClient = shared_ptr<THttpClient>(new THttpClient(bufferedTransport, EVERNOTE_HOST, EDAM_USER_STORE_PATH));
		}
		else {
			// use http
			userStoreHttpClient= shared_ptr<THttpClient>(new THttpClient(EVERNOTE_HOST, 80, EDAM_USER_STORE_PATH));
		}
		userStoreHttpClient->open();
		shared_ptr<TProtocol> iprot(new TBinaryProtocol(userStoreHttpClient));
		m_UserStoreClient = shared_ptr<UserStoreClient>(new UserStoreClient(iprot));
	} catch(TTransportException& e) {
		qDebug() << e.what();
		setError(e.what(), e.getType());
		return false;
	}

	return true;
}

void QvernoteAPI::reInitUserStore() {
	if(userStoreHttpClient)
		userStoreHttpClient->close();
	initUserStore();
}

bool QvernoteAPI::initNoteStore() {
	m_UserStoreClient->getUser(user, getAuthenticationToken());
	string noteStorePath = string(EDAM_NOTE_STORE_PATH) + string("/") + getShardId();
	QvernoteSettings* settings = QvernoteSettings::Instance();

	try {
		if(settings->getUseSsl() == true)
		{
			qDebug() << "attempting ssl connection";
			shared_ptr<TSSLSocketFactory> sslSocketFactory(new TSSLSocketFactory());
			QString pgmDir = qApp->applicationDirPath() + "/certs/cacert.pem";
			qDebug() << "userstore dir:" << pgmDir;
			sslSocketFactory->loadTrustedCertificates(pgmDir.toStdString().c_str());
			sslSocketFactory->authenticate(true);
			shared_ptr<TSocket> sslSocket = sslSocketFactory->createSocket(EVERNOTE_HOST, 443);
			shared_ptr<TBufferedTransport> bufferedTransport(new TBufferedTransport(sslSocket));
			noteStoreHttpClient = shared_ptr<TTransport>(new THttpClient(bufferedTransport, EVERNOTE_HOST, noteStorePath));
		}
		else {
			noteStoreHttpClient = shared_ptr<TTransport>(new THttpClient(EVERNOTE_HOST, 80, noteStorePath));
		}

		noteStoreHttpClient->open();
		shared_ptr<TProtocol> noteStoreProtocol(new TBinaryProtocol(noteStoreHttpClient));
		m_NoteStoreClient = shared_ptr<NoteStoreClient>(new NoteStoreClient(noteStoreProtocol));
	} catch(TTransportException& e) {
		qDebug() << e.what();
		setError(e.what(), e.getType());
		return false;
	}

	//loadNotebookList();
	return true;
}

bool QvernoteAPI::reinitNoteStore() {
	//m_NotebookList.clear();

	if(!isOnline())
	{
		setError("Unable to connect to the network", 0);
		return false;
	}

	if(noteStoreHttpClient)
		noteStoreHttpClient->close();
	return initNoteStore();
}

void QvernoteAPI::initLocalStore() {
	qDebug() << __FUNCTION__;
	m_LocalStoreClient = QvernoteStorage::Instance();

	if(m_LocalStoreClient->isEnabled() && isOnline())
	{
		qDebug() << __FUNCTION__ << "enabled and online";
		//synchronizeLocalStorage();
		syncThread->start();
	}
}

bool QvernoteAPI::checkVersion() {
	qDebug() << "attempting to check version!";
	UserStoreConstants usc;

	if(!isOnline())
	{
		setError("Unable to connect to the network", 0);
		return false;
	}

	return m_UserStoreClient->checkVersion(EDAM_CLIENT_NAME, usc.EDAM_VERSION_MAJOR, usc.EDAM_VERSION_MINOR);
}

bool QvernoteAPI::Authenticate() {
	if(!isOnline())
	{
		setError("Unable to connect to the network", 0);
		return false;
	}

	//AuthenticationResult authenticationResult;
	//m_AuthenticationResult = shared_ptr<AuthenticationResult>(new AuthenticationResult());

	try {
		reInitUserStore();
	} catch(EDAMUserException& e) {
		qDebug() << e.parameter.c_str();
		setError(e.parameter, e.errorCode);
		return false;
	} catch(TTransportException& te) {
		qDebug() << __FUNCTION__ << te.what();
		setError(te.what(), te.getType());
		return false;
	}

	authThread->setInitialSleep(1000 * (QDateTime::currentDateTime().secsTo(QDateTime::fromTime_t(m_AuthenticationResult->expiration / 1000)) - 60));
	authThread->start();
	return true;
}

qint64 QvernoteAPI::refreshAuthentication()
{
	qDebug() << __FUNCTION__ << "Re-auth started";
	try {
		reInitUserStore();
		m_UserStoreClient->refreshAuthentication(
				*m_AuthenticationResult,
				getAuthenticationToken());
	} catch(EDAMUserException& e) {
		qDebug() << e.parameter.c_str();
		setError(e.parameter, e.errorCode);
		return 60000;
	} catch(EDAMSystemException& se) {
			qDebug() << se.message.c_str();
			setError(se.message, se.errorCode);
			return 60000;
	} catch(TTransportException& te) {
		qDebug() << __FUNCTION__ << te.what();
		setError(te.what(), te.getType());
		return 60000;
	}

	return 1000 * (QDateTime::currentDateTime().secsTo(QDateTime::fromTime_t(m_AuthenticationResult->expiration / 1000)) - 60);
}

bool noteSortByDate(Note n1, Note n2) {
	if(QvernoteSettings::Instance()->getNoteDisplayOrder() == QvernoteSettings::NDO_ASC)
		return (n1.created >= n2.created);

	return (n1.created <= n2.created);
}

bool	QvernoteAPI::loadNotes(int maxNotes, const Notebook& notebook) {
	NoteFilter filter;

	if(notebook.guid.size() > 0)
	{
		filter.notebookGuid = notebook.guid;
		filter.__isset.notebookGuid = true;
	}

	if(m_LocalStoreClient->isEnabled())
	{
		m_LocalStoreClient->findNotes(m_NoteList, filter);
	}
	else
	{
		try {
			reinitNoteStore();
			m_NoteStoreClient->findNotes(m_NoteList, getAuthenticationToken(), filter, 0, maxNotes);
		} catch(EDAMUserException& e) {
			qDebug() << __FUNCTION__  << e.what();
			setError(e.parameter, e.errorCode);
			return false;
		} catch(EDAMNotFoundException& ne) {
			qDebug() << __FUNCTION__ << ne.identifier.c_str();
			setError(ne.identifier, 0);
			return false;
		} catch(TTransportException& te) {
			setError(te.what(), te.getType());
			return false;
		}
	}


	std::sort(m_NoteList.notes.begin(), m_NoteList.notes.end(), noteSortByDate);

	return true;
}

bool	QvernoteAPI::getNote(Note& note)
{
	if(m_LocalStoreClient->isEnabled())
	{
		m_LocalStoreClient->getNote(note, note.guid);
	}
	else
	{
		try {
			reinitNoteStore();
			m_NoteStoreClient->getNote(note, getAuthenticationToken(), note.guid, true, true, true, true);
		} catch(EDAMUserException& e) {
			setError(e.parameter, e.errorCode);
			return false;
		} catch(EDAMNotFoundException& ne) {
			qDebug() << __FUNCTION__ << ne.identifier.c_str();
			setError(ne.identifier, 0);
			return false;
		}
	}
	return true;
}

bool	QvernoteAPI::getNoteContent(Note& note)
{
	if(m_LocalStoreClient->isEnabled())
	{
		m_LocalStoreClient->getNoteContent(note);
	}

	return true;
}

bool	QvernoteAPI::getFirstNote(Note& note) {
	if(m_NoteList.notes.size() == 0)
		return false;

	m_NoteIterator = m_NoteList.notes.begin();

	if((*m_NoteIterator).contentHash.size() != 0)
	{
		note = (*m_NoteIterator);
		//qDebug() << "Get from iterator";
		return true;
	}

	if(m_LocalStoreClient->isEnabled())
	{
		m_LocalStoreClient->getShallowNote((*m_NoteIterator), (*m_NoteIterator).guid);
	}
	else
	{
		try {
			reinitNoteStore();
			m_NoteStoreClient->getNote(/*note*/(*m_NoteIterator), getAuthenticationToken(), (*m_NoteIterator).guid, false, false, false, false);
			reinitNoteStore();
			m_NoteStoreClient->getNoteTagNames(/*note.tagNames*/(*m_NoteIterator).tagNames, getAuthenticationToken(), (*m_NoteIterator).guid);
		} catch(EDAMUserException& e) {
			setError(e.parameter, e.errorCode);
			return false;
		} catch(EDAMNotFoundException& ne) {
			qDebug() << __FUNCTION__ << ne.identifier.c_str();
			setError(ne.identifier, 0);
			return false;
		}
	}
	note = (*m_NoteIterator);
	return true;
}

bool	QvernoteAPI::getNextNote(Note& note) {
	m_NoteIterator++;

	if(m_NoteIterator != m_NoteList.notes.end()) {
		if((*m_NoteIterator).contentHash.size() != 0)
		{
			note = (*m_NoteIterator);
			//qDebug() << "Get from iterator";
			return true;
		}

		if(m_LocalStoreClient->isEnabled())
		{
			m_LocalStoreClient->getShallowNote((*m_NoteIterator), (*m_NoteIterator).guid);
		}
		else
		{
			try {
				reinitNoteStore();
				m_NoteStoreClient->getNote(/*note*/(*m_NoteIterator), getAuthenticationToken(), (*m_NoteIterator).guid, false, false, false, false);
				reinitNoteStore();
				m_NoteStoreClient->getNoteTagNames(/*note.tagNames*/(*m_NoteIterator).tagNames, getAuthenticationToken(), (*m_NoteIterator).guid);
			} catch(EDAMUserException& e) {
				setError(e.parameter, e.errorCode);
				return false;
			} catch(EDAMNotFoundException& ne) {
				qDebug() << __FUNCTION__ << ne.identifier.c_str();
				setError(ne.identifier, 0);
				return false;
			}
		}

		note = (*m_NoteIterator);
		return true;
	}

	return false;
}

bool	QvernoteAPI::createNewNote(Note& newNote) {
	Note createdNote;

	QMutexLocker locker(&dbOpMutex);

	newNote.__isset.notebookGuid = true;
	newNote.__isset.title = true;
	newNote.__isset.content = true;
	newNote.__isset.tagGuids = true;

	if(m_LocalStoreClient->isEnabled())
	{
		m_LocalStoreClient->createNote(newNote);
		m_NoteList.notes.push_back(newNote);
	}
	else
	{
		try {
			reinitNoteStore();
			m_NoteStoreClient->createNote(createdNote, getAuthenticationToken(), newNote);
		} catch(EDAMUserException& e) {
			qDebug() << e.errorCode << QString(((string)e.parameter).c_str());
			setError(e.parameter, e.errorCode);
			return false;
		} catch(EDAMNotFoundException& ne) {
			qDebug() << __FUNCTION__ << ne.identifier.c_str();
			setError(ne.identifier, 0);
			return false;
		}

		m_NoteList.notes.push_back(createdNote);
	}

	return true;
}

bool 	QvernoteAPI::updateExistingNote(Note& existingNote) {
	Note updatedNote;

	QMutexLocker locker(&dbOpMutex);

	vector<Note>::iterator pos;
	existingNote.__isset.guid = true;
	existingNote.__isset.title = true;
	existingNote.__isset.content = true;
	existingNote.__isset.tagGuids = true;

	if(m_LocalStoreClient->isEnabled())
	{
		m_LocalStoreClient->updateNote(existingNote);
		if(searchNoteList(pos, existingNote.guid)) {
			m_NoteList.notes.erase(pos);
			m_NoteList.notes.insert(pos, existingNote);
		}
	}
	else
	{
		try {
			reinitNoteStore();
			m_NoteStoreClient->updateNote(updatedNote, getAuthenticationToken(), existingNote);
		} catch(EDAMUserException& e) {
			qDebug() << e.errorCode << QString(((string)e.parameter).c_str());
			setError(e.parameter, e.errorCode);
			return false;
		} catch(EDAMNotFoundException& ne) {
			qDebug() << __FUNCTION__ << ne.identifier.c_str();
			setError(ne.identifier, 0);
			return false;
		}

		if(searchNoteList(pos, existingNote.guid)) {
			m_NoteList.notes.erase(pos);
			m_NoteList.notes.insert(pos, updatedNote);
		}
	}

	return true;
}

bool	QvernoteAPI::deleteNote(Guid noteGuid) {
	QMutexLocker locker(&dbOpMutex);

	if(m_LocalStoreClient->isEnabled())
	{
		m_LocalStoreClient->deleteNote(noteGuid);
	}
	else
	{
		try {
			reinitNoteStore();
			m_NoteStoreClient->deleteNote(getAuthenticationToken(), noteGuid);
		} catch(EDAMUserException& e) {
			qDebug() << e.errorCode << QString(((string)e.parameter).c_str());
			setError(e.parameter, e.errorCode);
			return false;
		} catch(EDAMNotFoundException& ne) {
			qDebug() << __FUNCTION__ << ne.identifier.c_str();
			setError(ne.identifier, 0);
			return false;
		}
	}
	vector<Note>::iterator pos;
	if(searchNoteList(pos, noteGuid)) {
		m_NoteList.notes.erase(pos);
	}
	return true;
}

bool 	QvernoteAPI::searchNotes(const NoteFilter& filter, int maxResults) {
	if(m_LocalStoreClient->isEnabled())
	{
		m_LocalStoreClient->findNotes(m_NoteList, filter);
	}
	else
	{
		try {
			reinitNoteStore();
			m_NoteStoreClient->findNotes(m_NoteList, getAuthenticationToken(), filter, 0, maxResults);
		} catch(EDAMUserException& e) {
			qDebug() << e.what();
			setError(e.parameter, e.errorCode);
			return false;
		}
	}
	return true;
}

bool	QvernoteAPI::searchNoteList(vector<Note>::iterator& pos, Guid noteGuid) {
	for(pos = m_NoteList.notes.begin(); pos < m_NoteList.notes.end(); pos++) {
		if((*pos).guid == noteGuid)
			return true;
	}

	return false;
}

bool	QvernoteAPI::expungeNote(const Guid noteGuid) {
	QMutexLocker locker(&dbOpMutex);

	m_LocalStoreClient->expungeNote(noteGuid);

	try {
		reinitNoteStore();
		m_NoteStoreClient->expungeNote(getAuthenticationToken(), noteGuid);
	} catch(EDAMUserException& e) {
		qDebug() << e.what();
		setError(e.parameter, e.errorCode);
		return false;
	}

	return true;
}

bool 	QvernoteAPI::copyNote(Guid noteGuid, Guid toNotebookGuid) {
	QMutexLocker locker(&dbOpMutex);

	Note copiedNote;

	if(m_LocalStoreClient->isEnabled())
	{
		m_LocalStoreClient->copyNote(noteGuid, toNotebookGuid);
	}
	else
	{
		try {
			reinitNoteStore();
			m_NoteStoreClient->copyNote(copiedNote, getAuthenticationToken(), noteGuid, toNotebookGuid);
		} catch(EDAMUserException& e) {
			qDebug() << e.what();
			setError(e.parameter, e.errorCode);
			return false;
		} catch(EDAMNotFoundException& nfe) {
			qDebug() << __FUNCTION__  << nfe.what();
			setError(nfe.identifier, 0);
			return false;
		}
	}
	return true;
}

bool 	QvernoteAPI::moveNote(Guid noteGuid, Guid toNotebookGuid) {
	if(copyNote(noteGuid, toNotebookGuid))
		return deleteNote(noteGuid);

	return false;
}

bool	QvernoteAPI::undeleteNote(Note& note) {
	note.active = true;
	note.deleted = 0;
	note.__isset.active = true;
	note.__isset.deleted = true;

	return updateExistingNote(note);
}

bool 	QvernoteAPI::loadTrashNoteList(vector<Note>& noteList) {
	NoteFilter filter;

	if(m_LocalStoreClient->isEnabled())
	{
		m_LocalStoreClient->getTrashNotes(noteList);
	}
	else
	{
		qDebug() << "Trash is not available without offline sync mode";
	}
}

bool	QvernoteAPI::loadResource(Resource& loadedResource, Guid resourceGuid)
{
	if(m_LocalStoreClient->isEnabled())
	{
		m_LocalStoreClient->getResource(loadedResource, resourceGuid);
	}
	else
	{
		try {
			reinitNoteStore();
			m_NoteStoreClient->getResource(loadedResource, getAuthenticationToken(), resourceGuid, true, true, true, true);
		} catch (EDAMUserException& e) {
			qDebug() << e.what();
			setError(e.parameter, e.errorCode);
			return false;
		} catch (EDAMNotFoundException& ne) {
			qDebug() << __FUNCTION__ << ne.what();
			setError(ne.identifier, 0);
			return false;
		}
	}
	return true;
}


bool	QvernoteAPI::loadResourceByHash(Resource& loadedResource, Guid noteGuid, QString hexContentHash)
{
	if(m_LocalStoreClient->isEnabled())
	{
		return m_LocalStoreClient->getResourceByHash(loadedResource, noteGuid, hexContentHash);
	}
	else
	{
		try {
			reinitNoteStore();
			QByteArray mediaHash = QByteArray::fromHex(hexContentHash.toAscii());
			string initialBodyHashString((const char*)mediaHash, mediaHash.size());
			string finalBodyHashString = string(initialBodyHashString.begin(), initialBodyHashString.begin() + mediaHash.size());
			m_NoteStoreClient->getResourceByHash(loadedResource, getAuthenticationToken(), noteGuid, finalBodyHashString, true, false, false);
		} catch (EDAMUserException& e) {
			qDebug() << e.what();
			setError(e.parameter, e.errorCode);
			return false;
		} catch (EDAMNotFoundException& ne) {
			qDebug() << __FUNCTION__ << ne.what();
			setError(ne.identifier, 0);
			return false;
		} catch(TException& te) {
			qDebug() << te.what();
			setError(te.what(), 0);
			return false;
		}
	}
	return true;
}

bool QvernoteAPI::getDefaultNotebook(Notebook& defaultNotebook) {
	vector<Notebook>::iterator it;

	for(it = m_NotebookList.begin(); it < m_NotebookList.end(); it++) {
		if((*it).defaultNotebook == true) {
			defaultNotebook = (*it);
			return true;
		}
	}
	return false;
}

bool QvernoteAPI::loadNotebookList() {
	qDebug() << "Getting list of notebooks";

	NoteFilter filter;
	m_NotebookList.clear();
	m_NoteCounts.notebookCounts.clear();
	m_NoteCounts.tagCounts.clear();
	m_TagsPerNotebook.clear();

	if(m_LocalStoreClient->isEnabled())
	{
		m_LocalStoreClient->listNotebooks(m_NotebookList);
		m_LocalStoreClient->findNoteCounts(m_NoteCounts);
	}
	else
	{
		try {
			reinitNoteStore();
			m_NoteStoreClient->listNotebooks(m_NotebookList, getAuthenticationToken());
			reinitNoteStore();
			m_NoteStoreClient->findNoteCounts(m_NoteCounts, getAuthenticationToken(), filter, true);

		} catch(EDAMUserException& e) {
			setError(e.parameter, e.errorCode);
			return false;
		} catch(TTransportException& te) {
			setError(te.what(), te.getType());
			return false;
		}
	}

	return true;
}

bool QvernoteAPI::getNotebookByName(Notebook& notebook, string notebookName) {
	vector<Notebook>::iterator it;

	for(it = m_NotebookList.begin(); it < m_NotebookList.end(); it++) {
		if((*it).name == notebookName) {
			notebook = (*it);
			return true;
		}
	}

	return false;
}

bool QvernoteAPI::getNotebookByGuid(Notebook& notebook, Guid notebookGuid) {
	vector<Notebook>::iterator it;

	for(it = m_NotebookList.begin(); it < m_NotebookList.end(); it++) {
		if((*it).guid == notebookGuid) {
			notebook = (*it);
			return true;
		}
	}

	return false;
}

bool QvernoteAPI::getFirstNotebook(Notebook& notebook) {
	if(m_NotebookList.size() == 0)
		return false;

	m_NotebookIterator = m_NotebookList.begin();

	notebook = (*m_NotebookIterator);

	if(m_TagsPerNotebook[notebook.name].empty())
	{
		if(m_LocalStoreClient->isEnabled())
		{
			m_LocalStoreClient->listTagsByNotebook(m_TagsPerNotebook[notebook.name], notebook.guid);
		}
		else
		{
			try {
				reinitNoteStore();
				m_NoteStoreClient->listTagsByNotebook(m_TagsPerNotebook[notebook.name], getAuthenticationToken(), notebook.guid);
			} catch(EDAMUserException& e) {
				setError(e.parameter, e.errorCode);
				return false;
			}
		}
	}

	return true;
}

bool QvernoteAPI::getNextNotebook(Notebook& notebook) {
	m_NotebookIterator++;
	if(m_NotebookIterator != m_NotebookList.end()) {
		notebook = (*m_NotebookIterator);

		if(m_TagsPerNotebook[notebook.name].empty())
		{
			if(m_LocalStoreClient->isEnabled())
			{
				m_LocalStoreClient->listTagsByNotebook(m_TagsPerNotebook[notebook.name], notebook.guid);
			}
			else
			{
				try {
					reinitNoteStore();
					m_NoteStoreClient->listTagsByNotebook(m_TagsPerNotebook[notebook.name], getAuthenticationToken(), notebook.guid);
				} catch(EDAMUserException& e) {
					setError(e.parameter, e.errorCode);
					return false;
				}
			}
		}

		return true;
	}

	return false;
}

bool QvernoteAPI::createNewNotebook(const string& notebookName, bool isDefault, bool isPublished) {
	QMutexLocker locker(&dbOpMutex);

	Notebook newNotebook;
	Notebook result;
	newNotebook.name = notebookName;
	newNotebook.defaultNotebook = isDefault;
	newNotebook.published = isPublished;
	newNotebook.__isset.name = true;
	newNotebook.__isset.published = true;
	newNotebook.__isset.defaultNotebook = true;

	if(m_LocalStoreClient->isEnabled())
	{
		m_LocalStoreClient->createNotebook(newNotebook);
	}
	else
	{
		try {
			reinitNoteStore();
			m_NoteStoreClient->createNotebook(result, getAuthenticationToken(), newNotebook);
		} catch(EDAMUserException& e) {
			qDebug() << e.errorCode << QString(((string)e.parameter).c_str());
			setError(e.parameter, e.errorCode);
			return false;
		}
	}

	m_NotebookList.push_back(newNotebook);

	return true;
}

bool QvernoteAPI::deleteNotebook(Notebook& notebook) {
	QMutexLocker locker(&dbOpMutex);

	if(m_LocalStoreClient->isEnabled())
	{
		m_LocalStoreClient->expungeNotebook(notebook.guid);
	}
	else
	{
		try {
			reinitNoteStore();
			m_NoteStoreClient->expungeNotebook(getAuthenticationToken(), notebook.guid);
		} catch(EDAMUserException& e) {
			qDebug() << e.errorCode << QString(((string)e.parameter).c_str());
			setError(e.parameter, e.errorCode);
			return false;
		}
	}


	return true;
}

int	 QvernoteAPI::getNotebookCount() {
	return m_NotebookList.size();
}

int  QvernoteAPI::getNotebookContentsCount(Notebook& notebook) {
	return m_NoteCounts.notebookCounts[notebook.guid];
}

int QvernoteAPI::getTrashCount() {
	return m_NoteCounts.trashCount;
}

bool QvernoteAPI::getFirstNotebookTag(const string& notebookName, Tag& tag) {
	if(m_TagsPerNotebook[notebookName].empty())
		return false;

	m_TagsPerNotebookIterator = m_TagsPerNotebook[notebookName].begin();

	tag = (*m_TagsPerNotebookIterator);
	return true;
}

bool QvernoteAPI::getNextNotebookTag(const string& notebookName, Tag& tag) {
	m_TagsPerNotebookIterator++;

	if(m_TagsPerNotebookIterator != m_TagsPerNotebook[notebookName].end()) {
		tag = (*m_TagsPerNotebookIterator);
		return true;
	}

	return false;
}

bool QvernoteAPI::addNewTag(const string& newTagName, Tag& result) {
	QMutexLocker locker(&dbOpMutex);

	Tag newTag;

	newTag.name = newTagName;
	newTag.__isset.name = true;


	if(m_LocalStoreClient->isEnabled())
	{
		m_LocalStoreClient->createTag(newTag);
		result = newTag;
		m_TagList.push_back(newTag);
	}
	else
	{
		try {
			reinitNoteStore();
			m_NoteStoreClient->createTag(result, getAuthenticationToken(), newTag);
			m_TagList.push_back(result);
		} catch(EDAMUserException& e) {
			qDebug() << e.errorCode << QString(((string)e.parameter).c_str());
			setError(e.parameter, e.errorCode);
			return false;
		}
	}

	return true;
}

int QvernoteAPI::updateTag(Tag& updatedTag) {
	QMutexLocker locker(&dbOpMutex);

	updatedTag.__isset.guid = updatedTag.__isset.name =	updatedTag.__isset.parentGuid =	true;
	int newUSN;

	if(m_LocalStoreClient->isEnabled())
	{
		m_LocalStoreClient->updateTag(updatedTag);
	}
	else
	{
		try {
			reinitNoteStore();
			newUSN = m_NoteStoreClient->updateTag(getAuthenticationToken(), updatedTag);
		} catch(EDAMUserException& e) {
			qDebug() << e.errorCode << QString(((string)e.parameter).c_str());
			setError(e.parameter, e.errorCode);
			return 0;
		}
	}
	return newUSN;
}

bool QvernoteAPI::loadTagList() {
	QMutexLocker locker(&dbOpMutex);

	if(m_LocalStoreClient->isEnabled())
	{
		m_LocalStoreClient->listTags(m_TagList);
	}
	else
	{
		try {
			reinitNoteStore();
			m_NoteStoreClient->listTags(m_TagList, getAuthenticationToken());
		} catch(EDAMUserException& e) {
			qDebug() << e.errorCode << QString(((string)e.parameter).c_str());
			setError(e.parameter, e.errorCode);
			return false;
		}
	}
	return true;
}

bool QvernoteAPI::getFirstTag(Tag& tag) {
	if(m_TagList.size() == 0)
		return false;

	m_TagIterator = m_TagList.begin();

	tag = *m_TagIterator;
	m_TagIterator++;
	return true;
}

bool QvernoteAPI::getNextTag(Tag& tag) {
	if(m_TagIterator == m_TagList.end())
		return false;

	tag = *m_TagIterator;
	m_TagIterator++;
	return true;
}

bool QvernoteAPI::addExistingTag(const Tag& tag, Note& note) {
	note.tagGuids.push_back(tag.guid);
	note.tagNames.push_back(tag.name);
	return true;
}

bool QvernoteAPI::removeExistingTag(const Tag& tag, Note& note) {
	vector<Guid>::iterator itGuid = find(note.tagGuids.begin(), note.tagGuids.end(), tag.guid);
	if(itGuid != note.tagGuids.end())
		note.tagGuids.erase(itGuid);
	else
		return false;

	vector<string>::iterator itStr = find(note.tagNames.begin(), note.tagNames.end(), tag.name);
	if(itStr != note.tagNames.end())
		note.tagNames.erase(itStr);
	else
		return false;

	return true;
}

bool tagSortByNameAsc(Tag t1, Tag t2) {
	return (t1.name <= t2.name);
}

bool tagSortByNameDesc(Tag t1, Tag t2) {
	return (t1.name >= t2.name);
}

bool QvernoteAPI::sortTags(bool order) {
	std::sort(m_TagList.begin(), m_TagList.end(), (order)? tagSortByNameAsc : tagSortByNameDesc);
}

bool QvernoteAPI::emailNote(Note& note, string emailAddress) {
	NoteEmailParameters p;
	vector<string> addrList;

	addrList.push_back(emailAddress);
	p.guid = note.guid;
	p.toAddresses = addrList;

	p.__isset.guid = true;
	p.__isset.toAddresses = true;
	try {
		reinitNoteStore();
		m_NoteStoreClient->emailNote(getAuthenticationToken(), p);
	} catch (EDAMUserException& e) {
		qDebug() << e.what();
		setError(e.parameter, e.errorCode);
		return false;
	} catch (EDAMNotFoundException& ne) {
		qDebug() << ne.what();
		setError(ne.identifier, 0);
		return false;
	} catch(TException& te) {
		qDebug() << te.what();
		setError(te.what(), 0);
		return false;
	}

	return true;
}

bool QvernoteAPI::getSyncState(SyncState& syncState)
{
	try {
		reinitNoteStore();
		m_NoteStoreClient->getSyncState(syncState, getAuthenticationToken());
	} catch(EDAMUserException& e) {
		qDebug() << e.errorCode << QString(((string)e.parameter).c_str());
		setError(e.parameter, e.errorCode);
		return false;
	} catch(EDAMSystemException& se) {
		qDebug() << se.message.c_str();
		setError(se.message, se.errorCode);
		return false;
	} catch(TTransportException& te) {
		qDebug() << te.what();
		setError(te.what(), te.getType());
		return false;
	}

	return true;
}

bool QvernoteAPI::getSyncChunk(SyncChunk& syncChunk, int afterUSN, int maxEntries, bool isFullSync)
{
	try {
		reinitNoteStore();
		m_NoteStoreClient->getSyncChunk(syncChunk, getAuthenticationToken(), afterUSN, maxEntries, isFullSync);
	} catch(EDAMUserException& e) {
		qDebug() << e.errorCode << QString(((string)e.parameter).c_str());
		setError(e.parameter, e.errorCode);
		return false;
	} catch(EDAMSystemException& se) {
		qDebug() << se.message.c_str();
		setError(se.message, se.errorCode);
		return false;
	} catch(TTransportException& te) {
		setError(te.what(), te.getType());
		return false;
	}

	return true;
}

bool QvernoteAPI::synchronizeLocalStorage() {
	SyncState syncState;

	qDebug() << __FUNCTION__ << "running";

	if(!isOnline())
	{
		qDebug() << __FUNCTION__ << "Offline";
		return true;
	}

	if(syncThreadMutex.tryLock() == false)
	{
		qDebug() << __FUNCTION__ << "Failed to lock thread mutex";
		return true;
	}

	m_nLastUpdateCount = QvernoteSettings::Instance()->getLastUpdateCount();
	m_nLastSyncTime = QvernoteSettings::Instance()->getLastSyncTime();

	if(getSyncState(syncState) == true)
	{
		if(syncState.fullSyncBefore > m_nLastSyncTime)
		{
			synchronizeFull();
			sendChanges();
		}
		else if(syncState.updateCount == m_nLastUpdateCount)
		{
			sendChanges();
		}
		else
		{
			synchronizeChanges();
			sendChanges();
		}

		QvernoteSettings::Instance()->Store();
		syncThreadMutex.unlock();
		return true;
	}

	syncThreadMutex.unlock();
	return false;
}

bool QvernoteAPI::synchronizeFull()
{
	int afterUSN = 0;

	while(true)
	{
		SyncChunk syncChunk;
		if(getSyncChunk(syncChunk, afterUSN, 100, true) == true)
		{
			qDebug() << __FUNCTION__ << "Received sync data";

			if(syncChunk.chunkHighUSN < syncChunk.updateCount && syncChunk.chunkHighUSN != 0)
			{
				afterUSN = syncChunk.chunkHighUSN;
				if(!updateDB(syncChunk))
					return false;
			}
			else
			{
				if(!updateDB(syncChunk))
					return false;

				QvernoteSettings::Instance()->setLastUpdateCount(syncChunk.updateCount);
				QvernoteSettings::Instance()->setLastSyncTime(syncChunk.currentTime);
				break;
			}
		}
		else
		{
			qDebug() << __FUNCTION__ << "Failure receiving sync data";
			return false;
		}
	}

	return true;
}

bool QvernoteAPI::synchronizeChanges()
{
	SyncChunk syncChunk;
	int afterUSN = m_nLastUpdateCount;

	while(true) {
		if(getSyncChunk(syncChunk, afterUSN, 100, false) == true)
		{
			qDebug() << __FUNCTION__ << "Received sync data";
			if(syncChunk.chunkHighUSN < syncChunk.updateCount && syncChunk.chunkHighUSN != 0)
			{
				afterUSN = syncChunk.chunkHighUSN;
				if(!updateDB(syncChunk))
					return false;
			}
			else
			{
				if(!updateDB(syncChunk))
					return false;
				QvernoteSettings::Instance()->setLastUpdateCount(syncChunk.updateCount);
				QvernoteSettings::Instance()->setLastSyncTime(syncChunk.currentTime);
				break;
			}
		}
		else
		{
			qDebug() << __FUNCTION__ << "Failure receiving sync data";
			return false;
		}
	}

	return true;
}

bool QvernoteAPI::sendChanges()
{
	bool ret = true;

	ret = sendDirtyTags();
	ret &= sendDirtySearches();
	ret &= sendDirtyNotebooks();
	ret &= sendDirtyNotes();

	return ret;
}

bool QvernoteAPI::updateDB(SyncChunk& syncChunk)
{
	bool ret = true;

	QMutexLocker locker(&dbOpMutex);

	ret = updateTags(syncChunk);
	ret &= updateNotebooks(syncChunk);
	ret &= updateNotes(syncChunk);
	ret &= updateSearches(syncChunk);
	ret &= updateResources(syncChunk);

	return ret;
}

bool QvernoteAPI::updateNotes(SyncChunk& syncChunk)
{
	qDebug() << __FUNCTION__ << "Removing expunged notes";
	QStringList blacklistedNotebooks = QvernoteSettings::Instance()->getBlacklistedNotebooks().split(',');

	for(vector<Guid>::iterator i = syncChunk.expungedNotes.begin(); i != syncChunk.expungedNotes.end(); i++)
	{
		QNote::remove(m_LocalStoreClient->getDB(), *i);
	}

	for(vector<Note>::iterator i = syncChunk.notes.begin(); i != syncChunk.notes.end(); i++)
	{
		if(blacklistedNotebooks.contains(QString::fromStdString((*i).notebookGuid)))
		{
			qDebug() << __FUNCTION__ << "Blacklisted note" << (*i).title.c_str();
			continue;
		}

		qDebug() << __FUNCTION__ << "Inserting new note" << (*i).title.c_str();
		emit progressUpdated(QString::fromUtf8((*i).title.c_str()));

		try {
			reinitNoteStore();
			m_NoteStoreClient->getNote((*i), getAuthenticationToken(), (*i).guid, true, true, true, true);
		} catch(EDAMUserException& e) {
			setError(e.parameter, e.errorCode);
			qDebug() << __FUNCTION__ << "Unable to load note";
			continue;
		} catch (EDAMNotFoundException& ne) {
			qDebug() << __FUNCTION__ << ne.what();
			setError(ne.identifier, 0);
			return false;
		} catch(TTransportException& te) {
			setError(te.what(), te.getType());
			return false;
		}

		if((*i).active == false)
			QNote::addToTrash(m_LocalStoreClient->getDB(), (*i).guid);
		else
		{
			QNote::update(m_LocalStoreClient->getDB(), *i);
			emit noteContentUpdated((*i).guid);
		}
	}

	return true;
}

bool QvernoteAPI::updateNotebooks(SyncChunk& syncChunk)
{
	for(vector<Guid>::iterator i = syncChunk.expungedNotebooks.begin(); i != syncChunk.expungedNotebooks.end(); i++)
	{
		QNotebook::remove(m_LocalStoreClient->getDB(), (*i));
	}

	for(vector<Notebook>::iterator i = syncChunk.notebooks.begin(); i != syncChunk.notebooks.end(); i++)
	{
		QNotebook::update(m_LocalStoreClient->getDB(), (*i));
		emit progressUpdated(QString::fromUtf8((*i).name.c_str()));
	}

	return true;
}

bool QvernoteAPI::updateTags(SyncChunk& syncChunk)
{
	for(vector<Guid>::iterator i = syncChunk.expungedTags.begin(); i != syncChunk.expungedTags.end(); i++)
	{
		QTag::remove(m_LocalStoreClient->getDB(), (*i));
	}

	for(vector<Tag>::iterator i = syncChunk.tags.begin(); i != syncChunk.tags.end(); i++)
	{
		QTag::update(m_LocalStoreClient->getDB(), (*i));
	}

	return true;
}

bool QvernoteAPI::updateSearches(SyncChunk& syncChunk)
{
	return true;
}

bool QvernoteAPI::updateResources(SyncChunk& syncChunk)
{
	for(vector<Resource>::iterator i = syncChunk.resources.begin(); i != syncChunk.resources.end(); i++)
	{
		if((*i).attributes.fileName.empty())
			emit progressUpdated(QString("Resource: ") + QString::fromStdString((*i).mime));
		else
			emit progressUpdated(QString("Resource: ") + QString::fromUtf8((*i).attributes.fileName.c_str()));
		/*try {
			reinitNoteStore();
			m_NoteStoreClient->getResource(*i, getAuthenticationToken(), (*i).guid, true, true, true, true);
		} catch (EDAMUserException& e) {
			qDebug() << e.what();
			setError(e.parameter, e.errorCode);
			continue;
		} catch (EDAMNotFoundException& ne) {
			qDebug() << ne.what();
			setError(ne.identifier, 0);
			continue;
		}

		QNoteResource::update(m_LocalStoreClient->getDB(), *i);*/
	}
	return true;
}

bool QvernoteAPI::sendDirtyTags()
{
	Tag newTag;
	QVector<Tag> taglist;

	QTag::getNew(m_LocalStoreClient->getDB(), taglist);

	for(QVector<Tag>::iterator i = taglist.begin(); i != taglist.end(); i++)
	{
		try {
			reinitNoteStore();
			m_NoteStoreClient->createTag((*i), getAuthenticationToken(), (*i));
		} catch(EDAMUserException& e) {
			qDebug() << e.errorCode << QString(((string)e.parameter).c_str());
			setError(e.parameter, e.errorCode);
			continue;
		}

		QvernoteSettings::Instance()->setLastUpdateCount((*i).updateSequenceNum);
		QTag::updateServerData(m_LocalStoreClient->getDB(), *i);
	}

	taglist.clear();

	QTag::getDirty(m_LocalStoreClient->getDB(), taglist);
	int newUSN;
	for(QVector<Tag>::iterator i = taglist.begin(); i != taglist.end(); i++)
	{
		try {
			reinitNoteStore();
			newUSN = m_NoteStoreClient->updateTag(getAuthenticationToken(), *i);
		} catch(EDAMUserException& e) {
			qDebug() << e.errorCode << QString(((string)e.parameter).c_str());
			setError(e.parameter, e.errorCode);
			continue;
		} catch(EDAMNotFoundException& ne) {
			qDebug() << ne.identifier.c_str();
			setError(ne.identifier, 0);
			continue;
		}

		(*i).updateSequenceNum = newUSN;
		QvernoteSettings::Instance()->setLastUpdateCount((*i).updateSequenceNum);
		QTag::updateServerData(m_LocalStoreClient->getDB(), *i);
	}

	return true;
}

bool QvernoteAPI::sendDirtySearches()
{
	return true;
}

bool QvernoteAPI::sendDirtyNotebooks()
{
	Notebook newNotebook;
	Guid origGuid;

	QVector<Notebook> notebooklist;

	QNotebook::getNew(m_LocalStoreClient->getDB(), notebooklist);
	for(QVector<Notebook>::iterator i = notebooklist.begin(); i != notebooklist.end(); i++)
	{
		try {
			reinitNoteStore();
			m_NoteStoreClient->createNotebook(*i, getAuthenticationToken(), *i);
		} catch(EDAMUserException& e) {
			qDebug() << e.errorCode << QString(((string)e.parameter).c_str());
			setError(e.parameter, e.errorCode);
			continue;
		}

		QNotebook::updateServerData(m_LocalStoreClient->getDB(), *i);
	}

	notebooklist.clear();

	QNotebook::getDirty(m_LocalStoreClient->getDB(), notebooklist);
	int newUSN;
	for(QVector<Notebook>::iterator i = notebooklist.begin(); i != notebooklist.end(); i++)
	{
		try {
			reinitNoteStore();
			newUSN = m_NoteStoreClient->updateNotebook(getAuthenticationToken(), *i);
		} catch(EDAMUserException& e) {
			qDebug() << e.errorCode << QString(((string)e.parameter).c_str());
			setError(e.parameter, e.errorCode);
			continue;
		} catch(EDAMNotFoundException& ne) {
			qDebug() << ne.identifier.c_str();
			setError(ne.identifier, 0);
			continue;
		}

		(*i).updateSequenceNum = newUSN;
		QvernoteSettings::Instance()->setLastUpdateCount((*i).updateSequenceNum);
		QNotebook::updateServerData(m_LocalStoreClient->getDB(), *i);
	}

	notebooklist.clear();

	QNotebook::getDeleted(m_LocalStoreClient->getDB(), notebooklist);
	for(QVector<Notebook>::iterator i = notebooklist.begin(); i != notebooklist.end(); i++)
	{
		QNotebook::remove(m_LocalStoreClient->getDB(), (*i).guid);

		try {
			reinitNoteStore();
			m_NoteStoreClient->expungeNotebook(getAuthenticationToken(), (*i).guid);
		} catch(EDAMUserException& e) {
			qDebug() << e.errorCode << QString(((string)e.parameter).c_str());
			setError(e.parameter, e.errorCode);
			continue;
		} catch(EDAMNotFoundException& ne) {
			qDebug() << ne.identifier.c_str();
			setError(ne.identifier, 0);
			continue;
		}
	}

	return true;
}

bool QvernoteAPI::sendDirtyNotes()
{
	Note newNote;
	Guid origGuid;

	QVector<Note> notelist;

	QNote::getNew(m_LocalStoreClient->getDB(), notelist);
	for(QVector<Note>::iterator i = notelist.begin(); i != notelist.end(); i++)
	{
		// Populate note resources
		QNote::loadResourceData(m_LocalStoreClient->getDB(), *i);

		try {
			reinitNoteStore();
			m_NoteStoreClient->createNote(*i, getAuthenticationToken(), *i);
		} catch(EDAMUserException& e) {
			qDebug() << e.errorCode << QString(((string)e.parameter).c_str());
			setError(e.parameter, e.errorCode);
			continue;
		} catch(EDAMNotFoundException& ne) {
			qDebug() << __FUNCTION__ << ne.identifier.c_str();
			setError(ne.identifier, 0);
			continue;
		}

		QvernoteSettings::Instance()->setLastUpdateCount((*i).updateSequenceNum);
		QNote::updateServerData(m_LocalStoreClient->getDB(), *i);
	}

	notelist.clear();

	QNote::getDirty(m_LocalStoreClient->getDB(), notelist);
	for(QVector<Note>::iterator i = notelist.begin(); i != notelist.end(); i++)
	{
		try {
			reinitNoteStore();
			m_NoteStoreClient->updateNote(*i, getAuthenticationToken(), *i);
		} catch(EDAMUserException& e) {
			qDebug() << e.errorCode << QString(((string)e.parameter).c_str());
			setError(e.parameter, e.errorCode);
			continue;
		} catch(EDAMNotFoundException& ne) {
			qDebug() << __FUNCTION__ << ne.identifier.c_str();
			setError(ne.identifier, 0);
			continue;
		}

		QvernoteSettings::Instance()->setLastUpdateCount((*i).updateSequenceNum);
		QNote::updateServerData(m_LocalStoreClient->getDB(), *i);
		QItem::setDirtyFlag(m_LocalStoreClient->getDB(), "notes", "guid", QString::fromStdString((*i).guid), 0);
	}

	notelist.clear();

	QNote::getDeleted(m_LocalStoreClient->getDB(), notelist);
	for(QVector<Note>::iterator i = notelist.begin(); i != notelist.end(); i++)
	{
		QNote::addToTrash(m_LocalStoreClient->getDB(), (*i).guid);
		try {
			reinitNoteStore();
			m_NoteStoreClient->deleteNote(getAuthenticationToken(), (*i).guid);
		} catch(EDAMUserException& e) {
			qDebug() << e.errorCode << QString(((string)e.parameter).c_str());
			setError(e.parameter, e.errorCode);
			continue;
		} catch(EDAMNotFoundException& ne) {
			qDebug() << __FUNCTION__ << ne.identifier.c_str();
			setError(ne.identifier, 0);
			continue;
		}
	}

	return true;
}
