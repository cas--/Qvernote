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
#include "QvernoteStorage.h"
#include <QFile>
#include <QDebug>
#include "QvernoteDBSchema.h"

QvernoteStorage* QvernoteStorage::self = NULL;

QvernoteStorage::QvernoteStorage()
: m_bStorageEnabled(false)
{
	db = QSqlDatabase::addDatabase("QSQLITE");
	QString path(QDir::home().path());
	path.append(QDir::separator()).append(QVERNOTE_DB);
	path = QDir::toNativeSeparators(path);
	db.setDatabaseName(path);
	qDebug() << __FUNCTION__ << "Database path set to: " << path;


	if(!db.isValid())
	{
		qDebug() << __FUNCTION__ << "No valid driver for SQLITE";
		m_bStorageEnabled = false;
		return;
	}
	if(!db.open())
	{
		qDebug() << __FUNCTION__ << "Error opening db";
		m_bStorageEnabled = false;
		return;
	}

	if(db.tables().size() == 0)
	{
		if(initDB() == false)
		{
			m_bStorageEnabled = false;
			return;
		}
	}

	// check if thumbnail table exists:
	if(db.tables().indexOf("thumbnails") == -1)
	{
		QSqlQuery query(db);
		query.exec(PREPARE_NOTE_THUMBNAIL_SCRIPT);
	}

	enableForeignKeys();
	m_bStorageEnabled = true;
}

QvernoteStorage::~QvernoteStorage() {
	// TODO Auto-generated destructor stub
}

bool QvernoteStorage::initDB()
{
	QSqlQuery query(db);
	bool ret = false;

	QvernoteSettings::Instance()->setLastSyncTime(0);
	QvernoteSettings::Instance()->setLastUpdateCount(0);

	qDebug() << __FUNCTION__;

	ret = query.exec(PREPARE_NOTEBOOK_SCRIPT);
	if(ret == false)
		qDebug() << "Error creating database" << query.lastError().text();

	ret = query.exec(PREPARE_NOTEBOOK_INDEX_SCRIPT);
	if(ret == false)
		qDebug() << "Error creating database index" << query.lastError().text();

	ret &= query.exec(PREPARE_NOTE_SCRIPT);
	if(ret == false)
		qDebug() << "Error creating database" << query.lastError().text();

	ret = query.exec(PREPARE_NOTE_INDEX_SCRIPT);
	if(ret == false)
		qDebug() << "Error creating database index" << query.lastError().text();

	ret &= query.exec(PREPARE_TAG2NOTE_SCRIPT);
	if(ret == false)
		qDebug() << "Error creating database" << query.lastError().text();

	ret &= query.exec(PREPARE_NOTE_ATTRIB_SCRIPT);
	if(ret == false)
		qDebug() << "Error creating database" << query.lastError().text();

	ret = query.exec(PREPARE_NOTE_ATTRIB_INDEX_SCRIPT);
	if(ret == false)
		qDebug() << "Error creating database index" << query.lastError().text();

	ret &= query.exec(PREPARE_TAG_SCRIPT);
	if(ret == false)
		qDebug() << "Error creating database" << query.lastError().text();

	ret &= query.exec(PREPARE_SAVEDSEARCH_SCRIPT);
	if(ret == false)
		qDebug() << "Error creating database" << query.lastError().text();

	ret &= query.exec(PREPARE_RESOURCE_SCRIPT);
	if(ret == false)
		qDebug() << "Error creating database" << query.lastError().text();

	ret = query.exec(PREPARE_RESOURCE_INDEX_SCRIPT);
	if(ret == false)
		qDebug() << "Error creating database index" << query.lastError().text();

	ret &= query.exec(PREPARE_DATATAB_SCRIPT);
	if(ret == false)
		qDebug() << "Error creating database" << query.lastError().text();

	ret = query.exec(PREPARE_DATATAB_INDEX_SCRIPT);
	if(ret == false)
		qDebug() << "Error creating database index" << query.lastError().text();

	ret &= query.exec(PREPARE_RESOURCE_ATTRIB_SCRIPT);
	if(ret == false)
		qDebug() << "Error creating database" << query.lastError().text();

	ret &= query.exec(PREPARE_RESOURCE_ATTRIB_INDEX_SCRIPT);
	if(ret == false)
		qDebug() << "Error creating database index" << query.lastError().text();

	ret &= query.exec(PREPARE_NOTE_THUMBNAIL_SCRIPT);
	if(ret == false)
		qDebug() << "Error creating database" << query.lastError().text();

	ret = query.exec(PREPARE_NOTE_THUMBNAIL_INDEX_SCRIPT);
	if(ret == false)
		qDebug() << "Error creating database index" << query.lastError().text();

	ret = query.exec(PREPARE_FAVORITES_SCRIPT);
	if(ret == false)
		qDebug() << "Error creating database" << query.lastError().text();

	ret = query.exec(PREPARE_FAVORITES_INDEX_SCRIPT);
	if(ret == false)
		qDebug() << "Error creating database index" << query.lastError().text();

	int i = 0;
	while(PREPARE_FOREIGN_KEY_SCRIPT[i] != NULL)
	{
		ret &= query.exec(PREPARE_FOREIGN_KEY_SCRIPT[i]);
		if(ret == false)
		{
			qDebug() << "Error creating foreign keys" << query.lastError().text();
		}
		i++;
	}
	return true;
}

bool QvernoteStorage::listTags(vector<Tag>& tagList)
{
	QVector<Tag> qTagList;

	QTag::loadList(getDB(), qTagList);
	tagList = qTagList.toStdVector();
	return true;
}

bool QvernoteStorage::createTag(Tag& newTag)
{
	newTag.guid = getGuid().toStdString();
	newTag.updateSequenceNum = 0;
	QTag::store(getDB(), newTag);
	return true;
}

bool QvernoteStorage::updateTag(Tag& updatedTag)
{
	if(updatedTag.updateSequenceNum != 0)
		updatedTag.updateSequenceNum++;

	QTag::update(getDB(), updatedTag);

	if(updatedTag.updateSequenceNum != 0)
		QItem::setDirtyFlag(getDB(), "tags", "guid", QString::fromStdString(updatedTag.guid), 1);
}

bool QvernoteStorage::listNotebooks(std::vector<Notebook>& notebookList)
{
	QVector<Notebook> qNotebooklist;
	QNotebook::loadList(getDB(), qNotebooklist);
	notebookList = qNotebooklist.toStdVector();
	return true;
}

bool QvernoteStorage::findNoteCounts(NoteCollectionCounts& noteCounts)
{
	QNote::loadCount(getDB(), noteCounts.notebookCounts);
	QTag::loadCount(getDB(), noteCounts.tagCounts);
	return true;
}

bool QvernoteStorage::listTagsByNotebook(std::vector<Tag>& tagList, Guid notebookGuid)
{
	QVector<Tag> qTaglist;
	QTag::loadForNotebook(getDB(), notebookGuid, qTaglist);
	tagList = qTaglist.toStdVector();
	return true;
}

bool QvernoteStorage::createNotebook(Notebook& newNotebook)
{
	newNotebook.guid = getGuid().toStdString();
	newNotebook.updateSequenceNum = 0;
	QNotebook::store(getDB(), newNotebook);
	return true;
}

bool QvernoteStorage::expungeNotebook(Guid notebookGuid)
{
	QItem::setDeleted(getDB(), "notebooks", QString::fromStdString(notebookGuid));
	return true;
}

// Load shallow note by search conditions
bool QvernoteStorage::findNotes(NoteList& noteList, const NoteFilter& noteFilter)
{
	//QVector<Note> qNotelist;
	noteList.notes.clear();
	noteList.stoppedWords.clear();
	noteList.searchedWords.clear();

	QStringList sl = QString::fromStdString(noteFilter.words).split(" ");

	QNote::search(getDB(),
			sl,
			QString::fromStdString(noteFilter.notebookGuid),
			QVector<Guid>::fromStdVector(noteFilter.tagGuids),
			noteList.notes, QvernoteSettings::Instance()->getMaxNoteCount());

	noteList.startIndex = 0;
	noteList.totalNotes = noteList.notes.size();

	for(int i = 0; i < sl.size(); i++)
		noteList.searchedWords.push_back(sl[i].toStdString());

	return true;
}

bool QvernoteStorage::createNote(Note& newNote)
{
	newNote.guid = getGuid().toStdString();
	newNote.active = true;
	qlonglong creationTime = QDateTime::currentDateTime().toUTC().toTime_t();
	creationTime *= 1000;
	newNote.created = creationTime;
	newNote.updateSequenceNum = 0;

	// Update resource Guid
	for(vector<Resource>::iterator i = newNote.resources.begin(); i != newNote.resources.end(); i++)
	{
		(*i).guid = getGuid().toStdString();
		(*i).noteGuid = newNote.guid;
		(*i).active = true;
		(*i).updateSequenceNum = 0;
	}

	QNote::store(getDB(), newNote);
	return true;
}

bool QvernoteStorage::updateNote(Note& existingNote)
{
	// Do not increment usn on new notes, otherwise we wont sync them with server as new notes
	if(existingNote.updateSequenceNum != 0)
		existingNote.updateSequenceNum++;

	// Update resource Guid
	for(vector<Resource>::iterator i = existingNote.resources.begin(); i != existingNote.resources.end(); i++)
	{
		if((*i).guid.empty())
			(*i).guid = getGuid().toStdString();

		(*i).noteGuid = existingNote.guid;
	}

	QNote::update(getDB(), existingNote);

	if(existingNote.updateSequenceNum != 0)
		QItem::setDirtyFlag(getDB(), "notes", "guid", QString::fromStdString(existingNote.guid), 1);

	return true;
}

bool QvernoteStorage::getNote(Note& note, Guid guid)
{
	QNote::load(getDB(), guid, note);
	return true;
}

bool QvernoteStorage::getShallowNote(Note& note, Guid guid)
{
	QNote::load_shallow(getDB(), guid, note);
	return true;
}

bool QvernoteStorage::getNoteContent(Note& note)
{
	if(getShallowNote(note, note.guid))
	{
		QNote::load_content(getDB(), note);
		return true;
	}

	return false;
}

bool QvernoteStorage::getNoteTagNames(vector<string>& tagNames, Guid noteGuid)
{
	QVector<Tag> qTagList;

	QTag::loadForNote(getDB(), noteGuid, qTagList);

	for(QVector<Tag>::iterator i = qTagList.begin(); i != qTagList.end(); i++)
	{
		tagNames.push_back((*i).name);
	}

	return true;
}

bool QvernoteStorage::deleteNote(Guid noteGuid)
{
	QItem::setDeleted(getDB(), "notes", QString::fromStdString(noteGuid));
	return true;
}

bool QvernoteStorage::expungeNote(Guid noteGuid)
{
	QNote::remove(getDB(), noteGuid);
	return true;
}

bool QvernoteStorage::copyNote(Guid noteGuid, Guid toNotebookGuid)
{
	Note note;
	QNote::load(getDB(), noteGuid, note);
	note.notebookGuid = toNotebookGuid;
	return createNote(note);
}

bool QvernoteStorage::moveNote(Guid noteGuid, Guid toNotebookGuid)
{
	copyNote(noteGuid, toNotebookGuid);
	return deleteNote(noteGuid);
}

bool QvernoteStorage::getTrashNotes(vector<Note>& noteList)
{
	QVector<Note> qNoteList;
	QNote::getTrash(getDB(), qNoteList);

	noteList = qNoteList.toStdVector();
	return true;
}

bool QvernoteStorage::getFavoriteNotes(vector<Note>& noteList)
{
	QVector<Note> qNoteList;
	QNote::getFavorites(getDB(), qNoteList);

	noteList = qNoteList.toStdVector();
	return true;
}

bool QvernoteStorage::getResource(Resource& loadedResource, Guid resourceGuid)
{
	// Since the note already has the resource, just return the pointer to it
	/*for(vector<Resource>::iterator i = note.resources.begin(); i != note.resources.end(); i++)
	{
		if((*i).guid == resourceGuid)
		{
			loadedResource = *i;
			return true;
		}
	}
	*/

	QNoteResource::load(getDB(), resourceGuid, loadedResource);
	return true;
}

bool QvernoteStorage::getResourceByHash(Resource& loadedResource, Guid noteGuid, QString  hexContentHash)
{
	QVector<Resource> qResourceList;

	QNoteResource::loadForNote(getDB(), noteGuid, qResourceList);

	for(QVector<Resource>::iterator i = qResourceList.begin(); i != qResourceList.end(); i++)
	{
		QByteArray resourceHash = QByteArray::fromRawData((*i).data.bodyHash.c_str(), (*i).data.bodyHash.size());
		if(resourceHash.toHex() == hexContentHash)
		{
			QNoteResource::load(getDB(), (*i).guid, loadedResource);
			return true;
		}
	}

	return false;
}

bool QvernoteStorage::getNoteThumbnail(Guid noteGuid, QByteArray& thumbnail)
{
	thumbnail.clear();
	QNoteThumbnail::load(getDB(), noteGuid, thumbnail);
	return (thumbnail.size() > 0);
}

bool QvernoteStorage::setNoteThumbnail(Note& note, QByteArray& thumbnail)
{
	QNoteThumbnail::store(getDB(), note, thumbnail);
	return true;
}

bool QvernoteStorage::loadThumbnails(Guid& notebookGuid, QMap<QString, QByteArray>& data)
{
	QNoteThumbnail::loadForNotebook(getDB(), notebookGuid, data);
	return true;
}

bool QvernoteStorage::clearThumbnails()
{
	QNoteThumbnail::clear(getDB());
	return true;
}

bool QvernoteStorage::isNoteFavorite(Guid noteGuid)
{
	return QFavorite::isFavorite(getDB(), noteGuid);
}

bool QvernoteStorage::addFavorite(Guid noteGuid)
{
	QFavorite::insert(getDB(), noteGuid);
	return true;
}

bool QvernoteStorage::removeFavorite(Guid noteGuid)
{
	QFavorite::remove(getDB(), noteGuid);
	return true;
}

bool QvernoteStorage::clearDatabase()
{
	QNotebook::clear(getDB());
	return true;
}
