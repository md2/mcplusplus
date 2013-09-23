#ifndef DB_H
#define DB_H

#include <sqlite3.h>
#include <string>

class Db {
		std::string dbfile;
		sqlite3 *db;
	public:
		Db(const char *);
		~Db();
		void updateStatisticsForRule(const char *);
};

#endif

