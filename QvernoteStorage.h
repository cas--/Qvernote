/*
 * QvernoteStorage.h
 *
 *  Created on: Jun 12, 2010
 *      Author: alex
 */

#ifndef QVERNOTESTORAGE_H_
#define QVERNOTESTORAGE_H_

#include <QObject>
#include "QvernoteSettings.h"
#include <QtSql>
//#include "QvernoteDataFactory.h"
#include "DataTypes/QDataTypes.hpp"


#ifdef Q_WS_MAEMO_5
#define QVERNOTE_DB "/MyDocs/qvernote/qvernote.db"
#else
#define QVERNOTE_DB "/.config/qvernote.db"
#endif

class QvernoteStorage {
public:
	static QvernoteStorage* Instance() {
		if(self == NULL)
			self = new QvernoteStorage();
		return self;
	}

	bool isEnabled() { return m_bStorageEnabled; }
	QSqlDatabase* getDB() { return &db; }

	bool listTags(vector<Tag>& tagList);
	bool createTag(Tag& newTag);
	bool updateTag(Tag& updatedTag);
	bool listNotebooks(std::vector<Notebook>& notebookList);
	bool findNoteCounts(NoteCollectionCounts& noteCounts);
	bool listTagsByNotebook(std::vector<Tag>& tagList, Guid notebookGuid);
	bool createNotebook(Notebook& newNotebook);
	bool expungeNotebook(Guid notebookGuid);
	bool findNotes(NoteList& noteList, const NoteFilter& noteFilter);
	bool createNote(Note& newNote);
	bool updateNote(Note& existingNote);
	bool getNote(Note& note, Guid guid);
	bool getShallowNote(Note& note, Guid guid);
	bool getNoteContent(Note& note);
	bool getNoteTagNames(vector<string>& tagNames, Guid noteGuid);
	bool deleteNote(Guid noteGuid);
	bool expungeNote(Guid noteGuid);
	bool copyNote(Guid noteGuid, Guid toNotebookGuid);
	bool moveNote(Guid noteGuid, Guid toNotebookGuid);
	bool getTrashNotes(vector<Note>& noteList);
	bool getFavoriteNotes(vector<Note>& noteList);
	bool getResource(Resource& loadedResource, Guid resourceGuid);
	bool getResourceByHash(Resource& loadedResource, Guid noteGuid, QString  hexContentHash);

	bool getNoteThumbnail(Guid noteGuid, QByteArray& thumbnail);
	bool setNoteThumbnail(Note& note, QByteArray& thumbnail);
	bool loadThumbnails(Guid& notebookGuid, QMap<QString, QByteArray>& data);
	bool clearThumbnails();

	bool isNoteFavorite(Guid noteGuid);
	bool addFavorite(Guid noteGuid);
	bool removeFavorite(Guid noteGuid);

	bool clearDatabase();

private:
	QvernoteStorage();
	virtual ~QvernoteStorage();

	void enableForeignKeys() {
		qDebug() << __FUNCTION__;
		QSqlQuery query(db);
		query.exec(QLatin1String("PRAGMA foreign_keys = ON"));
		query.exec("PRAGMA foreign_keys");
		while(query.next())
			qDebug() << "PRAGMA foreign_keys: " << query.value(0).toInt();
	}

	bool initDB();
	QString getGuid() {
		QString guid = QUuid::createUuid().toString();
		guid.chop(1);guid.remove(0, 1);
		return guid;
	}

private:
	static QvernoteStorage* self;
	bool m_bStorageEnabled;
	QSqlDatabase db;
};

#endif /* QVERNOTESTORAGE_H_ */
