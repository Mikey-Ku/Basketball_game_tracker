#include <stdio.h>
#include <time.h>
#include <string.h>
#include "database.h"

static sqlite3 *db = NULL;

static void exec_sql(const char *sql) {
    char *err = NULL;
    if (sqlite3_exec(db, sql, NULL, NULL, &err) != SQLITE_OK) {
        printf("SQL error: %s\n", err);
        sqlite3_free(err);
    }
}

int db_init(const char *filename) {
    if (sqlite3_open(filename, &db) != SQLITE_OK) {
        printf("Failed to open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    printf("Database loaded: %s\n", filename);

    const char *sql =
        "CREATE TABLE IF NOT EXISTS events ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "timestamp TEXT,"
        "player TEXT,"
        "command TEXT,"
        "team TEXT,"
        "points INTEGER"
        ");";

    exec_sql(sql);

    return 1;
}

void db_insert_event(const char *player,
                     const char *command,
                     const char *team,
                     int points)
{
    if (!db) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char timestamp[32];
    sprintf(timestamp, "%04d-%02d-%02d %02d:%02d:%02d",
            t->tm_year + 1900,
            t->tm_mon + 1,
            t->tm_mday,
            t->tm_hour,
            t->tm_min,
            t->tm_sec
    );

    char sql[512];
    sprintf(sql,
        "INSERT INTO events (timestamp, player, command, team, points) "
        "VALUES ('%s', '%s', '%s', '%s', %d);",
        timestamp, player, command, team, points
    );

    exec_sql(sql);
}

void db_close() {
    if (db) {
        sqlite3_close(db);
        db = NULL;
        printf("Database closed.\n");
    }
}
