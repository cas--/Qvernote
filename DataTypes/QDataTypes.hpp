/*
 * QDataTypes.hpp
 *
 *  Created on: Jun 16, 2010
 *      Author: alexr
 */

#ifndef QDATATYPES_HPP_
#define QDATATYPES_HPP_

#include <QtSql>
#include <NoteStore.h>
#include "../QvernoteDBSchema.h"

using namespace evernote::edam;
using namespace std;

/* GLOBAL */
#define DELETE_ITEM(table, column, ref)  "delete from "+QString(table)+" where "+QString(column)+"='"+QString(ref)+"'"
#define LOAD_ITEM(table, k, v) "select * from "+QString(table)+" where "+QString(k)+" = \""+QString::fromStdString(v)+"\""
#define LOAD_ITEM_LIST(table) "select * from "+QString(table)+" where deleted = 0"

namespace QItem {
	bool 		executeQuery(QSqlQuery& query);
	bool 		isExists(QSqlDatabase* db, QString table, QString column, QString ref);
    bool 		isExists(QSqlDatabase* db, QString table, QString column, QString ref, QSqlRecord& r);
    bool 		isExists(QSqlDatabase* db, QString table, QString column, QString ref, QString condition);
    bool 		isDirty(QSqlRecord& r);
    QSqlRecord 	load(QSqlDatabase* db, QString table, QString column, QString ref);
    QSqlRecord 	load(QSqlDatabase* db, QString table, QString column, QString ref, QString condition);
    void 		loadList(QString table, QSqlQuery& q);
    std::string resolveConflict(QSqlDatabase* db, QSqlRecord& r, QString id, QString deleteQuery, std::string newName);
    int      	getDirtyItems(QString table, QSqlQuery& query);
    int 		getNewItems(QString table, QSqlQuery& query);
    int 		getNewItems(QString table, QSqlQuery& query, QString condition);
    void 		setDirtyFlag(QSqlDatabase* db, QString table, QString column, QString ref, int flag);
    void 		remove(QSqlDatabase* db, QString table, QString column, QString ref);
    void 		setUSN(QSqlDatabase* db, QString table, int usn, QString guid);
    void 		setDeleted(QSqlDatabase* db, QString table, QString guid);
    void 		getDeleted(QString table, QSqlQuery& query);
}

namespace QTag {
	void load(QSqlDatabase* db, Guid guid, Tag& tag);
    void store(QSqlDatabase* db, Tag& tag);
    void update(QSqlDatabase* db, Tag& tag);
    void updateServerData(QSqlDatabase* db, Tag& tag);
    void remove(QSqlDatabase* db, Guid guid);
    void loadList(QSqlDatabase* db, QVector<Tag>& taglist);
    void loadForNote(QSqlDatabase* db, Guid noteGuid, QVector<Tag>& taglist);
    void loadForNotebook(QSqlDatabase* db, Guid notebookGuid, QVector<Tag>& taglist);
    void getDirty(QSqlDatabase* db, QVector<Tag>& taglist);
    void getNew(QSqlDatabase* db, QVector<Tag>& taglist);
    void loadCount(QSqlDatabase* db, map<Guid, int>& tagCounts);
}

namespace QNotebook {
	void load(QSqlDatabase* db, Guid guid, Notebook& notebook);
    void store(QSqlDatabase* db, Notebook& notebook);
    void update(QSqlDatabase* db, Notebook& notebook);
    void updateServerData(QSqlDatabase* db, Notebook& notebook);
    void remove(QSqlDatabase* db, Guid guid);
    void loadList(QSqlDatabase* db, QVector<Notebook>& notebooklist);
    void getDirty(QSqlDatabase* db, QVector<Notebook>& notebooklist);
    void getNew(QSqlDatabase* db, QVector<Notebook>& notebooklist);
    void getDeleted(QSqlDatabase* db, QVector<Notebook>& notebooklist);
    void clear(QSqlDatabase* db);
}

namespace QNote {
	void load(QSqlDatabase* db, Guid guid, Note& note);
	void load_shallow(QSqlDatabase* db, Guid guid, Note& note);
	void load_content(QSqlDatabase* db, Note& note);
	void loadResourceData(QSqlDatabase* db, Note& note);
	void store(QSqlDatabase* db, Note& note);
	void update(QSqlDatabase* db, Note& note);
	void updateServerData(QSqlDatabase* db, Note& note);
	void remove(QSqlDatabase* db, Guid guid);
	void loadList(QSqlDatabase* db, Guid notebookGuid, QVector<Note>& notelist);
	void loadForNotebook(QSqlDatabase* db, Guid notebookGuid, QVector<Note>& notelist);
	void loadCount(QSqlDatabase* db, map<Guid, int>& notebookCounts);
	void search(QSqlDatabase* db, QStringList words, QString notebookGuid, QVector<Guid> tagGuids, vector<Note>& noteList, int maxNoteCount);
	void getDirty(QSqlDatabase* db, QVector<Note>& notelist);
	void getNew(QSqlDatabase* db, QVector<Note>& notelist);
	void getDeleted(QSqlDatabase* db, QVector<Note>& notelist);
	void getTrash(QSqlDatabase* db, QVector<Note>& notelist);
	void addToTrash(QSqlDatabase* db, Guid noteGuid);
	void getFavorites(QSqlDatabase* db, QVector<Note>& notelist);
}

namespace QNoteResource {
	void load(QSqlDatabase* db, Guid guid, Resource& resource);
	void load_shallow(QSqlDatabase* db, Guid guid, Resource& resource);
	void loadData(QSqlDatabase* db, Resource& resource);
    void store(QSqlDatabase* db, Resource& resource);
    void update(QSqlDatabase* db, Resource& resource);
    void remove(QSqlDatabase* db, Guid guid);
    void loadForNote(QSqlDatabase* db, Guid noteGuid, QVector<Resource>& resourcelist);
}

namespace QNoteAttributes {
	void load(QSqlDatabase* db, Guid guid, NoteAttributes& noteAttributes);
	void store(QSqlDatabase* db, Guid guid, NoteAttributes& noteAttributes);
	void update(QSqlDatabase* db, Guid guid, NoteAttributes& noteAttributes);
	void remove(QSqlDatabase* db, Guid guid);
}

namespace QData {
	void load(QSqlDatabase* db, Guid guid, ResourceDataType dataType, Data& data);
	void load_shallow(QSqlDatabase* db, Guid guid, ResourceDataType dataType, Data& data);
    void store(QSqlDatabase* db, Guid resourceid, ResourceDataType dataType, Data& data);
    void update(QSqlDatabase* db, Guid resourceid, ResourceDataType dataType, Data& data);
}

namespace QResourceAttributes {
	void load(QSqlDatabase* db, Guid guid, ResourceAttributes& resourceAttrib);
    void store(QSqlDatabase* db, Guid guid, ResourceAttributes& resourceAttrib);
    void update(QSqlDatabase* db, Guid guid, ResourceAttributes& resourceAttrib);
}

namespace QNoteThumbnail {
	void load(QSqlDatabase* db, Guid noteGuid, QByteArray& thumbnail);
	void store(QSqlDatabase* db, Note& note, QByteArray& thumbnail);
	void remove(QSqlDatabase* db, Guid noteGuid);
	void clear(QSqlDatabase* db);
	void loadForNotebook(QSqlDatabase* db, Guid notebookGuid, QMap<QString, QByteArray>& data);
}

namespace QFavorite {
	void insert(QSqlDatabase* db, Guid guid);
	void remove(QSqlDatabase* db, Guid guid);
	void load(QSqlDatabase* db, QVector<Guid>& guidList);
	bool isFavorite(QSqlDatabase* db, Guid guid);
}

#endif /* QDATATYPES_HPP_ */
