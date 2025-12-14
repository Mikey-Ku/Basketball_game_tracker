#include <time.h>
#ifndef STATS_H
#define STATS_H


#define MAX_PLAYER_NAME_LENGTH 32

typedef struct {
    char name[MAX_PLAYER_NAME_LENGTH];
    int points;
    float field_goal_percentage;
    int field_goals_made;
    int field_goals_attempted;
    float three_point_percentage;
    int threes_made;
    int threes_attempted;
    float two_point_percentage;
    int twos_made;
    int twos_attempted;
    int free_throws_made;
    int free_throws_attempted;
    float free_throw_percentage;
    int rebounds;
    int assists;
    int steals;
    int blocks;
    int turnovers;
    int fouls;
} PlayerStats;

PlayerStats* get_player(const char *name);

void made_three(const char *player);
void missed_three(const char *player);
void made_two(const char *player);
void missed_two(const char *player);
void made_free_throw(const char *player);
void missed_free_throw(const char *player);
void add_rebound(const char *player);
void add_assist(const char *player);
void add_steal(const char *player);
void add_block(const char *player);
void add_turnover(const char *player);
void add_foul(const char *player);

char* get_game_stats(int game_id);

void start_game();
void end_game();
int is_game_active();
int get_current_game_id();
void print_timestamp();
void print_all_stats(char *buffer, size_t buffer_size);
PlayerStats* get_player_stats(const char *name);

#endif
