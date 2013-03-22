/*
 * QvernoteNotebook.hpp
 *
 *  Created on: Jun 14, 2010
 *      Author: alex
 */

#ifndef QNOTEBOOK_HPP_
#define QNOTEBOOK_HPP_

#include <QtSql>
#include "QDataTypes.hpp"
#include "../QvernoteDataFactory.h"
//#include "../QvernoteDBSchema.h"

/* Notebook */
//#define INSERT_NOTEBOOK "insert into notebooks (guid, name, updateSequenceNum, defaultNotebook, serviceCreated, serviceUpdated, published) " \
	"values (:guid, :name, :updateSequenceNum, :defaultNotebook, :serviceCreated, :serviceUpdated, :published)"

#define INSERT_NOTEBOOK(guid, name, updateSequenceNum, defaultNotebook, serviceCreated, serviceUpdated, published) \
	"insert into notebooks (guid, name, updateSequenceNum, defaultNotebook, serviceCreated, serviceUpdated, published) " \
	"values ('"+QString::fromStdString(guid)+"', \""+QString::fromUtf8(name.c_str())+"\", "+QString::number(updateSequenceNum)+", "+QString::number(defaultNotebook)+", "+QString::number(serviceCreated)+", "+QString::number(serviceUpdated)+", "+QString::number(published)+")"

#define UPDATE_NOTEBOOK(guid, name, updateSequenceNum, defaultNotebook, serviceCreated, serviceUpdated, published) \
	"update notebooks set " \
	"name=\""+QString::fromUtf8(name.c_str())+"\", updateSequenceNum="+QString::number(updateSequenceNum)+", defaultNotebook="+QString::number(defaultNotebook)+", serviceCreated="+QString::number(serviceCreated)+", serviceUpdated="+QString::number(serviceUpdated)+", published="+QString::number(published)+" " \
	"where guid='"+QString::fromStdString(guid)+"'"

#define UPDATE_SERVER_DATA \
	"update notebooks set guid=?, updateSequenceNum=?, serviceCreated=?, serviceUpdated=? where name=?"

#define LOAD_NOTEBOOK(k, v) "select * from notebooks where " + QString(k) + "=\"" + QString::fromStdString(v)+"\""

namespace QNotebook {
	void load(QSqlDatabase* db, Guid guid, Notebook& notebook) {
		QvernoteDataFactory::createNotebook(
				QItem::load(db, "notebooks", "guid", QString::fromStdString(guid)),
				notebook);
	}

	void store(QSqlDatabase* db, Notebook& notebook) {
		QSqlQuery q(*db);

		q.prepare(INSERT_NOTEBOOK(notebook.guid, notebook.name, notebook.updateSequenceNum, notebook.defaultNotebook, notebook.serviceCreated, notebook.serviceUpdated, notebook.published));
		QItem::executeQuery(q);
	}

	void update(QSqlDatabase* db, Notebook& notebook) {
		QSqlQuery q(*db);
		QSqlRecord r;

		if(!QItem::isExists(db, "notebooks", "guid", QString::fromStdString(notebook.guid), r))
		{
			r = QItem::load(db, "notebooks", "name", QString::fromUtf8(notebook.name.c_str()));
			if(!r.isEmpty())
			{
				notebook.name = QItem::resolveConflict(db, r, QString::fromStdString(notebook.guid), DELETE_ITEM("notebooks", "guid", QString::fromStdString(notebook.guid)), notebook.name);
			}

			q.prepare(INSERT_NOTEBOOK(notebook.guid, notebook.name, notebook.updateSequenceNum, notebook.defaultNotebook, notebook.serviceCreated, notebook.serviceUpdated, notebook.published));
		}
		else if(r.value("updateSequenceNum").toInt() < notebook.updateSequenceNum)
		{
			q.prepare(UPDATE_NOTEBOOK(notebook.guid, notebook.name, notebook.updateSequenceNum, notebook.defaultNotebook, notebook.serviceCreated, notebook.serviceUpdated, notebook.published));
		}
		else
			return;

		QItem::executeQuery(q);
	}

	void updateServerData(QSqlDatabase* db, Notebook& notebook) {
		QSqlQuery q(*db);

		q.prepare(UPDATE_SERVER_DATA);
		q.addBindValue(QString::fromStdString(notebook.guid));
		q.addBindValue(notebook.updateSequenceNum);
		q.addBindValue(notebook.serviceCreated);
		q.addBindValue(notebook.serviceUpdated);
		q.addBindValue(QString::fromStdString(notebook.name));
		QItem::executeQuery(q);
	}

	void remove(QSqlDatabase* db, Guid guid) {
		QItem::remove(db, "notebooks", "guid", QString::fromStdString(guid));
	}

	void loadList(QSqlDatabase* db, QVector<Notebook>& notebooklist) {
		QSqlQuery q(*db);
		Notebook notebook;

		QItem::loadList("notebooks", q);
		while(q.next()) {
			QvernoteDataFactory::createNotebook(q.record(), notebook);
			notebooklist.push_back(notebook);
		}
	}

	void getDirty(QSqlDatabase* db, QVector<Notebook>& notebooklist) {
		QSqlQuery q(*db);

		QItem::getDirtyItems("notebooks", q);
		while(q.next()) {
			Notebook notebook;
			load(db, q.value(0).toString().toStdString(), notebook);
			notebooklist.push_back(notebook);
		}
	}

	void getNew(QSqlDatabase* db, QVector<Notebook>& notebooklist) {
		QSqlQuery q(*db);

		QItem::getNewItems("notebooks", q);
		while(q.next()) {
			Notebook notebook;
			load(db, q.value(0).toString().toStdString(), notebook);
			notebooklist.push_back(notebook);
		}
	}

	void getDeleted(QSqlDatabase* db, QVector<Notebook>& notebooklist) {
		QSqlQuery q(*db);

		QItem::getDeleted("notebooks", q);
		while(q.next()) {
			Notebook notebook;
			load(db, q.value(0).toString().toStdString(), notebook);
			notebooklist.push_back(notebook);
		}
	}

	void clear(QSqlDatabase* db) {
		QSqlQuery q(*db);

		q.exec("delete from notebooks");
	}
}
#endif /* QNOTEBOOK_HPP_ */
