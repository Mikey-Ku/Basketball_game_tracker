#include "database.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TIMESTAMP_BUFFER_SIZE 32

static sqlite3 *db = NULL;

static int exec_sql(const char *sql) {
    char *err_msg = 0;
    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    return 1;
}

static void format_timestamp(char *buffer, size_t buffer_size) {
    time_t now = time(NULL);
    struct tm tm_now;
    localtime_r(&now, &tm_now);
    if (strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", &tm_now) == 0) {
        buffer[0] = '\0';
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
        "game_id INTEGER,"
        "timestamp TEXT,"
        "player TEXT,"
        "command TEXT,"
        "points INTEGER"
        ");";

    if (!exec_sql(sql)) {
        return 0;
    }

    const char *sql_games =
        "CREATE TABLE IF NOT EXISTS games ("
        "game_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "start_time TEXT,"
        "end_time TEXT"
        ");";
    if (!exec_sql(sql_games)) {
        return 0;
    }

    return 1;
}

void db_insert_event(int game_id, const char *player, const char *command, int points) {
    if (!db || game_id <= 0) {
        return;
    }

    char timestamp[TIMESTAMP_BUFFER_SIZE];
    format_timestamp(timestamp, sizeof(timestamp));

    sqlite3_stmt *stmt;
    const char *sql =
        "INSERT INTO events (game_id, timestamp, player, command, points) "
        "VALUES (?, ?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("SQL prepare error: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, game_id);
    sqlite3_bind_text(stmt, 2, timestamp, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, player, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, command, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, points);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        printf("SQL step error: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
}

int db_get_events_for_game(int game_id, GameEvent **events, int *num_events) {
    if (!db) {
        return 0;
    }
    sqlite3_stmt *stmt;
    const char *sql = "SELECT game_id, timestamp, player, command, points FROM events WHERE game_id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("SQL prepare error: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    sqlite3_bind_int(stmt, 1, game_id);

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        count++;
    }

    *num_events = count;
    if (count == 0) {
        *events = NULL;
        sqlite3_finalize(stmt);
        return 1;
    }

    *events = malloc(sizeof(GameEvent) * count);
    if (!*events) {
        sqlite3_finalize(stmt);
        return 0;
    }

    sqlite3_reset(stmt);

    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        (*events)[i].game_id = sqlite3_column_int(stmt, 0);
        (*events)[i].timestamp = strdup((const char *)sqlite3_column_text(stmt, 1));
        (*events)[i].player = strdup((const char *)sqlite3_column_text(stmt, 2));
        (*events)[i].command = strdup((const char *)sqlite3_column_text(stmt, 3));
        (*events)[i].points = sqlite3_column_int(stmt, 4);
        i++;
    }

    sqlite3_finalize(stmt);
    return 1;
}

void db_free_events(GameEvent *events, int num_events) {
    if (!events) {
        return;
    }

    for (int i = 0; i < num_events; i++) {
        free(events[i].timestamp);
        free(events[i].player);
        free(events[i].command);
    }
    free(events);
}

int db_get_finished_games(Game **games, int *num_games) {
    if (!db) {
        return 0;
    }

    sqlite3_stmt *stmt;
    const char *sql = "SELECT game_id, start_time, end_time FROM games WHERE end_time IS NOT NULL;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("SQL prepare error: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        count++;
    }

    *num_games = count;
    if (count == 0) {
        *games = NULL;
        sqlite3_finalize(stmt);
        return 1;
    }

    *games = malloc(sizeof(Game) * count);
    if (!*games) {
        sqlite3_finalize(stmt);
        return 0;
    }

    sqlite3_reset(stmt);

    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        (*games)[i].id = sqlite3_column_int(stmt, 0);
        (*games)[i].start_time = strdup((const char *)sqlite3_column_text(stmt, 1));
        (*games)[i].end_time = strdup((const char *)sqlite3_column_text(stmt, 2));
        i++;
    }

    sqlite3_finalize(stmt);
    return 1;
}

void db_free_games(Game *games, int num_games) {
    if (!games) {
        return;
    }

    for (int i = 0; i < num_games; i++) {
        free(games[i].start_time);
        free(games[i].end_time);
    }
    free(games);
}

int db_start_game() {
    if (!db) {
        return -1;
    }
    char timestamp[TIMESTAMP_BUFFER_SIZE];
    format_timestamp(timestamp, sizeof(timestamp));

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO games (start_time) VALUES (?);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("SQL prepare error: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    sqlite3_bind_text(stmt, 1, timestamp, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        printf("SQL step error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);
    return (int)sqlite3_last_insert_rowid(db);
}

void db_end_game(int game_id) {
    if (!db) {
        return;
    }
    char timestamp[TIMESTAMP_BUFFER_SIZE];
    format_timestamp(timestamp, sizeof(timestamp));

    sqlite3_stmt *stmt;
    const char *sql = "UPDATE games SET end_time=? WHERE game_id=?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("SQL prepare error: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, timestamp, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, game_id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        printf("SQL step error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

void db_close() {
    if (db) {
        sqlite3_close(db);
        db = NULL;
        printf("Database closed.\n");
    }
}
