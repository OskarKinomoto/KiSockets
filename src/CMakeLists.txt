include_directories(".")

file(GLOB HEADERS *.h)

set(MAIN_SOURCES
    Socket.cpp
    Log.cpp)

add_library(${MAIN_TARGET} ${MAIN_SOURCES} ${HEADERS})

install(TARGETS ${MAIN_TARGET} DESTINATION ${BIN_DIR})
