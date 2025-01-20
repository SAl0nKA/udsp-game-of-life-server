cmake_minimum_required(VERSION 3.29)
project(udsp_game_of_life_server C)

set(CMAKE_C_STANDARD 11)

add_executable(udsp_game_of_life_server
    main.c
        server.h
        server.c)

target_link_libraries(udsp_game_of_life_server ws2_32)

