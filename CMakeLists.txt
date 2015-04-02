cmake_minimum_required(VERSION 2.8.9)
project(ptab C CXX)

# --- general project settings ---

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# --- descend into source ---

add_subdirectory(src)
add_subdirectory(tests)
