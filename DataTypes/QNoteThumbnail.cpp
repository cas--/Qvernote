/*
 * QNoteThumbnail.cpp
 *
 *  Created on: Jul 4, 2010
 *      Author: alex
 */

#include <QtSql>
#include "QDataTypes.hpp"

#define LOAD_THUMBNAIL \
	"select thumbnail from thumbnails where noteid=?"

#define STORE_THUMBNAIL \
	"insert into thumbnails (noteid, notebookid, thumbnail) values (?, ?, ?)"

#define REMOVE_THUMBNAIL \
	"delete from thumbnails where noteid = ?"

#define CLEAR_THUMBNAILS \
	"delete from thumbnails"

#define LOAD_THUMBNAILS_FOR_NOTEBOOK \
	"select noteid,thumbnail from thumbnails where notebookid=?"

namespace QNoteThumbnail {

	void load(QSqlDatabase* db, Guid noteGuid, QByteArray& thumbnail) {
		QSqlQuery q(*db);

		q.prepare(LOAD_THUMBNAIL);
		q.addBindValue(QString::fromStdString(noteGuid));
		QItem::executeQuery(q);
		q.next();
		if(q.isValid())
			thumbnail = q.value(0).toByteArray();
	}

	void store(QSqlDatabase* db, Note& note, QByteArray& thumbnail) {
		QSqlQuery q(*db);

		q.prepare(STORE_THUMBNAIL);
		q.addBindValue(QString::fromStdString(note.guid));
		q.addBindValue(QString::fromStdString(note.notebookGuid));
		q.addBindValue(thumbnail);
		QItem::executeQuery(q);
	}

	void remove(QSqlDatabase* db, Guid noteGuid) {
		QSqlQuery q(*db);

		q.prepare(REMOVE_THUMBNAIL);
		q.addBindValue(QString::fromStdString(noteGuid));
		QItem::executeQuery(q);
	}

	void clear(QSqlDatabase* db) {
		QSqlQuery q(*db);

		q.prepare(CLEAR_THUMBNAILS);
		QItem::executeQuery(q);
	}

	void loadForNotebook(QSqlDatabase* db, Guid notebookGuid, QMap<QString, QByteArray>& data) {
		QSqlQuery q(*db);

		if(notebookGuid.empty())
		{
			q.prepare("select noteid,thumbnail from thumbnails");
		}
		else
		{
			q.prepare(LOAD_THUMBNAILS_FOR_NOTEBOOK);
			q.addBindValue(QString::fromStdString(notebookGuid));
		}
		QItem::executeQuery(q);

		while(q.next()) {
			data.insert(q.value(0).toString(), q.value(1).toByteArray());
		}
	}
}
