cmake_minimum_required(VERSION 2.8.7)
project(libptables C)

# --- general project settings ---

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# --- compiler settings ---

add_definitions(-Wall -Wextra -pedantic -std=c99)
include_directories(include)


# --- dependency checks ---

include(CheckIncludeFiles)


# --- descend into source ---

add_subdirectory(src)
