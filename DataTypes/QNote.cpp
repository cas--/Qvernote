/*
 * QNote.hpp
 *
 *  Created on: Jun 15, 2010
 *      Author: alexr
 */

#ifndef QNOTE_HPP_
#define QNOTE_HPP_

#include <QtSql>

#include "QDataTypes.hpp"
#include "../QvernoteDataFactory.h"
//#include "../QvernoteDBSchema.h"

#define INSERT_NOTE \
	"insert into notes (guid, title, content, contentHash, contentLength, created, updated, deleted, active, updateSequenceNum, notebookGuid)" \
	"values (:guid, :title, :content, :contentHash, :contentLength, :created, :updated, :deleted, :active, :updateSequenceNum, :notebookGuid)"
#define UPDATE_NOTE "update notes set " \
	"title=:title, content=:content, contentHash=:contentHash, contentLength=:contentLength, created=:created, updated=:updated, deleted=:deleted, active=:active, updateSequenceNum=:updateSequenceNum, notebookGuid=:notebookGuid " \
	"where guid=:guid"
#define INSERT_NOTETAG(tagid, noteid) "insert into notetags (tagid, noteid) values ('"+QString::fromStdString(tagid)+"','"+QString::fromStdString(noteid)+"')"

#define UPDATE_SERVER_DATA \
	"update notes set guid=?, updateSequenceNum=?, created=?, updated=?, deleted=? where title=? and notebookGuid=? and active='true'"

#define LOAD_SHALLOW_NOTE \
	"select guid,title, contentHash, contentLength, created, updated, deleted, active, updateSequenceNum, notebookGuid from notes where guid=? and deleted = 0 and active != 'false'"

#define LOAD_NOTE_CONTENT \
	"select content from notes where guid=?"

namespace QNote {
	void load(QSqlDatabase* db, Guid guid, Note& note) {
		NoteAttributes noteAttributes;
		QVector<Tag> taglist;
		QVector<Resource> resources;
		/* satellite data
		 * QSqlRecord noteAttribRecord, QVector<Guid> tagGuids, QVector<Resource> resources, QVector<std::string> tagNames
		 */
		QNoteAttributes::load(db, guid, noteAttributes);
		QTag::loadForNote(db, guid, taglist);
		QNoteResource::loadForNote(db, guid, resources);

		QvernoteDataFactory::createNote(
				QItem::load(db, "notes", "guid", QString::fromStdString(guid)),
				noteAttributes, resources, taglist, note);
	}

	void load_shallow(QSqlDatabase* db, Guid guid, Note& note) {
		QSqlQuery q(*db);
		NoteAttributes noteAttributes;
		QVector<Tag> taglist;
		QVector<Resource> resources;
		/* satellite data
		 * QSqlRecord noteAttribRecord, QVector<Guid> tagGuids, QVector<Resource> resources, QVector<std::string> tagNames
		 */
		QNoteAttributes::load(db, guid, noteAttributes);
		QNoteResource::loadForNote(db, guid, resources);
		QTag::loadForNote(db, guid, taglist);

		q.prepare(LOAD_SHALLOW_NOTE);
		q.addBindValue(QString::fromStdString(guid));
		QItem::executeQuery(q);
		q.next();
		QvernoteDataFactory::createNote(
				//QItem::load(db, "notes", "guid", QString::fromStdString(guid)),
				q.record(),
				noteAttributes, resources, taglist, note);
	}

	void load_content(QSqlDatabase* db, Note& note) {
		QSqlQuery q(*db);
		q.prepare(LOAD_NOTE_CONTENT);
		q.addBindValue(QString::fromStdString(note.guid));
		QItem::executeQuery(q);
		q.next();
		note.content = QString(q.value(0).toString().toUtf8()).toStdString();
	}

	void loadResourceData(QSqlDatabase* db, Note& note) {
		for(vector<Resource>::iterator i = note.resources.begin(); i != note.resources.end(); i++) {
			QNoteResource::loadData(db, *i);
		}
	}

	void store(QSqlDatabase* db, Note& note) {
		QSqlQuery q(*db);

		q.prepare(INSERT_NOTE);
		q.bindValue(":guid", QString::fromStdString(note.guid));
		q.bindValue(":title", QString::fromUtf8(note.title.c_str()));
		q.bindValue(":content", QString::fromUtf8(note.content.c_str()));
		q.bindValue(":contentHash", QString::fromStdString(note.contentHash));
		q.bindValue(":contentLength", note.contentLength);
		q.bindValue(":created", note.created);
		q.bindValue(":updated", note.updated);
		q.bindValue(":deleted", note.deleted);
		q.bindValue(":active", note.active);
		q.bindValue(":updateSequenceNum", note.updateSequenceNum);
		q.bindValue(":notebookGuid", QString::fromStdString(note.notebookGuid));

		QItem::executeQuery(q);

		// Store attributes
		QNoteAttributes::store(db, note.guid, note.attributes);

		// Store note-tags
		for(vector<Guid>::iterator i = note.tagGuids.begin(); i != note.tagGuids.end(); i++) {
			q.prepare(INSERT_NOTETAG(*i, note.guid));
			QItem::executeQuery(q);
		}

		// Store resources
		for(vector<Resource>::iterator i = note.resources.begin(); i != note.resources.end(); i++) {
			QNoteResource::store(db, *i);
		}
	}

	void update(QSqlDatabase* db, Note& note) {
		QSqlQuery q(*db);
		QSqlRecord r;

		if(!QItem::isExists(db, "notes", "guid", QString::fromStdString(note.guid), r))
		{
			r = QItem::load(db, "notes", "title", QString::fromUtf8(note.title.c_str()), "active !='false' and deleted=0 and notebookguid = '" + QString::fromStdString(note.notebookGuid) + "'");
			if(!r.isEmpty())
			{
				note.title = QItem::resolveConflict(db, r, QString::fromStdString(note.guid), DELETE_ITEM("notes", "guid", QString::fromStdString(note.guid)), note.title);
			}

			q.prepare(INSERT_NOTE);
		}
		else if(r.value("updateSequenceNum").toInt() < note.updateSequenceNum || note.updateSequenceNum == 0)
		{
			//qDebug() << r.value("updateSequenceNum").toInt() << r;
			// update tag
			q.prepare(UPDATE_NOTE);

			// We want to delete all existing resources from DB before updating
			//QItem::remove(db, "resources", "noteGuid", QString::fromStdString(note.guid));
		}
		else
			return;

		q.bindValue(":guid", QString::fromStdString(note.guid));
		q.bindValue(":title", QString::fromUtf8(note.title.c_str()));
		q.bindValue(":content", QString::fromUtf8(note.content.c_str()));
		q.bindValue(":contentHash", QString::fromStdString(note.contentHash));
		q.bindValue(":contentLength", note.contentLength);
		q.bindValue(":created", note.created);
		q.bindValue(":updated", note.updated);
		q.bindValue(":deleted", note.deleted);
		q.bindValue(":active", note.active);
		q.bindValue(":updateSequenceNum", note.updateSequenceNum);
		q.bindValue(":notebookGuid", QString::fromStdString(note.notebookGuid));

		QItem::executeQuery(q);

		// Store attributes
		QNoteAttributes::update(db, note.guid, note.attributes);

		// Store note-tags
		for(vector<Guid>::iterator i = note.tagGuids.begin(); i != note.tagGuids.end(); i++) {
			if(!QItem::isExists(db, "notetags", "tagid", QString::fromStdString(*i), "noteid='"+QString::fromStdString(note.guid)+"'"))
			{
				q.prepare(INSERT_NOTETAG(*i, note.guid));
				QItem::executeQuery(q);
			}
		}

		// Store resources
		for(vector<Resource>::iterator i = note.resources.begin(); i != note.resources.end(); i++) {
			QNoteResource::update(db, *i);
		}

		//Remove thumbnail
		QNoteThumbnail::remove(db, note.guid);
	}

	void updateServerData(QSqlDatabase* db, Note& note) {
		QSqlQuery q(*db);
		//guid=?, updateSequenceNum=?, created=?, updated=?, deleted=? where title=?
		q.prepare(UPDATE_SERVER_DATA);
		q.addBindValue(QString::fromStdString(note.guid));
		q.addBindValue(note.updateSequenceNum);
		q.addBindValue(note.created);
		q.addBindValue(note.updated);
		q.addBindValue(note.deleted);
		q.addBindValue(QString::fromStdString(note.title));
		q.addBindValue(QString::fromStdString(note.notebookGuid));
		QItem::executeQuery(q);
	}

	void remove(QSqlDatabase* db, Guid guid) {
		QItem::remove(db, "notes", "guid", QString::fromStdString(guid));
	}

	void loadForNotebook(QSqlDatabase* db, Guid notebookGuid, QVector<Note>& notelist) {
		QSqlQuery q(*db);
		Note note;

		q.prepare("select guid from notes where notebookGuid  = '" + QString::fromStdString(notebookGuid) + "' and deleted = 0 and active='true'");
		if(QItem::executeQuery(q)) {
			while(q.next()) {
				load_shallow(db, q.value(0).toString().toStdString(), note);
				notelist.push_back(note);
			}
		}
	}

	void loadCount(QSqlDatabase* db, map<Guid, int>& notebookCounts) {
		QSqlQuery q(*db);

		q.prepare("select notebookguid,count(guid) from notes where deleted = 0 and active='true' group by notebookguid");
		QItem::executeQuery(q);
		while(q.next()) {
			notebookCounts.insert(pair<string,int>(q.value(0).toString().toStdString(), q.value(1).toInt()));
		}
	}

	void search(QSqlDatabase* db, QStringList words, QString notebookGuid, QVector<Guid> tagGuids, vector<Note>& noteList, int maxNoteCount) {
		QSqlQuery q(*db);
		QString searchCriteria = "";
		Note note;

		if(words[0].length() > 0) {
			q.prepare("create temporary table searchwords (word TEXT)");
			QItem::executeQuery(q);
			q.prepare("insert into searchwords (word) values (?)");
			for(int i = 0; i < words.count(); i++) {
				q.bindValue(0, words[i]);
				QItem::executeQuery(q);
			}

			searchCriteria += " inner join searchwords on notes.content like '%'|| searchwords.word || '%'";
		}

		if(tagGuids.count() > 0) {
			q.prepare("create temporary table searchtags (guid TEXT)");
			QItem::executeQuery(q);
			q.prepare("insert into searchtags (guid) values (?)");
			for(int i = 0; i < words.size(); i++) {
				q.bindValue(0, QString::fromStdString(tagGuids[i]));
				QItem::executeQuery(q);
			}

			searchCriteria += " inner join notetags on notes.guid = notetags.noteid inner join searchtags on notetags.tagid = searchtags.guid";
		}

		if(notebookGuid.size() > 0) {
			searchCriteria += " where notes.notebookGuid = '" + notebookGuid + "' and notes.deleted = 0 and notes.active = 'true'";
		}

		//qDebug() << "select guid from notes " << searchCriteria;
		if(maxNoteCount <= 0)
			q.prepare("select notes.guid from notes " + searchCriteria);
		else
			q.prepare("select notes.guid from notes " + searchCriteria + " order by notes.created DESC limit " + QString::number(maxNoteCount));

		QItem::executeQuery(q);
		while(q.next()) {
			load_shallow(db, q.value(0).toString().toStdString(), note);
			noteList.push_back(note);
		}

		q.exec("drop table searchwords");
		q.exec("drop table searchtags");
	}

	void getDirty(QSqlDatabase* db, QVector<Note>& notelist) {
		QSqlQuery q(*db);

		QItem::getDirtyItems("notes", q);
		while(q.next()) {
			Note note;
			load(db, q.record().value("guid").toString().toStdString(), note);
			notelist.push_back(note);
		}
	}

	void getNew(QSqlDatabase* db, QVector<Note>& notelist) {
		QSqlQuery q(*db);

		QItem::getNewItems("notes", q, "active='true'");
		while(q.next()) {
			Note note;
			load(db, q.value(0).toString().toStdString(), note);
			notelist.push_back(note);
		}
	}

	void getDeleted(QSqlDatabase* db, QVector<Note>& notelist) {
		QSqlQuery q(*db);

		q.prepare("select guid from notes where deleted != 0 and active = 'true'");
		QItem::executeQuery(q);
		while(q.next()) {
			Note note;
			load(db, q.value(0).toString().toStdString(), note);
			notelist.push_back(note);
		}
	}

	void getTrash(QSqlDatabase* db, QVector<Note>& notelist) {
		QSqlQuery q(*db);

		q.prepare("select guid from notes where active = 'false'");
		QItem::executeQuery(q);
		while(q.next()) {
			Note note;
			load(db, q.value(0).toString().toStdString(), note);
			notelist.push_back(note);
		}
	}

	void addToTrash(QSqlDatabase* db, Guid noteGuid) {
		QSqlQuery q(*db);

		q.prepare("update notes set active='false' where guid='"+QString::fromStdString(noteGuid)+"'");
		QItem::executeQuery(q);
		//Remove thumbnail
		QNoteThumbnail::remove(db, noteGuid);
	}

	void getFavorites(QSqlDatabase* db, QVector<Note>& notelist) {
		QSqlQuery q(*db);
		QVector<Guid> noteGuidList;
		Note note;

		QFavorite::load(db, noteGuidList);
		foreach(Guid noteGuid, noteGuidList) {
			load_shallow(db, noteGuid, note);
			notelist.push_back(note);
		}
	}
}
#endif /* QNOTE_HPP_ */
