#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "commands.h"
#include "stats.h"

CommandType command_from_string(const char *cmd) {
    if (strcmp(cmd, "MADE_3") == 0) return CMD_MADE_3;
    if (strcmp(cmd, "MISSED_3") == 0) return CMD_MISSED_3;
    if (strcmp(cmd, "MADE_2") == 0) return CMD_MADE_2;
    if (strcmp(cmd, "MISSED_2") == 0) return CMD_MISSED_2;
    if (strcmp(cmd, "MADE_FT") == 0) return CMD_MADE_FT;
    if (strcmp(cmd, "MISSED_FT") == 0) return CMD_MISSED_FT;
    if (strcmp(cmd, "ASSIST") == 0) return CMD_ASSIST;
    if (strcmp(cmd, "REBOUND") == 0) return CMD_REBOUND;
    if (strcmp(cmd, "STEAL") == 0) return CMD_STEAL;
    if (strcmp(cmd, "BLOCK") == 0) return CMD_BLOCK;
    if (strcmp(cmd, "TURNOVER") == 0) return CMD_TURNOVER;
    if (strcmp(cmd, "FOUL") == 0) return CMD_FOUL;
    if (strcmp(cmd, "START_GAME") == 0) return CMD_START_GAME;
    if (strcmp(cmd, "END_GAME") == 0) return CMD_END_GAME;

    return CMD_INVALID;
}

void parse_message(char *msg) {
    char msg_copy[1024];
    strcpy(msg_copy, msg);
    handle_message(msg_copy);
}

void handle_message(char *msg) {
    char *command_str = strtok(msg, " ");
    char *player = strtok(NULL, " ");

    if (command_str == NULL) {
        printf("Empty message.\n");
        return;
    }

    CommandType cmd = command_from_string(command_str);

    if (cmd == CMD_START_GAME) {
        start_game();
        return;
    }

    if (cmd == CMD_END_GAME) {
        end_game();
        return;
    }

    if (player == NULL) {
        printf("Command '%s' missing player.\n", command_str);
        return;
    }

    if (!is_game_active() && cmd != CMD_START_GAME) {
    printf("Cannot record stats — no active game.\n");
    return;
    }
    printf("Event received: %s\n", msg);
    print_timestamp();

    switch (cmd) {
        case CMD_MADE_3:
            printf("-> %s made a THREE.\n", player);
            made_three(player);
            break;
        case CMD_MISSED_3:
            printf("-> %s MISSED a THREE.\n", player);
            missed_three(player);
            break;
        case CMD_MADE_2:
            printf("-> %s made a TWO.\n", player);
            made_two(player);
            break;
        case CMD_MISSED_2:
            printf("-> %s MISSED a TWO.\n", player);
            missed_two(player);
            break;
        case CMD_MADE_FT:
            printf("-> %s made a FREE THROW.\n", player);
            made_free_throw(player);
            break;
        case CMD_MISSED_FT:
            printf("-> %s MISSED a FREE THROW.\n", player);
            missed_free_throw(player);
            break;
        case CMD_ASSIST:
            printf("-> %s recorded an ASSIST.\n", player);
            add_assist(player);
            break;
        case CMD_REBOUND:
            printf("-> %s grabbed a REBOUND.\n", player);
            add_rebound(player);
            break;
        case CMD_STEAL:
            printf("-> %s got a STEAL.\n", player);
            add_steal(player);
            break;
        case CMD_BLOCK:
            printf("-> %s BLOCKED a shot.\n", player);
            add_block(player);
            break;
        case CMD_TURNOVER:
            printf("-> %s committed a TURNOVER.\n", player);
            add_turnover(player);
            break;
        case CMD_FOUL:
            printf("-> %s committed a FOUL.\n", player);
            add_foul(player);
            break;
        case CMD_START_GAME:
            start_game();
            return;
        case CMD_END_GAME:
            end_game();
            return;
        default:
            printf("-> Unknown command '%s'\n", command_str);
            break;
    }
}
