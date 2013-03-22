/*
 * QFavorite.cpp
 *
 *  Created on: Jul 24, 2010
 *      Author: alex
 */

#include <QtSql>

#include "QDataTypes.hpp"

#define INSERT_FAVORITE \
	"insert into favorites (noteid) values (?)"

#define REMOVE_FAVORITE \
	"delete from favorites where noteid=?"

#define LOAD_FAVORITES \
	"select * from favorites"

#define LOAD_FAVORITE \
	"select * from favorite where noteid=?"

namespace QFavorite {
	void insert(QSqlDatabase* db, Guid guid) {
		QSqlQuery q(*db);

		q.prepare(INSERT_FAVORITE);
		q.addBindValue(QString::fromStdString(guid));
		QItem::executeQuery(q);
	}

	void remove(QSqlDatabase* db, Guid guid) {
		QSqlQuery q(*db);

		q.prepare(REMOVE_FAVORITE);
		q.addBindValue(QString::fromStdString(guid));
		QItem::executeQuery(q);
	}

	void load(QSqlDatabase* db, QVector<Guid>& guidList) {
		QSqlQuery q(*db);

		q.prepare(LOAD_FAVORITES);
		if(QItem::executeQuery(q)) {
			while(q.next()) {
				guidList.push_back(q.value(0).toString().toStdString());
			}
		}
	}

	bool isFavorite(QSqlDatabase* db, Guid guid) {
		QSqlQuery q(*db);

		return QItem::isExists(db, "favorites", "noteid", QString::fromStdString(guid));
	}
}
