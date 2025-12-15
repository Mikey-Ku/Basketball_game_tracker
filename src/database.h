#ifndef db_h
#define db_h

#include <sqlite3.h>

typedef struct {
    int game_id;
    char *timestamp;
    char *player;
    char *command;
    int points;
} GameEvent;

typedef struct {
    int id;
    char *start_time;
    char *end_time;
} Game;

int db_init(const char *filename);

void db_insert_event(int game_id,
                     const char *player,
                     const char *command,
                     int points);

int db_get_events_for_game(int game_id, GameEvent **events, int *num_events);
void db_free_events(GameEvent *events, int num_events);

int db_get_finished_games(Game **games, int *num_games);
void db_free_games(Game *games, int num_games);

int db_start_game();
void db_end_game(int game_id);

void db_close();

#endif
