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
#include "QvernoteDataFactory.h"

namespace QvernoteDataFactory {

#define SETVALUE(obj, field, val) \
	obj.field = val; \
	obj.__isset.field = true;

bool createTag(QSqlRecord tagRecord, Tag& newTag)
{
	SETVALUE(newTag, guid, tagRecord.value("guid").toString().toStdString());
	SETVALUE(newTag, name, tagRecord.value("name").toString().toStdString());
	/*SETVALUE(newTag, parentGuid, tagRecord.value("parengGuid").toString().toStdString());*/
	SETVALUE(newTag, updateSequenceNum, tagRecord.value("updateSequenceNum").toInt());

	return true;
}

bool createSavedSearch(QSqlRecord savedSearchRecord, SavedSearch& newSavedSearch)
{
	return true;
}

bool createNotebook(QSqlRecord notebookRecord, Notebook& newNotebook)
{
	SETVALUE(newNotebook, guid, notebookRecord.value("guid").toString().toStdString());
	SETVALUE(newNotebook, name, notebookRecord.value("name").toString().toStdString());
	SETVALUE(newNotebook, updateSequenceNum, notebookRecord.value("updateSequenceNum").toInt());
	SETVALUE(newNotebook, defaultNotebook, notebookRecord.value("defaultNotebook").toBool());
	SETVALUE(newNotebook, serviceCreated, notebookRecord.value("serviceCreated").toLongLong());
	SETVALUE(newNotebook, serviceUpdated, notebookRecord.value("serviceUpdated").toLongLong());
	SETVALUE(newNotebook, published, notebookRecord.value("published").toBool());

	return true;
}

bool createNote(QSqlRecord noteRecord, NoteAttributes noteAttributes, QVector<Resource> resources, QVector<Tag> tagList, Note& newNote)
{
	SETVALUE(newNote, guid, noteRecord.value("guid").toString().toStdString());
	SETVALUE(newNote, title, QString(noteRecord.value("title").toString().toUtf8()).toStdString());
	SETVALUE(newNote, content, QString(noteRecord.value("content").toString().toUtf8()).toStdString());
	SETVALUE(newNote, contentHash, noteRecord.value("contentHash").toString().toStdString());
	SETVALUE(newNote, contentLength, noteRecord.value("contentLength").toInt());
	SETVALUE(newNote, created, noteRecord.value("created").toLongLong());
	SETVALUE(newNote, updated, noteRecord.value("updated").toLongLong());
	SETVALUE(newNote, deleted, noteRecord.value("deleted").toLongLong());
	SETVALUE(newNote, active, noteRecord.value("active").toBool());
	SETVALUE(newNote, updateSequenceNum, noteRecord.value("updateSequenceNum").toInt());
	SETVALUE(newNote, notebookGuid, noteRecord.value("notebookGuid").toString().toStdString());

	newNote.__isset.tagGuids = true;
	newNote.__isset.tagNames = true;

	newNote.tagGuids.clear();
	newNote.tagNames.clear();

	for(QVector<Tag>::iterator i = tagList.begin(); i != tagList.end(); i++)
	{
		newNote.tagGuids.push_back((*i).guid);
		newNote.tagNames.push_back((*i).name);
	}

	newNote.__isset.resources = true;
	newNote.resources.clear();

	for(QVector<Resource>::iterator i = resources.begin(); i != resources.end(); i++)
	{
		newNote.resources.push_back(*i);
	}

	SETVALUE(newNote, attributes, noteAttributes);

	return true;
}

bool createResource(QSqlRecord resourceRecord, ResourceAttributes resourceAttributes, Data data, Data recoData, Data altData, Resource& newResource)
{
	SETVALUE(newResource, guid, resourceRecord.value("guid").toString().toStdString());
	SETVALUE(newResource, noteGuid, resourceRecord.value("noteGuid").toString().toStdString());
	SETVALUE(newResource, mime, resourceRecord.value("mime").toString().toStdString());
	SETVALUE(newResource, width, resourceRecord.value("width").toInt());
	SETVALUE(newResource, height, resourceRecord.value("height").toInt());
	SETVALUE(newResource, duration, resourceRecord.value("duration").toInt());
	SETVALUE(newResource, active, resourceRecord.value("active").toBool());
	SETVALUE(newResource, updateSequenceNum, resourceRecord.value("updateSequenceNum").toInt());

	SETVALUE(newResource, data, data);
	SETVALUE(newResource, recognition, recoData);
	SETVALUE(newResource, alternateData, altData);

	SETVALUE(newResource, attributes, resourceAttributes);
	return true;
}

void createNoteAttributes(QSqlRecord r, NoteAttributes& newAttrib)
{
	SETVALUE(newAttrib, subjectDate, r.value("subjectDate").toInt());
	SETVALUE(newAttrib, latitude, r.value("latitude").toReal());
	SETVALUE(newAttrib, longitude, r.value("longitude").toReal());
	SETVALUE(newAttrib, altitude, r.value("altitude").toReal());
	SETVALUE(newAttrib, author, r.value("author").toString().toStdString());
	SETVALUE(newAttrib, source, r.value("source").toString().toStdString());
	SETVALUE(newAttrib, sourceURL, r.value("sourceURL").toString().toStdString());
	SETVALUE(newAttrib, sourceApplication, r.value("sourceApplication").toString().toStdString());
}

void createData(QSqlRecord r, Data& data)
{
	if(r.value("body").isValid())
	{
		QByteArray d = r.value("body").toByteArray();
		string initialDataString((const char*)d, d.size());
		SETVALUE(data, body, string(initialDataString.begin(), initialDataString.begin() + d.size()));
	}

	QByteArray h = r.value("bodyHash").toByteArray();
	string initialBodyHashString((const char*)h, h.size());
	SETVALUE(data, bodyHash, string(initialBodyHashString.begin(), initialBodyHashString.begin() + h.size()));

	SETVALUE(data, size, r.value("size").toInt());
}

void createResourceAttributes(QSqlRecord r, ResourceAttributes& newAttrib)
{
	SETVALUE(newAttrib, sourceURL, r.value("sourceURL").toString().toStdString());
	SETVALUE(newAttrib, timestamp, r.value("timestamp").toInt());
	SETVALUE(newAttrib, latitude, r.value("latitude").toReal());
	SETVALUE(newAttrib, longitude, r.value("longitude").toReal());
	SETVALUE(newAttrib, altitude, r.value("altitude").toReal());
	SETVALUE(newAttrib, cameraMake, r.value("cameraMake").toString().toStdString());
	SETVALUE(newAttrib, cameraModel, r.value("cameraModel").toString().toStdString());
	SETVALUE(newAttrib, clientWillIndex, r.value("clientWillIndex").toBool());
	SETVALUE(newAttrib, fileName, r.value("fileName").toString().toStdString());
	SETVALUE(newAttrib, attachment, r.value("attachment").toBool());
}

}
