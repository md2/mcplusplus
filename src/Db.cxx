#include <string.h>
#include "common.h"

Db::Db(const char *dbfile): db(NULL), dbfile(dbfile) {
	char *sql;
	char *errmsg;
	if (sqlite3_open_v2(dbfile, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL) != SQLITE_OK) {
		fprintf(stderr, _("SQLite3 error: %s\n"), sqlite3_errmsg(db));
		sqlite3_close(db);
		db = NULL;
		return;
	}
	sqlite3_busy_timeout(db, 1000);
	sql = sqlite3_mprintf("create table stat(rule varchar(256) unique,success_count int);");
	if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK) {
		//fprintf(stderr, _("SQLite3 error: %s\n"), errmsg);
		sqlite3_free(errmsg);
		sqlite3_free(sql);
		errmsg = NULL;
		return;
	}
	sqlite3_free(sql);
}

Db::~Db() {
	sqlite3_close(db);
	db = NULL;
}

void Db::updateStatisticsForRule(const char *rule) {
	char *sql;
	sqlite3_stmt *stmt;
	char *errmsg;
	sql = sqlite3_mprintf("select rowid from stat where rule = :rule;");
	sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	sqlite3_bind_text(stmt, 1, rule, -1, NULL);
	int result;
	sqlite_int64 rowid = -1;
	while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
		rowid = sqlite3_column_int64(stmt, 0);
	}
	if (result != SQLITE_DONE) {
		fprintf(stderr, _("SQLite3 error: %s\n"), sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_free(sql);
		return;
	}
	sqlite3_finalize(stmt);
	sqlite3_free(sql);
	if (rowid != -1) {
		sql = sqlite3_mprintf("update stat set success_count = success_count + 1 where rowid = :rowid;");
		sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
		sqlite3_bind_int64(stmt, 1, rowid);
	} else {
		sql = sqlite3_mprintf("insert into stat(rule,success_count) values(:rule,1);");
		sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
		sqlite3_bind_text(stmt, 1, rule, -1, NULL);
	}
	if (sqlite3_step(stmt) != SQLITE_DONE) {
		fprintf(stderr, _("SQLite3 error: %s\n"), sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_free(sql);
		return;
	}
	sqlite3_finalize(stmt);
	sqlite3_free(sql);
}

int callback(void *, int nc, char ** val, char **) {
	for (int i = 0; i < nc; i++, val++) {
		printf("%s ", *val);
	}
	printf("\n");
	return 0;
}

