/*
 * QvernoteItem.hpp
 *
 *  Created on: Jun 14, 2010
 *      Author: alex
 */

#ifndef QITEM_HPP_
#define QITEM_HPP_

#include "QDataTypes.hpp"

#define UPDATE_USN(table) \
	"update "+table+" set updateSequenceNum=? where guid=?"

#define UPDATE_DIRTY(table) \
	"update "+table+" set dirty=? where ?=?"

namespace QItem {
	bool executeQuery(QSqlQuery& query) {

		if(query.exec() == false)
		{
			qDebug() << __FUNCTION__ << query.lastQuery();
					if(query.boundValues().size() > 0)
						qDebug() << query.boundValues();
			qDebug() << __FUNCTION__ << "Error in query" << query.lastError().text();
			//assert(false);
			return false;
		}
		return true;
	}

	bool isExists(QSqlDatabase* db, QString table, QString column, QString ref)	{
		QSqlQuery q(*db);

		q.prepare("select " + column + " from " + table + " where " + column + " = '" + ref + "'");
		executeQuery(q);
		q.next();
		//qDebug() << __FUNCTION__ << "Rows:" << q.numRowsAffected() << "Size:" << q.size() << "Record value:" << q.record().value(column).isValid();
		return (q.record().value(column).isValid());
	}

	bool isExists(QSqlDatabase* db, QString table, QString column, QString ref, QSqlRecord& r) {
		QSqlQuery q(*db);

		q.prepare("select * from " + table + " where " + column + " = '" + ref + "'");
		executeQuery(q);
		q.next();
		r = q.record();
		//qDebug() << __FUNCTION__ << "Rows:" << q.numRowsAffected() << "Size:" << q.size() << "Record value:" << q.record().value(column).isValid();
		return (r.value(0).isValid());
	}

	bool isExists(QSqlDatabase* db, QString table, QString column, QString ref, QString condition) {
		QSqlQuery q(*db);

		q.prepare("select " + column + " from " + table + " where " + column + " = '" + ref + "' and "+condition);
		executeQuery(q);
		q.next();
		return (q.record().value(column).isValid());
	}

	bool isDirty(QSqlRecord& r) {
		return (r.value("dirty").toInt() == 1);
	}

	QSqlRecord load(QSqlDatabase* db, QString table, QString column, QString ref) {
		QSqlQuery q(*db);
		QString qs = "select * from " + table + " where " + column + " = \"" + ref + "\"";
		//qDebug() << __FUNCTION__ << qs;
		q.prepare(qs);
		executeQuery(q);
		q.next();

		return (q.record().value(column).isNull()) ? QSqlRecord() : q.record();
	}

	QSqlRecord load(QSqlDatabase* db, QString table, QString column, QString ref, QString condition) {
		QSqlQuery q(*db);
		QString qs = "select * from " + table + " where " + column + " = '" + ref + "' and "+condition;
		//qDebug() << __FUNCTION__ << qs;
		q.prepare(qs);
		executeQuery(q);
		q.next();

		return (q.record().value(column).isNull()) ? QSqlRecord() : q.record();
	}

	void loadList(QString table, QSqlQuery& q) {
		q.prepare(LOAD_ITEM_LIST(table));
		executeQuery(q);
	}

	int getCount(QSqlDatabase* db, QString table, QString column, QString ref) {
		QSqlQuery q(*db);

		q.prepare("select count(?) from ? where ?=?");
		q.addBindValue(column);
		q.addBindValue(table);
		q.addBindValue(column);
		q.addBindValue(ref);
		executeQuery(q);

		q.next();
		return q.value(0).toInt();
	}

	std::string resolveConflict(QSqlDatabase* db, QSqlRecord& r, QString id, QString deleteQuery, std::string newName) {
		QSqlQuery q(*db);

		if(isDirty(r))
		{
			// We have a conflict! I decide that in case of a conflict, server wins!!!
			q.prepare(deleteQuery);
			q.addBindValue(id);
			executeQuery(q);
			return newName;
		}
		else
		{
			// Tag exists but with different guid.
			// We will insert new tag with different name
			return newName + QString(" (" + r.value("guid").toString().left(4) + ")").toStdString();
		}
	}

	int	 getDirtyItems(QString table, QSqlQuery& query) {
		query.prepare("select guid from " + table + " where dirty=1");
		executeQuery(query);

		return query.size();
	}

	int getNewItems(QString table, QSqlQuery& query) {
		query.prepare("select guid from " + table + " where updateSequenceNum=0");
		executeQuery(query);

		return query.size();
	}

	int getNewItems(QString table, QSqlQuery& query, QString condition) {
		query.prepare("select guid from " + table + " where updateSequenceNum=0 and "+condition);
		executeQuery(query);

		return query.size();
	}

	void setDirtyFlag(QSqlDatabase* db, QString table, QString column, QString ref, int flag) {
		QSqlQuery q(*db);

		/*q.prepare(UPDATE_DIRTY(table));

		q.addBindValue(flag);
		q.addBindValue(column);
		q.addBindValue(ref);
		*/
		q.prepare("update "+table+" set dirty="+QString::number(flag) + " where "+column+"='"+ref+"'");
		executeQuery(q);
	}

	void remove(QSqlDatabase* db, QString table, QString column, QString ref) {
		QSqlQuery q(*db);

		q.prepare(DELETE_ITEM(table, column, ref));
		executeQuery(q);
	}

	void setUSN(QSqlDatabase* db, QString table, int usn, QString guid) {
		QSqlQuery q(*db);

		q.prepare(UPDATE_USN(table));
		q.addBindValue(usn);
		q.addBindValue(guid);
		executeQuery(q);
	}

	void setDeleted(QSqlDatabase* db, QString table, QString guid) {
		QSqlQuery q(*db);
		qint64 timestamp = QDateTime::currentDateTime().toTime_t() * 1000;

		q.prepare("update "+table+" set deleted=" + QString::number(timestamp) + " where guid='"+guid+"'");
		executeQuery(q);
	}

	void getDeleted(QString table, QSqlQuery& query) {
		query.prepare("select guid from "+table+" where deleted != 0");
		executeQuery(query);
	}
};
#endif /* QITEM_HPP_ */
