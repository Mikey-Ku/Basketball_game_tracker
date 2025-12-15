
#include "stats.h"

#include "database.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POINTS_FOR_THREE_POINTER 3
#define POINTS_FOR_TWO_POINTER 2
#define POINTS_FOR_FREE_THROW 1
#define PERCENTAGE_MULTIPLIER 100
#define LARGE_BUFFER_SIZE 4096
#define MEDIUM_BUFFER_SIZE 1024

#define MAX_PLAYERS 30

static PlayerStats players[MAX_PLAYERS];
static int player_count = 0;
static int game_active = 0;
static int current_game_id = 0;

PlayerStats* get_player(const char *name) {
    for (int i = 0; i < player_count; i++) {
        if (strcmp(players[i].name, name) == 0) {
            return &players[i];
        }
    }
    if (player_count >= MAX_PLAYERS) {
        return NULL;
    }

    snprintf(players[player_count].name, sizeof(players[player_count].name), "%s", name);
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
    if (game_active) {
        return;
    }

    for (int i = 0; i < player_count; i++) {
        players[i].points = 0;
        players[i].field_goal_percentage = 0;
        players[i].field_goals_made = 0;
        players[i].field_goals_attempted = 0;
        players[i].three_point_percentage = 0;
        players[i].threes_made = 0;
        players[i].threes_attempted = 0;
        players[i].two_point_percentage = 0;
        players[i].twos_made = 0;
        players[i].twos_attempted = 0;
        players[i].free_throws_made = 0;
        players[i].free_throws_attempted = 0;
        players[i].free_throw_percentage = 0;
        players[i].rebounds = 0;
        players[i].assists = 0;
        players[i].steals = 0;
        players[i].blocks = 0;
        players[i].turnovers = 0;
        players[i].fouls = 0;
    }

    player_count = 0;

    int started_id = db_start_game();
    if (started_id < 0) {
        game_active = 0;
        return;
    }

    current_game_id = started_id;
    game_active = 1;
}

int is_game_active() {
    return game_active;
}

void end_game() {
    if (!game_active) {
        return;
    }

    db_end_game(current_game_id);
    game_active = 0;
    current_game_id = 0;
    return;
}

int get_current_game_id() {
    return current_game_id;
}

static void update_shot_stats(PlayerStats *plyr, int points, int is_made, int is_three) {
    if (is_made) {
        plyr->points += points;
        plyr->field_goals_made++;
        if (is_three) {
            plyr->threes_made++;
        } else {
            plyr->twos_made++;
        }
    }
    plyr->field_goals_attempted++;
    if (is_three) {
        plyr->threes_attempted++;
    } else {
        plyr->twos_attempted++;
    }
}

static void update_percentages(PlayerStats *plyr) {
    if (plyr->field_goals_attempted > 0) {
        plyr->field_goal_percentage = (float)plyr->field_goals_made / plyr->field_goals_attempted * PERCENTAGE_MULTIPLIER;
    }
    
    if (plyr->threes_attempted > 0) {
        plyr->three_point_percentage = (float)plyr->threes_made / plyr->threes_attempted * PERCENTAGE_MULTIPLIER;
    }
    
    if (plyr->twos_attempted > 0) {
        plyr->two_point_percentage = (float)plyr->twos_made / plyr->twos_attempted * PERCENTAGE_MULTIPLIER;
    }
    
    if (plyr->free_throws_attempted > 0) {
        plyr->free_throw_percentage = (float)plyr->free_throws_made / plyr->free_throws_attempted * PERCENTAGE_MULTIPLIER;
    }
}


void made_three(const char *player) {
    PlayerStats *plyr = get_player(player);
    if (!plyr) { return; }
    update_shot_stats(plyr, POINTS_FOR_THREE_POINTER, 1, 1);
}

void missed_three(const char *player) {
    PlayerStats *plyr = get_player(player);
    if (!plyr) { return; }
    update_shot_stats(plyr, 0, 0, 1);
}

void made_two(const char *player) {
    PlayerStats *plyr = get_player(player);
    if (!plyr) { return; }
    update_shot_stats(plyr, POINTS_FOR_TWO_POINTER, 1, 0);
}

void missed_two(const char *player) {
    PlayerStats *plyr = get_player(player);
    if (!plyr) { return; }
    update_shot_stats(plyr, 0, 0, 0);
}

static void update_free_throw_stats(PlayerStats *plyr, int is_made) {
    if (is_made) {
        plyr->points += POINTS_FOR_FREE_THROW;
        plyr->free_throws_made++;
    }
    plyr->free_throws_attempted++;
}

void made_free_throw(const char *player) {
    PlayerStats *plyr = get_player(player);
    if (!plyr) { return; }
    update_free_throw_stats(plyr, 1);
}

void missed_free_throw(const char *player) {
    PlayerStats *plyr = get_player(player);
    if (!plyr) { return; }
    update_free_throw_stats(plyr, 0);
}

void add_rebound(const char *player) {
    PlayerStats *plyr = get_player(player);
    if (!plyr) { return; }
    plyr->rebounds += 1;
}

void add_assist(const char *player) {
    PlayerStats *plyr = get_player(player);
    if (!plyr) { return; }
    plyr->assists += 1;
}

void add_steal(const char *player) {
    PlayerStats *plyr = get_player(player);
    if (!plyr) { return; }
    plyr->steals += 1;
}

void add_block(const char *player) {
    PlayerStats *plyr = get_player(player);
    if (!plyr) { return; }
    plyr->blocks += 1;
}

void add_turnover(const char *player) {
    PlayerStats *plyr = get_player(player);
    if (!plyr) { return; }
    plyr->turnovers += 1;
}

void add_foul(const char *player) {
    PlayerStats *plyr = get_player(player);
    if (!plyr) { return; }
    plyr->fouls += 1;
}

void print_all_stats(char *buffer, size_t buffer_size) {
    char temp_buffer[LARGE_BUFFER_SIZE] = "";
    int offset = 0;
    for (int i = 0; i < player_count; i++) {
        PlayerStats *plyr = &players[i];
        int written = snprintf(temp_buffer + offset, sizeof(temp_buffer) - offset,
            "\n%s:\n  Points: %d\n  Field Goals: %d/%d (%.1f%%)\n  3-Pointers: %d/%d (%.1f%%)\n  2-Pointers: %d/%d (%.1f%%)\n  Free Throws: %d/%d (%.1f%%)\n  Rebounds: %d | Assists: %d | Steals: %d | Blocks: %d | Turnovers: %d | Fouls: %d\n",
            plyr->name, plyr->points, plyr->field_goals_made, plyr->field_goals_attempted, plyr->field_goal_percentage,
            plyr->threes_made, plyr->threes_attempted, plyr->three_point_percentage, plyr->twos_made, plyr->twos_attempted, plyr->two_point_percentage,
            plyr->free_throws_made, plyr->free_throws_attempted, plyr->free_throw_percentage, plyr->rebounds, plyr->assists, plyr->steals,
            plyr->blocks, plyr->turnovers, plyr->fouls);
        if (written > 0) {
            offset += written;
        }
    }
    snprintf(buffer, buffer_size, "%s", temp_buffer);
}

static PlayerStats* get_player_from_array(PlayerStats *players_array, int *player_count_ptr, const char *name) {
    for (int i = 0; i < *player_count_ptr; i++) {
        if (strcmp(players_array[i].name, name) == 0) {
            return &players_array[i];
        }
    }
    if (*player_count_ptr >= MAX_PLAYERS) {
        return NULL;
    }

    snprintf(players_array[*player_count_ptr].name, sizeof(players_array[*player_count_ptr].name), "%s", name);
    players_array[*player_count_ptr].points = 0;
    players_array[*player_count_ptr].field_goal_percentage = 0;
    players_array[*player_count_ptr].field_goals_made = 0;
    players_array[*player_count_ptr].field_goals_attempted = 0;
    players_array[*player_count_ptr].three_point_percentage = 0;
    players_array[*player_count_ptr].threes_made = 0;
    players_array[*player_count_ptr].threes_attempted = 0;
    players_array[*player_count_ptr].two_point_percentage = 0;
    players_array[*player_count_ptr].twos_made = 0;
    players_array[*player_count_ptr].twos_attempted = 0;
    players_array[*player_count_ptr].free_throws_made = 0;
    players_array[*player_count_ptr].free_throws_attempted = 0;
    players_array[*player_count_ptr].free_throw_percentage = 0;
    players_array[*player_count_ptr].rebounds = 0;
    players_array[*player_count_ptr].assists = 0;
    players_array[*player_count_ptr].steals = 0;
    players_array[*player_count_ptr].blocks = 0;
    players_array[*player_count_ptr].turnovers = 0;
    players_array[*player_count_ptr].fouls = 0;

    (*player_count_ptr)++;
    return &players_array[*player_count_ptr - 1];
}

static void update_stats_from_events(PlayerStats *players_array, int *player_count_ptr, GameEvent *events, int num_events) {
    for (int i = 0; i < num_events; i++) {
        GameEvent *event = &events[i];
        PlayerStats *plyr = get_player_from_array(players_array, player_count_ptr, event->player);
        if (!plyr) { continue; }

        if (strcmp(event->command, "MADE_3") == 0) {
            update_shot_stats(plyr, POINTS_FOR_THREE_POINTER, 1, 1);
        } else if (strcmp(event->command, "MISSED_3") == 0) {
            update_shot_stats(plyr, 0, 0, 1);
        } else if (strcmp(event->command, "MADE_2") == 0) {
            update_shot_stats(plyr, POINTS_FOR_TWO_POINTER, 1, 0);
        } else if (strcmp(event->command, "MISSED_2") == 0) {
            update_shot_stats(plyr, 0, 0, 0);
        } else if (strcmp(event->command, "MADE_FT") == 0) {
            update_free_throw_stats(plyr, 1);
        } else if (strcmp(event->command, "MISSED_FT") == 0) {
            update_free_throw_stats(plyr, 0);
        } else if (strcmp(event->command, "REBOUND") == 0) {
            plyr->rebounds += 1;
        } else if (strcmp(event->command, "ASSIST") == 0) {
            plyr->assists += 1;
        } else if (strcmp(event->command, "STEAL") == 0) {
            plyr->steals += 1;
        } else if (strcmp(event->command, "BLOCK") == 0) {
            plyr->blocks += 1;
        } else if (strcmp(event->command, "TURNOVER") == 0) {
            plyr->turnovers += 1;
        } else if (strcmp(event->command, "FOUL") == 0) {
            plyr->fouls += 1;
        }
    }

    for (int i = 0; i < *player_count_ptr; i++) {
        update_percentages(&players_array[i]);
    }
}

char* get_game_stats(int game_id) {
    GameEvent *events;
    int num_events;

    if (!db_get_events_for_game(game_id, &events, &num_events)) {
        return NULL;
    }

    if (num_events == 0) {
        db_free_events(events, num_events);
        return strdup("No events found for this game.\n");
    }

    PlayerStats local_players[MAX_PLAYERS];
    int local_player_count = 0;

    update_stats_from_events(local_players, &local_player_count, events, num_events);

    char *response = malloc(LARGE_BUFFER_SIZE);
    if (!response) {
        db_free_events(events, num_events);
        return NULL;
    }
    response[0] = '\0';
    int offset = 0;

    for (int i = 0; i < local_player_count; i++) {
        PlayerStats *plyr = &local_players[i];
        int written = snprintf(response + offset, LARGE_BUFFER_SIZE - offset,
            "\n%s:\n  Points: %d\n  Field Goals: %d/%d (%.1f%%)\n  3-Pointers: %d/%d (%.1f%%)\n  2-Pointers: %d/%d (%.1f%%)\n  Free Throws: %d/%d (%.1f%%)\n  Rebounds: %d | Assists: %d | Steals: %d | Blocks: %d | Turnovers: %d | Fouls: %d\n",
            plyr->name, plyr->points, plyr->field_goals_made, plyr->field_goals_attempted, plyr->field_goal_percentage,
            plyr->threes_made, plyr->threes_attempted, plyr->three_point_percentage, plyr->twos_made, plyr->twos_attempted, plyr->two_point_percentage,
            plyr->free_throws_made, plyr->free_throws_attempted, plyr->free_throw_percentage, plyr->rebounds, plyr->assists, plyr->steals,
            plyr->blocks, plyr->turnovers, plyr->fouls);
        if (written > 0) {
            offset += written;
        }
    }

    db_free_events(events, num_events);
    return response;
}

PlayerStats* get_player_stats(const char *name) {
    for (int i = 0; i < player_count; i++) {
        if (strcmp(players[i].name, name) == 0) {
            return &players[i];
        }
    }
    return NULL;
}
