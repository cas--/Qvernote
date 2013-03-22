/*
 * QvernoteTag.h
 *
 *  Created on: Jun 14, 2010
 *      Author: alexr
 */

#ifndef QTAG_H_
#define QTAG_H_

#include "QDataTypes.hpp"
#include "../QvernoteDataFactory.h"
//#include "../QvernoteDBSchema.h"

/* TAGS */
#define UPDATE_TAG(guid, name, parentGuid, updateSequenceNum) \
	"update tags set name='"+QString::fromUtf8(name.c_str())+"', parentGuid='"+QString::fromStdString(parentGuid)+"', updateSequenceNum="+QString::number(updateSequenceNum)+" where guid = '"+QString::fromStdString(guid)+"'"
#define LOAD_TAG(k, v) "select * from tags where " + QString(k) + " = '" + QString::fromStdString(v) + "'"
#define INSERT_TAG(guid, name, parentGuid, updateSequenceNum) \
	"insert into tags (guid, name, parentGuid, updateSequenceNum) values ('"+QString::fromStdString(guid)+"', '"+QString::fromUtf8(name.c_str())+"', '"+QString::fromStdString(parentGuid)+"', "+QString::number(updateSequenceNum)+")"

#define UPDATE_SERVER_DATA \
	"update tags set guid=?, updateSequenceNum=? where name=?"

namespace QTag {
	void load(QSqlDatabase* db, Guid guid, Tag& tag) {
		QvernoteDataFactory::createTag(
				QItem::load(db, "tags", "guid", QString::fromStdString(guid)),
				tag);

	}

	void store(QSqlDatabase* db, Tag& tag) {
		QSqlQuery q(*db);

		q.prepare(INSERT_TAG(tag.guid, tag.name, tag.parentGuid, tag.updateSequenceNum));
		QItem::executeQuery(q);
	}

	void update(QSqlDatabase* db, Tag& tag) {
		QSqlQuery q(*db);
		QSqlRecord r;

		if(!QItem::isExists(db, "tags", "guid", QString::fromStdString(tag.guid), r))
		{
			r = QItem::load(db, "tags", "name", QString::fromUtf8(tag.name.c_str()));
			if(!r.isEmpty())
			{
				tag.name = QItem::resolveConflict(db, r, QString::fromStdString(tag.guid), DELETE_ITEM("tags", "guid", QString::fromStdString(tag.guid)), tag.name);
			}

			q.prepare(INSERT_TAG(tag.guid, tag.name, tag.parentGuid, tag.updateSequenceNum));
		}
		else if(r.value("updateSequenceNum").toInt() < tag.updateSequenceNum)
		{
			qDebug() << r.value("updateSequenceNum").toInt() << r;
			// update tag
			q.prepare(UPDATE_TAG(tag.guid, tag.name, tag.parentGuid, tag.updateSequenceNum));
		}
		else
			return;

		QItem::executeQuery(q);
	}

	void updateServerData(QSqlDatabase* db, Tag& tag) {
		QSqlQuery q(*db);

		q.prepare(UPDATE_SERVER_DATA);
		q.addBindValue(QString::fromStdString(tag.guid));
		q.addBindValue(tag.updateSequenceNum);
		q.addBindValue(QString::fromStdString(tag.name));
		QItem::executeQuery(q);
	}

	void remove(QSqlDatabase* db, Guid guid) {
		QSqlQuery q(*db);
		q.prepare(DELETE_ITEM("tags", "guid", guid.c_str()));
		QItem::executeQuery(q);
	}

	void loadList(QSqlDatabase* db, QVector<Tag>& taglist) {
		QSqlQuery q(*db);

		q.prepare(LOAD_ITEM_LIST("tags"));
		if(QItem::executeQuery(q)) {
			while(q.next()) {
				Tag tag;
				QvernoteDataFactory::createTag(q.record(), tag);
				taglist.push_back(tag);
			}
		}
	}

	void loadForNote(QSqlDatabase* db, Guid noteGuid, QVector<Tag>& taglist) {
		QSqlQuery q(*db);

		q.prepare("select * from tags inner join notetags on tags.guid = notetags.tagid where noteid = '" + QString::fromStdString(noteGuid) + "'");
		if(QItem::executeQuery(q)) {
			while(q.next()) {
				Tag tag;
				QvernoteDataFactory::createTag(q.record(), tag);
				taglist.push_back(tag);
			}
		}
	}

	void loadForNotebook(QSqlDatabase* db, Guid notebookGuid, QVector<Tag>& taglist) {
		QSqlQuery q(*db);

		q.prepare("select * from tags where guid in (select tagid from notetags where noteid in (select guid from notes where notebookguid = '" + QString::fromStdString(notebookGuid) + "'))");
		QItem::executeQuery(q);
		while(q.next()) {
			Tag tag;
			QvernoteDataFactory::createTag(q.record(), tag);
			taglist.push_back(tag);
		}
	}

	void getDirty(QSqlDatabase* db, QVector<Tag>& taglist) {
		QSqlQuery q(*db);

		QItem::getDirtyItems("tags", q);
		while(q.next()) {
			Tag tag;
			load(db, q.value(0).toString().toStdString(), tag);
			taglist.push_back(tag);
		}
	}

	void getNew(QSqlDatabase* db, QVector<Tag>& taglist) {
		QSqlQuery q(*db);

		QItem::getNewItems("tags", q);
		while(q.next()) {
			Tag tag;
			load(db, q.value(0).toString().toStdString(), tag);
			taglist.push_back(tag);
		}
	}

	void loadCount(QSqlDatabase* db, map<Guid, int>& tagCounts) {
		QSqlQuery q(*db);

		q.prepare("select tagid,count(noteid) from notetags group by tagid");
		QItem::executeQuery(q);
		while(q.next()) {
			tagCounts.insert(pair<string,int>(q.value(0).toString().toStdString(), q.value(1).toInt()));
		}
	}
};

#endif /* QTAG_H_ */
