enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

include_directories("../src")

set(TEST_SOURCES
    main.cpp
    socket.cpp)

add_executable(${TEST_TARGET} ${TEST_SOURCES})

target_link_libraries(${TEST_TARGET} ${GTEST_BOTH_LIBRARIES} ${MAIN_TARGET})

install(TARGETS ${TEST_TARGET} DESTINATION ${BIN_DIR})
