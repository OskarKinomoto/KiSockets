cmake_minimum_required(VERSION 3.5)

project(KiSockets)

set(BIN_DIR "/usr/bin")

set(MAIN_TARGET "KiSockets")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_subdirectory(src)
