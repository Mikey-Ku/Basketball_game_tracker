
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "commands.h"
#include "stats.h"
#include "database.h"

#define LARGE_RESPONSE_BUFFER 4096
#define SMALL_RESPONSE_BUFFER 256
#define POINTS_FOR_THREE_POINTER 3
#define POINTS_FOR_TWO_POINTER 2
#define POINTS_FOR_FREE_THROW 1


CommandType command_from_string(const char *cmd) {
    if (strcmp(cmd, "MADE_3") == 0) { return CMD_MADE_3; }
    if (strcmp(cmd, "MISSED_3") == 0) { return CMD_MISSED_3; }
    if (strcmp(cmd, "MADE_2") == 0) { return CMD_MADE_2; }
    if (strcmp(cmd, "MISSED_2") == 0) { return CMD_MISSED_2; }
    if (strcmp(cmd, "MADE_FT") == 0) { return CMD_MADE_FT; }
    if (strcmp(cmd, "MISSED_FT") == 0) { return CMD_MISSED_FT; }
    if (strcmp(cmd, "ASSIST") == 0) { return CMD_ASSIST; }
    if (strcmp(cmd, "REBOUND") == 0) { return CMD_REBOUND; }
    if (strcmp(cmd, "STEAL") == 0) { return CMD_STEAL; }
    if (strcmp(cmd, "BLOCK") == 0) { return CMD_BLOCK; }
    if (strcmp(cmd, "TURNOVER") == 0) { return CMD_TURNOVER; }
    if (strcmp(cmd, "FOUL") == 0) { return CMD_FOUL; }
    if (strcmp(cmd, "START_GAME") == 0) { return CMD_START_GAME; }
    if (strcmp(cmd, "END_GAME") == 0) { return CMD_END_GAME; }
    if (strcmp(cmd, "STATS") == 0) { return CMD_GET_STATS; }
    if (strcmp(cmd, "LIST_GAMES") == 0) { return CMD_LIST_GAMES; }
    if (strcmp(cmd, "GET_GAME_LOG") == 0) { return CMD_GET_GAME_LOG; }
    if (strcmp(cmd, "PLAYER_STATS") == 0) { return CMD_PLAYER_STATS; }

    return CMD_INVALID;
}

char* handle_message(char *msg) {
    char *command_str = strtok(msg, " ");
    char *player = strtok(NULL, " ");
    char *team = strtok(NULL, " ");

    if (command_str == NULL) {
        return NULL;
    }

    CommandType cmd = command_from_string(command_str);

    if (cmd == CMD_GET_STATS) {
        char *game_id_str = player;
        if (game_id_str == NULL) {
            return NULL;
        }
        int game_id = atoi(game_id_str);
        return get_game_stats(game_id);
    }

    if (cmd == CMD_PLAYER_STATS) {
        if (player == NULL) {
            return NULL;
        }
        PlayerStats *p = get_player_stats(player);
        if (p == NULL) {
            return strdup("Player not found.\n");
        }
        char *response = malloc(LARGE_RESPONSE_BUFFER);
        snprintf(response, LARGE_RESPONSE_BUFFER,
            "\n%s:\n  Points: %d\n  Field Goals: %d/%d (%.1f%%)\n  3-Pointers: %d/%d (%.1f%%)\n  2-Pointers: %d/%d (%.1f%%)\n  Free Throws: %d/%d (%.1f%%)\n  Rebounds: %d | Assists: %d | Steals: %d | Blocks: %d | Turnovers: %d | Fouls: %d\n",
            p->name, p->points, p->field_goals_made, p->field_goals_attempted, p->field_goal_percentage,
            p->threes_made, p->threes_attempted, p->three_point_percentage, p->twos_made, p->twos_attempted, p->two_point_percentage,
            p->free_throws_made, p->free_throws_attempted, p->free_throw_percentage, p->rebounds, p->assists, p->steals,
            p->blocks, p->turnovers, p->fouls);
        return response;
    }

    if (cmd == CMD_LIST_GAMES) {
        Game *games;
        int num_games;
        db_get_finished_games(&games, &num_games);

        char *response = malloc(LARGE_RESPONSE_BUFFER);
        response[0] = '\0';
        strcat(response, "Finished Games:\n");

        for (int i = 0; i < num_games; i++) {
            char line[SMALL_RESPONSE_BUFFER];
            snprintf(line, sizeof(line), "Game %d: %s -> %s\n", games[i].id, games[i].start_time, games[i].end_time);
            strcat(response, line);
        }

        db_free_games(games, num_games);
        return response;
    }

    if (cmd == CMD_GET_GAME_LOG) {
        char *game_id_str = player;
        if (game_id_str == NULL) {
            return NULL;
        }
        int game_id = atoi(game_id_str);
        
        GameEvent *events;
        int num_events;
        db_get_events_for_game(game_id, &events, &num_events);

        char *response = malloc(LARGE_RESPONSE_BUFFER);
        response[0] = '\0';
        strcat(response, "Game Log:\n");

        for (int i = 0; i < num_events; i++) {
            char line[SMALL_RESPONSE_BUFFER];
            snprintf(line, sizeof(line), "[%s] %s: %s (%s) - %d points\n", events[i].timestamp, events[i].player, events[i].command, events[i].team, events[i].points);
            strcat(response, line);
        }

        db_free_events(events, num_events);
        return response;
    }

    if (cmd == CMD_START_GAME) {
        start_game();
        return strdup("ACK\n");
    }

    if (cmd == CMD_END_GAME) {
        end_game();
        return strdup("ACK\n");
    }

    if (player == NULL) {
        return NULL;
    }

    if (!is_game_active() && cmd != CMD_START_GAME) {
        return NULL;
    }

    const char *team_value = team ? team : "UNKNOWN";
    int points = 0;

    switch (cmd) {
        case CMD_MADE_3:
            made_three(player);
            points = POINTS_FOR_THREE_POINTER;
            break;
        case CMD_MISSED_3:
            missed_three(player);
            break;
        case CMD_MADE_2:
            made_two(player);
            points = POINTS_FOR_TWO_POINTER;
            break;
        case CMD_MISSED_2:
            missed_two(player);
            break;
        case CMD_MADE_FT:
            made_free_throw(player);
            points = POINTS_FOR_FREE_THROW;
            break;
        case CMD_MISSED_FT:
            missed_free_throw(player);
            break;
        case CMD_ASSIST:
            add_assist(player);
            break;
        case CMD_REBOUND:
            add_rebound(player);
            break;
        case CMD_STEAL:
            add_steal(player);
            break;
        case CMD_BLOCK:
            add_block(player);
            break;
        case CMD_TURNOVER:
            add_turnover(player);
            break;
        case CMD_FOUL:
            add_foul(player);
            break;
        default:
            break;
    }

    if (cmd != CMD_INVALID) {
        db_insert_event(get_current_game_id(), player, command_str, team_value, points);
    }
    
    return strdup("ACK\n");
}

