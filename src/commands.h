#ifndef COMMANDS_H
#define COMMANDS_H

typedef enum {
    CMD_INVALID = 0,
    CMD_MADE_3,
    CMD_MISSED_3,
    CMD_MADE_2,
    CMD_MISSED_2,
    CMD_MADE_FT,
    CMD_MISSED_FT,
    CMD_ASSIST,
    CMD_REBOUND,
    CMD_STEAL,
    CMD_BLOCK,
    CMD_TURNOVER,
    CMD_FOUL,
    CMD_START_GAME,
    CMD_END_GAME
} CommandType;

#endif
