/*
 * QData.hpp
 *
 *  Created on: Jun 15, 2010
 *      Author: alexr
 */

#ifndef QDATA_HPP_
#define QDATA_HPP_

#include <QtSql>

#include "QDataTypes.hpp"
#include <Limits_constants.h>
#include "../QvernoteDataFactory.h"
#include "../QvernoteDBSchema.h"

#define INSERT_DATA \
	"insert into data (resourceid, type, body, bodyHash, size) values (:resourceid, :type, :body, :bodyHash, :size)"


#define UPDATE_DATA \
	"update data set body=:body, bodyHash=:bodyHash, size=:size where resourceid=:resourceid and type=:type"

#define LOAD_DATA_SHALLOW \
	"select bodyHash, size from data where resourceid=? and type=?"

using namespace evernote::limits;

namespace QData {
	void load(QSqlDatabase* db, Guid guid, ResourceDataType dataType, Data& data) {
		QvernoteDataFactory::createData(
				QItem::load(db, "data", "resourceid", QString::fromStdString(guid), "type="+QString::number(dataType)),
				data);
	}

	void load_shallow(QSqlDatabase* db, Guid guid, ResourceDataType dataType, Data& data) {
		QSqlQuery q(*db);

		q.prepare(LOAD_DATA_SHALLOW);
		q.addBindValue(QString::fromStdString(guid));
		q.addBindValue(dataType);
		QItem::executeQuery(q);
		q.next();

		QvernoteDataFactory::createData(
				q.record(),
				data);
	}

	void store(QSqlDatabase* db, Guid resourceid, ResourceDataType dataType, Data& data) {
		QSqlQuery q(*db);
		LimitsConstants lc;

		q.prepare(INSERT_DATA);
		q.bindValue("resourceid", QString::fromStdString(resourceid));
		q.bindValue("type", dataType);
		q.bindValue("body", QByteArray::fromRawData(data.body.c_str(), data.size));
		q.bindValue("bodyHash", QByteArray::fromRawData(data.bodyHash.c_str(), lc.EDAM_HASH_LEN));
		q.bindValue("size", data.size);
		QItem::executeQuery(q);
	}

	void update(QSqlDatabase* db, Guid resourceid, ResourceDataType dataType, Data& data) {
		QSqlQuery q(*db);
		LimitsConstants lc;

		if(!QItem::isExists(db, "data", "resourceid", QString::fromStdString(resourceid), "type="+QString::number(dataType)))
		{
			q.prepare(INSERT_DATA);
		}
		else
		{
			q.prepare(UPDATE_DATA);
		}

		q.bindValue("resourceid", QString::fromStdString(resourceid));
		q.bindValue("type", dataType);
		q.bindValue("body", QByteArray::fromRawData(data.body.c_str(), data.size));
		q.bindValue("bodyHash", QByteArray::fromRawData(data.bodyHash.c_str(), lc.EDAM_HASH_LEN));
		q.bindValue("size", data.size);
		QItem::executeQuery(q);
	}
}

#endif /* QDATA_HPP_ */
