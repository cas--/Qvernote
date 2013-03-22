/*
 * QResource.hpp
 *
 *  Created on: Jun 15, 2010
 *      Author: alexr
 */

#ifndef QNOTERESOURCE_HPP_
#define QNOTERESOURCE_HPP_

#include <QtSql>

#include "QDataTypes.hpp"
#include "../QvernoteDataFactory.h"
//#include "../QvernoteDBSchema.h"

#define INSERT_RESOURCE "insert into resources (guid, noteGuid, mime, width, height, duration, active, updateSequenceNum) " \
	"values (?, ?, ?, ?, ?, ?, ?, ?)"
#define UPDATE_RESOURCE "update resources set " \
	"noteGuid=?, mime=?, width=?, height=?, duration=?, active=?, updateSequenceNum=? " \
	"where guid=?"

namespace QNoteResource {
	void load(QSqlDatabase* db, Guid guid, Resource& resource) {
		QSqlQuery q(*db);
		Data data;
		Data recoData;
		Data altData;
		ResourceAttributes resourceAttrib;

		QData::load(db, guid, DT_Resource, data);
		QData::load(db, guid, DT_Recognition, recoData);
		QData::load(db, guid, DT_Alternate, altData);
		QResourceAttributes::load(db, guid, resourceAttrib);

		QvernoteDataFactory::createResource(
				QItem::load(db, "resources", "guid", QString::fromStdString(guid)),
				resourceAttrib, data, recoData, altData, resource);

	}

	void load_shallow(QSqlDatabase* db, Guid guid, Resource& resource) {
		QSqlQuery q(*db);
		Data data;
		Data recoData;
		Data altData;
		ResourceAttributes resourceAttrib;

		QData::load_shallow(db, guid, DT_Resource, data);

		// I removed this only to save time
		// I'll be needeng those later
		//QData::load_shallow(db, guid, DT_Recognition, recoData);
		//QData::load_shallow(db, guid, DT_Alternate, altData);
		QResourceAttributes::load(db, guid, resourceAttrib);

		QvernoteDataFactory::createResource(
				QItem::load(db, "resources", "guid", QString::fromStdString(guid)),
				resourceAttrib, data, recoData, altData, resource);

	}

	void loadData(QSqlDatabase* db, Resource& resource) {
		QData::load(db, resource.guid, DT_Resource, resource.data);
		QData::load(db, resource.guid, DT_Recognition, resource.recognition);
		QData::load(db, resource.guid, DT_Alternate, resource.alternateData);
	}

	void store(QSqlDatabase* db, Resource& resource) {
		QSqlQuery q(*db);

		q.prepare(INSERT_RESOURCE);
		q.addBindValue(QString::fromStdString(resource.guid));
		q.addBindValue(QString::fromStdString(resource.noteGuid));
		q.addBindValue(QString::fromStdString(resource.mime));
		q.addBindValue(resource.width);
		q.addBindValue(resource.height);
		q.addBindValue(resource.duration);
		q.addBindValue(resource.active);
		q.addBindValue(resource.updateSequenceNum);

		QItem::executeQuery(q);

		QResourceAttributes::store(db, resource.guid, resource.attributes);
		QData::store(db, resource.guid, DT_Resource, resource.data);
		QData::store(db, resource.guid, DT_Recognition, resource.recognition);
		QData::store(db, resource.guid, DT_Alternate, resource.alternateData);
	}

	void update(QSqlDatabase* db, Resource& resource) {
		QSqlQuery q(*db);
		QSqlRecord r;

		if(!QItem::isExists(db, "resources", "guid", QString::fromStdString(resource.guid), r))
		{
			store(db, resource);
			return;
		}
		else if(r.value("updateSequenceNum").toInt() < resource.updateSequenceNum)
		{
			q.prepare(UPDATE_RESOURCE);
		}
		else
			return;

		q.addBindValue(QString::fromStdString(resource.noteGuid));
		q.addBindValue(QString::fromStdString(resource.mime));
		q.addBindValue(resource.width);
		q.addBindValue(resource.height);
		q.addBindValue(resource.duration);
		q.addBindValue(resource.active);
		q.addBindValue(resource.updateSequenceNum);
		q.addBindValue(QString::fromStdString(resource.guid));

		QItem::executeQuery(q);

		QResourceAttributes::update(db, resource.guid, resource.attributes);
		QData::update(db, resource.guid, DT_Resource, resource.data);
		QData::update(db, resource.guid, DT_Recognition, resource.recognition);
		QData::update(db, resource.guid, DT_Alternate, resource.alternateData);
	}

	void remove(QSqlDatabase* db, Guid guid) {
		QItem::remove(db, "resources", "guid", QString::fromStdString(guid));
	}

	void loadForNote(QSqlDatabase* db, Guid noteGuid, QVector<Resource>& resourcelist) {
		QSqlQuery q(*db);
		Resource r;

		q.prepare("select guid from resources where noteGuid='"+QString::fromStdString(noteGuid)+"'");
		if(QItem::executeQuery(q)) {
			while(q.next()) {
				load_shallow(db, q.value(0).toString().toStdString(), r);
				resourcelist.push_back(r);
			}
		}
	}
}
#endif /* QNOTERESOURCE_HPP_ */
