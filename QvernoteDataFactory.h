/*
 * QvernoteDataFactory.h
 *
 *  Created on: Jun 14, 2010
 *      Author: alexr
 */

#ifndef QVERNOTEDATAFACTORY_H_
#define QVERNOTEDATAFACTORY_H_

#include <QtSql>
#include <NoteStore.h>

using namespace evernote::edam;
using namespace std;

namespace QvernoteDataFactory {
	bool createTag(QSqlRecord tagRecord, Tag& newTag);
	bool createSavedSearch(QSqlRecord savedSearchRecord, SavedSearch& newSavedSearch);
	bool createNotebook(QSqlRecord notebookRecord, Notebook& newNotebook);
	bool createNote(QSqlRecord noteRecord, NoteAttributes noteAttributes, QVector<Resource> resources, QVector<Tag> tagList, Note& newNote);
	bool createResource(QSqlRecord resourceRecord, ResourceAttributes resourceAttributes, Data data, Data recoData, Data altData, Resource& newResource);
	void createNoteAttributes(QSqlRecord r, NoteAttributes& newAttrib);
	void createData(QSqlRecord r, Data& data);
	void createResourceAttributes(QSqlRecord r, ResourceAttributes& newAttrib);
};

#endif /* QVERNOTEDATAFACTORY_H_ */
