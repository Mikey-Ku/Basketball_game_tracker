find_package(SQLite3 REQUIRED)

add_library(bball_core
    commands.h
    database.c
    database.h
    parser.c
    parser.h
    stats.c
    stats.h
)

target_include_directories(bball_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(bball_core PUBLIC SQLite::SQLite3)

add_executable(server server.c)
target_link_libraries(server PRIVATE bball_core)

add_executable(client client.c)
target_link_libraries(client PRIVATE bball_core)
