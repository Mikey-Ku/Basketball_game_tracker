#ifndef db_h
#define db_h

#include <sqlite3.h>

int db_init(const char *filename);

void db_insert_event(const char *player,
                     const char *command,
                     const char *team,
                     int points);

void db_close();

#endif
