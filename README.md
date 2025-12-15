# Basketball Game Tracker

A small TCP client/server written in C that records basketball play-by-play events and stores them through SQLite. The server listens on port `9000`, accepts simple text commands from the client, and keeps per-player box score totals in memory while a game is active. All events are stored so finished games can be reviewed later.

## Features
- Start/stop a game session and log every stat event with timestamps.
- Per-player box score tracking in-memory for the current game.
- SQLite-backed event log for replaying finished games and listing game history.

## Project layout
- `src/server.c` — TCP server entry point; accepts one client and routes commands.
- `src/client.c` — interactive CLI client that sends commands to the server.
- `src/parser.c` — command decoding and response formatting.
- `src/stats.c`/`stats.h` — in-memory box score calculations for the active game.
- `src/database.c`/`database.h` — SQLite helpers for events and game metadata.
- `src/commands.h` — enum of supported command tokens.
- `CMakeLists.txt`, `src/CMakeLists.txt` — build configuration.

## Requirements
- C compiler (GCC/Clang)
- CMake and Make
- SQLite3

## Build
```bash
mkdir -p build
cd build
cmake ..
make
```
This produces `./src/server` and `./src/client` inside the `build` directory.

## Run
Start the server in one terminal:
```bash
cd build
./src/server
```

In another terminal, start the client and enter commands when you see the `>` prompt:
```bash
cd build
./src/client
```

## Command reference
Game flow:
- `START_GAME` — begin a new game (resets in-memory stats, opens a DB game row).
- `END_GAME` — close the current game (marks end time in the DB).

Recording events (format: `<COMMAND> <PLAYER_NAME>`):
- `MADE_3`, `MISSED_3`
- `MADE_2`, `MISSED_2`
- `MADE_FT`, `MISSED_FT`
- `REBOUND`, `ASSIST`, `STEAL`, `BLOCK`, `TURNOVER`, `FOUL`

Queries:
- `STATS <player>` — show the in-memory box score for that player in the active game.
- `LIST_GAMES` — list finished games with IDs and start/end timestamps.
- `GET_GAME_LOG <game_id>` — print the event log for a completed game.

Session control:
- `quit` — exit the client (server keeps running).

## Example session
```
$ ./src/server
Server is listening on port 9000...

$ ./src/client
Connected to the server!
> START_GAME
> MADE_3 MIKE
> ASSIST BOB
> MADE_FT BOB
> STATS MIKE
> END_GAME
> LIST_GAMES
> GET_GAME_LOG 1
> quit
```

## Data and Storage
- Events and game metadata are stored in `games.db` in the working directory where the server runs.
- Each command is timestamped when it reaches the server.
- If you want to start fresh, stop the server and delete `games.db` before restarting.


### Resources Used
https://zetcode.com/db/sqlitec/
https://sqlite.org/c3ref/exec.html
Head First C Programming Chapter 11
https://www.geeksforgeeks.org/c/time-h-header-file-in-c-with-examples/
https://dev.to/jeffreythecoder/how-i-built-a-simple-http-server-from-scratch-using-c-739
