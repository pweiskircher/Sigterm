#ifndef _LIBRARY_H
#define _LIBRARY_H

#include <QString>
#include <QSqlDatabase>

class Library {
	public:
		Library(const QString &inDatabasePath);
		virtual ~Library();

		bool open();
		
		bool createDatabase();
		
		QString &databasePath();

	private:
		QString mDatabasePath;
		QSqlDatabase mDatabase;

};

#endif
