/***********************************************************************
Copyright (C) 2010 Alex R <burbulator@gmail.com>

This file is part of Qvernote.

Qvernote is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Qvernote is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Qvernote.  If not, see <http://www.gnu.org/licenses/>.

***********************************************************************/
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
