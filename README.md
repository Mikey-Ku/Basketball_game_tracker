# Basketball Game Tracker

This is a client-server application for tracking basketball game statistics.

## How to Build and Run

### Prerequisites

- C compiler (like GCC)
- CMake
- Make

### Building

1.  Create a build directory:
    ```bash
    mkdir build
    cd build
    ```

2.  Run CMake and Make:
    ```bash
    cmake ..
    make
    ```

### Running the Server

From the `build` directory, run the server:
```bash
./src/server
```
The server will start and listen on port 9000.

### Running the Client

In a separate terminal, from the `build` directory, run the client:
```bash
./src/client
```
The client will connect to the server, and you will see a `>` prompt.

## Commands

The client accepts the following commands:

### Game Management

-   `START_GAME`: Starts a new game. A new game must be started before any stats can be recorded.
-   `END_GAME`: Ends the current game.

### Stat Tracking

To record a stat for a player, use the following format:
`<COMMAND> <PLAYER_NAME>`

Example:
`MADE_3 Michael`

Here are the available stat commands:
-   `MADE_3`: Records a made 3-point shot.
-   `MISSED_3`: Records a missed 3-point shot.
-   `MADE_2`: Records a made 2-point shot.
-   `MISSED_2`: Records a missed 2-point shot.
-   `MADE_FT`: Records a made free throw.
-   `MISSED_FT`: Records a missed free throw.
-   `REBOUND`: Records a rebound.
-   `ASSIST`: Records an assist.
-   `STEAL`: Records a steal.
-   `BLOCK`: Records a block.
-   `TURNOVER`: Records a turnover.
-   `FOUL`: Records a foul.

### Viewing Stats and Game Logs

-   `LIST_GAMES`: Displays a list of all completed games, with their `game_id` and timestamps.
-   `GET_GAME_LOG <game_id>`: Displays the log of all events for a specific game. Replace `<game_id>` with the ID of the game you want to view.
-   `STATS <game_id>`: Displays the full statistics for all players in a specific game. Replace `<game_id>` with the ID of the game you want to view.
-   `PLAYER_STATS <player_name>`: Displays the statistics for a specific player in the current game. Replace `<player_name>` with the name of the player you want to view.

### Exiting the Client

-   `quit`: Disconnects from the server and exits the client.