cmake_minimum_required(VERSION 3.10)

# Project name
project(WordleGame)

# Set C standard
set(CMAKE_C_STANDARD 99)

# Add source files
set(SOURCES
    main.c
    players.c
    wordle.c
)

# Include header files
include_directories(${CMAKE_SOURCE_DIR})

# Create executable
add_executable(wordle ${SOURCES})
