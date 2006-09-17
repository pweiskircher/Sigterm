
#include "Library.h"
#include <QSqlQuery>

Library::Library(const QString &inDatabasePath) {
	mDatabasePath = inDatabasePath;
	
	mDatabase = QSqlDatabase::addDatabase("QSQLITE", "sigtermDb");
	mDatabase.setDatabaseName(mDatabasePath);
}
	
Library::~Library() {
}

bool Library::open() {
	
	if (!mDatabase.open()) {
		qWarning("Could not open database");
	}

	return this->createDatabase();
}

bool Library::createDatabase() {
	QSqlQuery *query = new QSqlQuery(mDatabase);
	
	query->exec("CREATE TABLE artist(id INTEGER CONSTRAINT artist_id NOT NULL PRIMARY KEY AUTOINCREMENT, name TEXT)");
	query->exec("CREATE UNIQUE INDEX artist_key_name ON artist(name asc)");
	query->exec("CREATE TABLE album(id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, artist INTEGER NOT NULL, name TEXT)");
	query->exec("CREATE UNIQUE INDEX album_key_name ON album(artist, name asc)");
	query->exec("CREATE TABLE track(id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, filename TEXT, artist INTEGER, album INTEGER, title TEXT, length INTEGER, rating INTEGER, playcount INTEGER)");
	query->exec("CREATE UNIQUE INDEX track_key_filename ON track(filename asc)");
	query->exec("CREATE INDEX track_key_name ON track(artist, title)");

	return true;
}

QString &Library::databasePath() {
	return mDatabasePath;
}


