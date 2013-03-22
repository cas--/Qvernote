/*
 * QNoteAttributes.hpp
 *
 *  Created on: Jun 15, 2010
 *      Author: alexr
 */

#ifndef QNOTEATTRIBUTES_HPP_
#define QNOTEATTRIBUTES_HPP_

#include <QtSql>

#include "QDataTypes.hpp"
#include "../QvernoteDataFactory.h"
//#include "../QvernoteDBSchema.h"

#define INSERT_NOTE_ATTRIB \
	"insert into noteattributes (noteid, subjectDate, latitude, longitude, altitude, author, source, sourceURL, sourceApplication)" \
	"values (?, ?, ?, ?, ?, ?, ?, ?, ?)"
#define UPDATE_NOTE_ATTRIB \
	"update noteattributes set subjectDate=?, latitude=?, longitude=?, altitude=?, author=?, source=?, sourceURL=?, sourceApplication=? " \
    "where noteid=?"

namespace QNoteAttributes {
	void load(QSqlDatabase* db, Guid guid, NoteAttributes& noteAttributes) {
		QvernoteDataFactory::createNoteAttributes(
				QItem::load(db, "noteattributes", "noteid", QString::fromStdString(guid)),
				noteAttributes);
	}

	void store(QSqlDatabase* db, Guid guid, NoteAttributes& noteAttributes) {
		QSqlQuery q(*db);

		q.prepare(INSERT_NOTE_ATTRIB);
		q.addBindValue(QString::fromStdString(guid));
		q.addBindValue(noteAttributes.subjectDate);
		q.addBindValue(noteAttributes.latitude);
		q.addBindValue(noteAttributes.longitude);
		q.addBindValue(noteAttributes.altitude);
		q.addBindValue(QString::fromStdString(noteAttributes.author));
		q.addBindValue(QString::fromStdString(noteAttributes.source));
		q.addBindValue(QString::fromStdString(noteAttributes.sourceURL));
		q.addBindValue(QString::fromStdString(noteAttributes.sourceApplication));
		QItem::executeQuery(q);
	}

	void update(QSqlDatabase* db, Guid guid, NoteAttributes& noteAttributes) {
		QSqlQuery q(*db);

		if(!QItem::isExists(db, "noteattributes", "noteid", QString::fromStdString(guid)))
		{
			store(db, guid, noteAttributes);
			return;
		}

		q.prepare(UPDATE_NOTE_ATTRIB);
		q.addBindValue(noteAttributes.subjectDate);
		q.addBindValue(noteAttributes.latitude);
		q.addBindValue(noteAttributes.longitude);
		q.addBindValue(noteAttributes.altitude);
		q.addBindValue(QString::fromStdString(noteAttributes.author));
		q.addBindValue(QString::fromStdString(noteAttributes.source));
		q.addBindValue(QString::fromStdString(noteAttributes.sourceURL));
		q.addBindValue(QString::fromStdString(noteAttributes.sourceApplication));
		q.addBindValue(QString::fromStdString(guid));

		QItem::executeQuery(q);
	}

	void remove(QSqlDatabase* db, Guid guid) {

	}
}

#endif /* QNOTEATTRIBUTES_HPP_ */
