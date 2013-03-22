/*
 * QvernoteStorageQueries.h
 *
 *  Created on: Jun 13, 2010
 *      Author: alexr
 */

#ifndef QVERNOTESTORAGEQUERIES_H_
#define QVERNOTESTORAGEQUERIES_H_

#define PREPARE_NOTEBOOK_SCRIPT	\
	"create table notebooks ( " \
		"dirty integer default 0, " \
		"deleted integer default 0," \
		"guid varchar(38) unique primary key not null, " \
		"name TEXT, " \
		"updateSequenceNum integer default 0, "\
		"defaultNotebook smallint, " \
		"serviceCreated integer, " \
		"serviceUpdated integer, " \
		"published integer )"

#define PREPARE_NOTEBOOK_INDEX_SCRIPT \
	"create index notebookidx on notebooks (guid)"

#define PREPARE_NOTE_SCRIPT \
	"create table notes ( " \
			"dirty integer default 0, " \
			"guid varchar(38) unique primary key not null, " \
			"title TEXT, " \
			"content CBLOB, " \
			"contentHash varchar(64), " \
			"contentLength integer, " \
			"created integer, " \
			"updated integer, " \
			"deleted integer default 0, " \
			"active integer, " \
			"updateSequenceNum integer default 0, " \
			"notebookGuid varchar(38), " \
			"foreign key (notebookGuid) references notebooks(guid) ON DELETE CASCADE ON UPDATE CASCADE)"

#define PREPARE_NOTE_INDEX_SCRIPT \
	"create index noteidx on notes (notebookGuid, guid)"

#define PREPARE_TAG2NOTE_SCRIPT \
	"create table notetags ( " \
			"tagid varchar(38), " \
			"noteid varchar(38), " \
			"foreign key (tagid) references tags(guid) ON DELETE CASCADE ON UPDATE CASCADE, " \
			"foreign key (noteid) references notes(guid) ON DELETE CASCADE ON UPDATE CASCADE)"

#define PREPARE_NOTE_ATTRIB_SCRIPT \
	"create table noteattributes ( " \
			"noteid varchar(38), " \
			"subjectDate integer, " \
			"latitude real, " \
			"longitude real, " \
			"altitude real, " \
			"author varchar(255), " \
			"source varchar(255), " \
			"sourceURL varchar(255), " \
			"sourceApplication varchar(255)," \
			"foreign key (noteid) references notes(guid) ON DELETE CASCADE ON UPDATE CASCADE)"

#define PREPARE_NOTE_ATTRIB_INDEX_SCRIPT \
	"create index noteattributeindex on noteattributes (noteid)"

#define PREPARE_TAG_SCRIPT \
	"create table tags ( " \
			"dirty integer default 0, " \
			"deleted integer default 0," \
			"guid varchar(38) unique primary key not null, " \
			"name varchar(255), " \
			"parentGuid varchar(38), " \
			"updateSequenceNum integer default 0 )"

#define PREPARE_SAVEDSEARCH_SCRIPT \
	"create table savedsearches ( " \
			"dirty integer default 0, " \
			"guid varchar(38) primary key not null, " \
			"name varchar(255), " \
			"query varchar(255), " \
			"format integer, " \
			"updateSequenceNum integer default 0 )"

#define PREPARE_RESOURCE_SCRIPT \
	"create table resources ( " \
			"dirty integer default 0, " \
			"guid varchar(38) unique primary key not null, " \
			"noteGuid varchar(38), " \
			"mime varchar(255), " \
			"width integer, " \
			"height integer, " \
			"duration integer, " \
			"active integer , " \
			"updateSequenceNum integer default 0, " \
			"foreign key (noteGuid) references notes(guid) ON DELETE CASCADE ON UPDATE CASCADE)"
			//"foreign key (active) references notes(active) ON UPDATE CASCADE)"

#define PREPARE_RESOURCE_INDEX_SCRIPT \
	"create index resourceidx on resources (guid, noteGuid)"

/*
 * Data can be of types:
 *
 * Resource = 0
 * Recognition = 1
 * Alternate = 2
 */

typedef enum eResourceDataType {
	DT_Resource = 0,
	DT_Recognition = 1,
	DT_Alternate = 2
} ResourceDataType;

#define PREPARE_DATATAB_SCRIPT \
	"create table data ( " \
			"resourceid varchar(38), " \
			"type integer, "		\
			"bodyHash varchar(255), " \
			"size integer, " \
			"body BLOB, " \
			"foreign key (resourceid) references resources(guid) ON DELETE CASCADE ON UPDATE CASCADE)"

#define PREPARE_DATATAB_INDEX_SCRIPT \
	"create index dataidx on data (resourceid, type)"

#define PREPARE_RESOURCE_ATTRIB_SCRIPT \
	"create table resourceattrib ( " \
			"resourceid varchar(38), " \
			"sourceURL varchar(255), " \
			"timestamp integer, " \
			"latitude real, " \
			"longitude real, " \
			"altitude real, " \
			"cameraMake varchar(255), " \
			"cameraModel varchar(255), " \
			"clientWillIndex integer, " \
			"fileName varchar(255), " \
			"attachment integer, "\
			"foreign key (resourceid) references resources(guid) ON DELETE CASCADE ON UPDATE CASCADE)"

#define PREPARE_RESOURCE_ATTRIB_INDEX_SCRIPT \
	"create index resourceattribidx on resourceattrib (resourceid)"

#define PREPARE_NOTE_THUMBNAIL_SCRIPT \
	"create table thumbnails ( " \
			"noteid varchar(38) unique primary key not null, " \
			"notebookid varchar(38)," \
			"thumbnail BLOB," \
			"foreign key (noteid) references notes(guid) ON DELETE CASCADE ON UPDATE CASCADE)"

#define PREPARE_NOTE_THUMBNAIL_INDEX_SCRIPT \
	"create index thumbnailidx on thumbnails (noteid, notebookid)"

#define PREPARE_FAVORITES_SCRIPT \
	"create table favorites ( " \
			"noteid varchar(38) unique primary key not null, " \
			"foreign key (noteid) references notes(guid) ON DELETE CASCADE ON UPDATE CASCADE)"

#define PREPARE_FAVORITES_INDEX_SCRIPT \
	"create index favoriteidx on favorites (noteid)"

static QString PREPARE_FOREIGN_KEY_SCRIPT[40] = {
		"CREATE TRIGGER genfkey6_insert_referencing BEFORE INSERT ON \"data\" WHEN "\
		"    new.\"resourceid\" IS NOT NULL AND NOT EXISTS (SELECT 1 FROM \"resources\" WHERE new.\"resourceid\" == \"guid\")"\
		"BEGIN"\
		"  SELECT RAISE(ABORT, 'constraint failed');"\
		"END;",
		"CREATE TRIGGER genfkey6_update_referencing BEFORE"\
		"    UPDATE OF \"resourceid\" ON \"data\" WHEN "\
		"    new.\"resourceid\" IS NOT NULL AND "\
		"    NOT EXISTS (SELECT 1 FROM \"resources\" WHERE new.\"resourceid\" == \"guid\")"\
		"BEGIN"\
		"  SELECT RAISE(ABORT, 'constraint failed');"\
		"END;",
		"CREATE TRIGGER genfkey6_delete_referenced BEFORE DELETE ON \"resources\" WHEN"\
		"    EXISTS (SELECT 1 FROM \"data\" WHERE old.\"guid\" == \"resourceid\")"\
		"BEGIN"\
		"  DELETE FROM \"data\" WHERE \"resourceid\" = old.\"guid\";"\
		"END;",
		"CREATE TRIGGER genfkey6_update_referenced AFTER"\
		"    UPDATE OF \"guid\" ON \"resources\" WHEN "\
		"    EXISTS (SELECT 1 FROM \"data\" WHERE old.\"guid\" == \"resourceid\")"\
		"BEGIN"\
		"  UPDATE \"data\" SET \"resourceid\" = new.\"guid\" WHERE \"resourceid\" = old.\"guid\";"\
		"END;",
		"CREATE TRIGGER genfkey9_insert_referencing BEFORE INSERT ON \"favorites\" WHEN "\
		"    new.\"noteid\" IS NOT NULL AND NOT EXISTS (SELECT 1 FROM \"notes\" WHERE new.\"noteid\" == \"guid\")"\
		"BEGIN"\
		"  SELECT RAISE(ABORT, 'constraint failed');"\
		"END;",
		"CREATE TRIGGER genfkey9_update_referencing BEFORE"\
		"    UPDATE OF \"noteid\" ON \"favorites\" WHEN "\
		"    new.\"noteid\" IS NOT NULL AND "\
		"    NOT EXISTS (SELECT 1 FROM \"notes\" WHERE new.\"noteid\" == \"guid\")"\
		"BEGIN"\
		"  SELECT RAISE(ABORT, 'constraint failed');"\
		"END;",
		"CREATE TRIGGER genfkey9_delete_referenced BEFORE DELETE ON \"notes\" WHEN"\
		"    EXISTS (SELECT 1 FROM \"favorites\" WHERE old.\"guid\" == \"noteid\")"\
		"BEGIN"\
		"  DELETE FROM \"favorites\" WHERE \"noteid\" = old.\"guid\";"\
		"END;",
		"CREATE TRIGGER genfkey9_update_referenced AFTER"\
		"    UPDATE OF \"guid\" ON \"notes\" WHEN "\
		"    EXISTS (SELECT 1 FROM \"favorites\" WHERE old.\"guid\" == \"noteid\")"\
		"BEGIN"\
		"  UPDATE \"favorites\" SET \"noteid\" = new.\"guid\" WHERE \"noteid\" = old.\"guid\";"\
		"END;",
		""\
		"CREATE TRIGGER genfkey4_insert_referencing BEFORE INSERT ON \"noteattributes\" WHEN "\
		"    new.\"noteid\" IS NOT NULL AND NOT EXISTS (SELECT 1 FROM \"notes\" WHERE new.\"noteid\" == \"guid\")"\
		"BEGIN"\
		"  SELECT RAISE(ABORT, 'constraint failed');"\
		"END;",
		"CREATE TRIGGER genfkey4_update_referencing BEFORE"\
		"    UPDATE OF \"noteid\" ON \"noteattributes\" WHEN "\
		"    new.\"noteid\" IS NOT NULL AND "\
		"    NOT EXISTS (SELECT 1 FROM \"notes\" WHERE new.\"noteid\" == \"guid\")"\
		"BEGIN"\
		"  SELECT RAISE(ABORT, 'constraint failed');"\
		"END;",
		"CREATE TRIGGER genfkey4_delete_referenced BEFORE DELETE ON \"notes\" WHEN"\
		"    EXISTS (SELECT 1 FROM \"noteattributes\" WHERE old.\"guid\" == \"noteid\")"\
		"BEGIN"\
		"  DELETE FROM \"noteattributes\" WHERE \"noteid\" = old.\"guid\";"\
		"END;",
		"CREATE TRIGGER genfkey4_update_referenced AFTER"\
		"    UPDATE OF \"guid\" ON \"notes\" WHEN "\
		"    EXISTS (SELECT 1 FROM \"noteattributes\" WHERE old.\"guid\" == \"noteid\")"\
		"BEGIN"\
		"  UPDATE \"noteattributes\" SET \"noteid\" = new.\"guid\" WHERE \"noteid\" = old.\"guid\";"\
		"END;",
		""\
		"CREATE TRIGGER genfkey1_insert_referencing BEFORE INSERT ON \"notes\" WHEN "\
		"    new.\"notebookGuid\" IS NOT NULL AND NOT EXISTS (SELECT 1 FROM \"notebooks\" WHERE new.\"notebookGuid\" == \"guid\")"\
		"BEGIN"\
		"  SELECT RAISE(ABORT, 'constraint failed');"\
		"END;",
		"CREATE TRIGGER genfkey1_update_referencing BEFORE"\
		"    UPDATE OF \"notebookGuid\" ON \"notes\" WHEN "\
		"    new.\"notebookGuid\" IS NOT NULL AND "\
		"    NOT EXISTS (SELECT 1 FROM \"notebooks\" WHERE new.\"notebookGuid\" == \"guid\")"\
		"BEGIN"\
		"  SELECT RAISE(ABORT, 'constraint failed');"\
		"END;",
		"CREATE TRIGGER genfkey1_delete_referenced BEFORE DELETE ON \"notebooks\" WHEN"\
		"    EXISTS (SELECT 1 FROM \"notes\" WHERE old.\"guid\" == \"notebookGuid\")"\
		"BEGIN"\
		"  DELETE FROM \"notes\" WHERE \"notebookGuid\" = old.\"guid\";"\
		"END;",
		"CREATE TRIGGER genfkey1_update_referenced AFTER"\
		"    UPDATE OF \"guid\" ON \"notebooks\" WHEN "\
		"    EXISTS (SELECT 1 FROM \"notes\" WHERE old.\"guid\" == \"notebookGuid\")"\
		"BEGIN"\
		"  UPDATE \"notes\" SET \"notebookGuid\" = new.\"guid\" WHERE \"notebookGuid\" = old.\"guid\";"\
		"END;",
		""\
		"CREATE TRIGGER genfkey2_insert_referencing BEFORE INSERT ON \"notetags\" WHEN "\
		"    new.\"noteid\" IS NOT NULL AND NOT EXISTS (SELECT 1 FROM \"notes\" WHERE new.\"noteid\" == \"guid\")"\
		"BEGIN"\
		"  SELECT RAISE(ABORT, 'constraint failed');"\
		"END;",
		"CREATE TRIGGER genfkey2_update_referencing BEFORE"\
		"    UPDATE OF \"noteid\" ON \"notetags\" WHEN "\
		"    new.\"noteid\" IS NOT NULL AND "\
		"    NOT EXISTS (SELECT 1 FROM \"notes\" WHERE new.\"noteid\" == \"guid\")"\
		"BEGIN"\
		"  SELECT RAISE(ABORT, 'constraint failed');"\
		"END;",
		"CREATE TRIGGER genfkey2_delete_referenced BEFORE DELETE ON \"notes\" WHEN"\
		"    EXISTS (SELECT 1 FROM \"notetags\" WHERE old.\"guid\" == \"noteid\")"\
		"BEGIN"\
		"  DELETE FROM \"notetags\" WHERE \"noteid\" = old.\"guid\";"\
		"END;",
		"CREATE TRIGGER genfkey2_update_referenced AFTER"\
		"    UPDATE OF \"guid\" ON \"notes\" WHEN "\
		"    EXISTS (SELECT 1 FROM \"notetags\" WHERE old.\"guid\" == \"noteid\")"\
		"BEGIN"\
		"  UPDATE \"notetags\" SET \"noteid\" = new.\"guid\" WHERE \"noteid\" = old.\"guid\";"\
		"END;",
		""\
		"CREATE TRIGGER genfkey3_insert_referencing BEFORE INSERT ON \"notetags\" WHEN "\
		"    new.\"tagid\" IS NOT NULL AND NOT EXISTS (SELECT 1 FROM \"tags\" WHERE new.\"tagid\" == \"guid\")"\
		"BEGIN"\
		"  SELECT RAISE(ABORT, 'constraint failed');"\
		"END;",
		"CREATE TRIGGER genfkey3_update_referencing BEFORE"\
		"    UPDATE OF \"tagid\" ON \"notetags\" WHEN "\
		"    new.\"tagid\" IS NOT NULL AND "\
		"    NOT EXISTS (SELECT 1 FROM \"tags\" WHERE new.\"tagid\" == \"guid\")"\
		"BEGIN"\
		"  SELECT RAISE(ABORT, 'constraint failed');"\
		"END;",
		"CREATE TRIGGER genfkey3_delete_referenced BEFORE DELETE ON \"tags\" WHEN"\
		"    EXISTS (SELECT 1 FROM \"notetags\" WHERE old.\"guid\" == \"tagid\")"\
		"BEGIN"\
		"  DELETE FROM \"notetags\" WHERE \"tagid\" = old.\"guid\";"\
		"END;",
		"CREATE TRIGGER genfkey3_update_referenced AFTER"\
		"    UPDATE OF \"guid\" ON \"tags\" WHEN "\
		"    EXISTS (SELECT 1 FROM \"notetags\" WHERE old.\"guid\" == \"tagid\")"\
		"BEGIN"\
		"  UPDATE \"notetags\" SET \"tagid\" = new.\"guid\" WHERE \"tagid\" = old.\"guid\";"\
		"END;",
		""\
		"CREATE TRIGGER genfkey7_insert_referencing BEFORE INSERT ON \"resourceattrib\" WHEN "\
		"    new.\"resourceid\" IS NOT NULL AND NOT EXISTS (SELECT 1 FROM \"resources\" WHERE new.\"resourceid\" == \"guid\")"\
		"BEGIN"\
		"  SELECT RAISE(ABORT, 'constraint failed');"\
		"END;",
		"CREATE TRIGGER genfkey7_update_referencing BEFORE"\
		"    UPDATE OF \"resourceid\" ON \"resourceattrib\" WHEN "\
		"    new.\"resourceid\" IS NOT NULL AND "\
		"    NOT EXISTS (SELECT 1 FROM \"resources\" WHERE new.\"resourceid\" == \"guid\")"\
		"BEGIN"\
		"  SELECT RAISE(ABORT, 'constraint failed');"\
		"END;",
		"CREATE TRIGGER genfkey7_delete_referenced BEFORE DELETE ON \"resources\" WHEN"\
		"    EXISTS (SELECT 1 FROM \"resourceattrib\" WHERE old.\"guid\" == \"resourceid\")"\
		"BEGIN"\
		"  DELETE FROM \"resourceattrib\" WHERE \"resourceid\" = old.\"guid\";"\
		"END;",
		"CREATE TRIGGER genfkey7_update_referenced AFTER"\
		"    UPDATE OF \"guid\" ON \"resources\" WHEN "\
		"    EXISTS (SELECT 1 FROM \"resourceattrib\" WHERE old.\"guid\" == \"resourceid\")"\
		"BEGIN"\
		"  UPDATE \"resourceattrib\" SET \"resourceid\" = new.\"guid\" WHERE \"resourceid\" = old.\"guid\";"\
		"END;",
		""\
		"CREATE TRIGGER genfkey5_insert_referencing BEFORE INSERT ON \"resources\" WHEN "\
		"    new.\"noteGuid\" IS NOT NULL AND NOT EXISTS (SELECT 1 FROM \"notes\" WHERE new.\"noteGuid\" == \"guid\")"\
		"BEGIN"\
		"  SELECT RAISE(ABORT, 'constraint failed');"\
		"END;",
		"CREATE TRIGGER genfkey5_update_referencing BEFORE"\
		"    UPDATE OF \"noteGuid\" ON \"resources\" WHEN "\
		"    new.\"noteGuid\" IS NOT NULL AND "\
		"    NOT EXISTS (SELECT 1 FROM \"notes\" WHERE new.\"noteGuid\" == \"guid\")"\
		"BEGIN"\
		"  SELECT RAISE(ABORT, 'constraint failed');"\
		"END;",
		"CREATE TRIGGER genfkey5_delete_referenced BEFORE DELETE ON \"notes\" WHEN"\
		"    EXISTS (SELECT 1 FROM \"resources\" WHERE old.\"guid\" == \"noteGuid\")"\
		"BEGIN"\
		"  DELETE FROM \"resources\" WHERE \"noteGuid\" = old.\"guid\";"\
		"END;",
		"CREATE TRIGGER genfkey5_update_referenced AFTER"\
		"    UPDATE OF \"guid\" ON \"notes\" WHEN "\
		"    EXISTS (SELECT 1 FROM \"resources\" WHERE old.\"guid\" == \"noteGuid\")"\
		"BEGIN"\
		"  UPDATE \"resources\" SET \"noteGuid\" = new.\"guid\" WHERE \"noteGuid\" = old.\"guid\";"\
		"END;",
		""\
		"CREATE TRIGGER genfkey8_insert_referencing BEFORE INSERT ON \"thumbnails\" WHEN "\
		"    new.\"noteid\" IS NOT NULL AND NOT EXISTS (SELECT 1 FROM \"notes\" WHERE new.\"noteid\" == \"guid\")"\
		"BEGIN"\
		"  SELECT RAISE(ABORT, 'constraint failed');"\
		"END;",
		"CREATE TRIGGER genfkey8_update_referencing BEFORE"\
		"    UPDATE OF \"noteid\" ON \"thumbnails\" WHEN "\
		"    new.\"noteid\" IS NOT NULL AND "\
		"    NOT EXISTS (SELECT 1 FROM \"notes\" WHERE new.\"noteid\" == \"guid\")"\
		"BEGIN"\
		"  SELECT RAISE(ABORT, 'constraint failed');"\
		"END;",
		"CREATE TRIGGER genfkey8_delete_referenced BEFORE DELETE ON \"notes\" WHEN"\
		"    EXISTS (SELECT 1 FROM \"thumbnails\" WHERE old.\"guid\" == \"noteid\")"\
		"BEGIN"\
		"  DELETE FROM \"thumbnails\" WHERE \"noteid\" = old.\"guid\";"\
		"END;",
		"CREATE TRIGGER genfkey8_update_referenced AFTER"\
		"    UPDATE OF \"guid\" ON \"notes\" WHEN "\
		"    EXISTS (SELECT 1 FROM \"thumbnails\" WHERE old.\"guid\" == \"noteid\")"\
		"BEGIN"\
		"  UPDATE \"thumbnails\" SET \"noteid\" = new.\"guid\" WHERE \"noteid\" = old.\"guid\";"\
		"END;",
		 NULL
	};

#endif /* QVERNOTESTORAGEQUERIES_H_ */
