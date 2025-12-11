#include <stdio.h>
#include <string.h>
#include "stats.h"

static PlayerStats players[50];
static int player_count = 0;
static int game_active = 0;
static int current_game_id = 0;

PlayerStats* get_player(const char *name) {
    for (int i = 0; i < player_count; i++) {
        if (strcmp(players[i].name, name) == 0) {
            return &players[i];
        }
    }
    strcpy(players[player_count].name, name);
    players[player_count].points = 0;
    players[player_count].field_goal_percentage = 0;
    players[player_count].field_goals_made = 0;
    players[player_count].field_goals_attempted = 0;
    players[player_count].three_point_percentage = 0;
    players[player_count].threes_made = 0;
    players[player_count].threes_attempted = 0;
    players[player_count].two_point_percentage = 0;
    players[player_count].twos_made = 0;
    players[player_count].twos_attempted = 0;
    players[player_count].free_throws_made = 0;
    players[player_count].free_throws_attempted = 0;
    players[player_count].free_throw_percentage = 0;
    players[player_count].rebounds = 0;
    players[player_count].assists = 0;
    players[player_count].steals = 0;
    players[player_count].blocks = 0;
    players[player_count].turnovers = 0;
    players[player_count].fouls = 0;

    player_count++;
    return &players[player_count - 1];
}

void start_game() {
    printf("\n=== STARTING NEW GAME ===\n");

    for (int i = 0; i < player_count; i++) {
        players[i].points = 0;
        players[i].threes_made = 0;
        players[i].twos_made = 0;
        players[i].free_throws_made = 0;
        players[i].rebounds = 0;
        players[i].assists = 0;
        players[i].steals = 0;
        players[i].blocks = 0;
        players[i].turnovers = 0;
        players[i].fouls = 0;
    }

    player_count = 0;
    game_active = 1;
    current_game_id++;

    printf("Game %d started.\n\n", current_game_id);
}

int is_game_active() {
    return game_active;
}

void end_game() {
    if (!game_active) {
        printf("No game is currently active.\n");
        return;
    }

    printf("\n=== GAME %d ENDED ===\n", current_game_id);
    print_all_stats();


    int game_active = 0;
}

void print_timestamp() {
    time_t now = time(NULL);         
    struct tm *t = localtime(&now);

    printf("[%04d-%02d-%02d %02d:%02d:%02d] ",
        t->tm_year + 1900,
        t->tm_mon + 1,
        t->tm_mday,
        t->tm_hour,
        t->tm_min,
        t->tm_sec
    );
}

static void update_percentages(PlayerStats *p) {
    if (p->field_goals_attempted > 0) {
        p->field_goal_percentage = (float)p->field_goals_made / p->field_goals_attempted * 100;
    }
    
    if (p->threes_attempted > 0) {
        p->three_point_percentage = (float)p->threes_made / p->threes_attempted * 100;
    }
    
    if (p->twos_attempted > 0) {
        p->two_point_percentage = (float)p->twos_made / p->twos_attempted * 100;
    }
    
    if (p->free_throws_attempted > 0) {
        p->free_throw_percentage = (float)p->free_throws_made / p->free_throws_attempted * 100;
    }
}


void made_three(const char *player) {
    PlayerStats *p = get_player(player);
    p->points += 3;
    p->threes_made += 1;
    p->threes_attempted += 1;
    p->field_goals_made += 1;
    p->field_goals_attempted += 1;
    update_percentages(p);
}

void missed_three(const char *player) {
    PlayerStats *p = get_player(player);
    p->threes_attempted += 1;
    p->field_goals_attempted += 1;
    update_percentages(p);
}

void made_two(const char *player) {
    PlayerStats *p = get_player(player);
    p->points += 2;
    p->twos_made += 1;
    p->twos_attempted += 1;
    p->field_goals_made += 1;
    p->field_goals_attempted += 1;
    update_percentages(p);
}

void missed_two(const char *player) {
    PlayerStats *p = get_player(player);
    p->twos_attempted += 1;
    p->field_goals_attempted += 1;
    update_percentages(p);
}

void made_free_throw(const char *player) {
    PlayerStats *p = get_player(player);
    p->points += 1;
    p->free_throws_made += 1;
    p->free_throws_attempted += 1;
    update_percentages(p);
}

void missed_free_throw(const char *player) {
    PlayerStats *p = get_player(player);
    p->free_throws_attempted += 1;
    update_percentages(p);
}

void add_rebound(const char *player) {
    PlayerStats *p = get_player(player);
    p->rebounds += 1;
}

void add_assist(const char *player) {
    PlayerStats *p = get_player(player);
    p->assists += 1;
}

void add_steal(const char *player) {
    PlayerStats *p = get_player(player);
    p->steals += 1;
}

void add_block(const char *player) {
    PlayerStats *p = get_player(player);
    p->blocks += 1;
}

void add_turnover(const char *player) {
    PlayerStats *p = get_player(player);
    p->turnovers += 1;
}

void add_foul(const char *player) {
    PlayerStats *p = get_player(player);
    p->fouls += 1;
}

void print_all_stats() {
    printf("\n=== Current Player Stats ===\n");
    for (int i = 0; i < player_count; i++) {
        PlayerStats *p = &players[i];
        printf("\n%s:\n", p->name);
        printf("  Points: %d\n", p->points);
        printf("  Field Goals: %d/%d (%.1f%%)\n", p->field_goals_made, p->field_goals_attempted, p->field_goal_percentage);
        printf("  3-Pointers: %d/%d (%.1f%%)\n", p->threes_made, p->threes_attempted, p->three_point_percentage);
        printf("  2-Pointers: %d/%d (%.1f%%)\n", p->twos_made, p->twos_attempted, p->two_point_percentage);
        printf("  Free Throws: %d/%d (%.1f%%)\n", p->free_throws_made, p->free_throws_attempted, p->free_throw_percentage);
        printf("  Rebounds: %d | Assists: %d | Steals: %d | Blocks: %d | Turnovers: %d | Fouls: %d\n",
            p->rebounds, p->assists, p->steals, p->blocks, p->turnovers, p->fouls);
    }
    printf("\n============================\n\n");
}
