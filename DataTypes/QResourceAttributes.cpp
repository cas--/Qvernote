/*
 * QResourceAttributes.hpp
 *
 *  Created on: Jun 15, 2010
 *      Author: alexr
 */

#ifndef QRESOURCEATTRIBUTES_HPP_
#define QRESOURCEATTRIBUTES_HPP_

#include <QtSql>

#include "QDataTypes.hpp"
#include "../QvernoteDataFactory.h"
//#include "../QvernoteDBSchema.h"

#define INSERT_RESOURCE_ATTRIB \
		"insert into resourceattrib (resourceid, sourceURL, timestamp, " \
		"latitude, longitude, altitude, cameraMake, cameraModel, clientWillIndex, fileName, attachment)" \
		"values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"

namespace QResourceAttributes {
	void load(QSqlDatabase* db, Guid guid, ResourceAttributes& resourceAttrib) {
		QvernoteDataFactory::createResourceAttributes(
				QItem::load(db, "resourceattrib", "resourceid", QString::fromStdString(guid)),
				resourceAttrib);
	}

	void store(QSqlDatabase* db, Guid guid, ResourceAttributes& resourceAttrib) {
		QSqlQuery q(*db);

		q.prepare(INSERT_RESOURCE_ATTRIB);
		q.addBindValue(QString::fromStdString(guid));
		q.addBindValue(QString::fromStdString(resourceAttrib.sourceURL));
		q.addBindValue(resourceAttrib.timestamp);
		q.addBindValue(resourceAttrib.latitude);
		q.addBindValue(resourceAttrib.longitude);
		q.addBindValue(resourceAttrib.altitude);
		q.addBindValue(QString::fromStdString(resourceAttrib.cameraMake));
		q.addBindValue(QString::fromStdString(resourceAttrib.cameraModel));
		q.addBindValue(resourceAttrib.clientWillIndex);
		q.addBindValue(QString::fromStdString(resourceAttrib.fileName));
		q.addBindValue(resourceAttrib.attachment);
		QItem::executeQuery(q);
	}

	void update(QSqlDatabase* db, Guid guid, ResourceAttributes& resourceAttrib) {
		QSqlQuery q(*db);

		if(!QItem::isExists(db, "resourceattrib", "resourceid", QString::fromStdString(guid)))
		{
			store(db, guid, resourceAttrib);
		}
		else
		{
			//q.prepare(UPDATE_DATA(resourceid, dataType, data.body, data.bodyHash, data.size));
		}
	}

}
#endif /* QRESOURCEATTRIBUTES_HPP_ */
