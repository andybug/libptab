# --- compiler settings ---

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
set(CMKAE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_definitions(-Wall -Wextra -pedantic)
include_directories(${CMAKE_SOURCE_DIR}/include)


# --- build ---

add_subdirectory(libptab)
add_subdirectory(ptab)
