#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "commands.h"

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
    return CMD_INVALID;
}

void handle_message(char *msg) {
    char *command_str = strtok(msg, " ");
    char *player = strtok(NULL, " ");

    if (command_str == NULL) {
        printf("Empty message.\n");
        return;
    }

    if (player == NULL) {
        printf("Command '%s' missing player.\n", command_str);
        return;
    }

    CommandType cmd = command_from_string(command_str);

    printf("Parsed command: '%s', player: '%s'\n", command_str, player);

    switch (cmd) {
        case CMD_MADE_3:
            printf("-> %s made a THREE.\n", player);
            break;
        case CMD_MISSED_3:
            printf("-> %s MISSED a THREE.\n", player);
            break;
        case CMD_MADE_2:
            printf("-> %s made a TWO.\n", player);
            break;
        case CMD_MISSED_2:
            printf("-> %s MISSED a TWO.\n", player);
            break;
        case CMD_MADE_FT:
            printf("-> %s made a FREE THROW.\n", player);
            break;
        case CMD_MISSED_FT:
            printf("-> %s MISSED a FREE THROW.\n", player);
            break;
        case CMD_ASSIST:
            printf("-> %s recorded an ASSIST.\n", player);
            break;
        case CMD_REBOUND:
            printf("-> %s grabbed a REBOUND.\n", player);
            break;
        case CMD_STEAL:
            printf("-> %s got a STEAL.\n", player);
            break;
        case CMD_BLOCK:
            printf("-> %s BLOCKED a shot.\n", player);
            break;
        case CMD_TURNOVER:
            printf("-> %s committed a TURNOVER.\n", player);
            break;
        case CMD_FOUL:
            printf("-> %s committed a FOUL.\n", player);
            break;
        default:
            printf("-> Unknown command '%s'\n", command_str);
            break;
    }
}
