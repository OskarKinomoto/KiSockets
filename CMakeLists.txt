cmake_minimum_required(VERSION 3.5)

project(KiSockets)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_definitions(-DBUILD_TYPE_DEBUG)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")


#add_definitions(-DCERR_LOGGER)
#add_definitions(-DSYSTEMD_LOGGER)

set(BIN_DIR "/usr/bin")

set(MAIN_TARGET "kisocket")
set(TEST_TARGET "kisocket-test")

add_subdirectory(src)
add_subdirectory(tests)
