


cmake_minimum_required(VERSION 2.8)

project(km2 DESCRIPTION "kmova2 compiler")

message("km2 Source dir: " ${CMAKE_CURRENT_SOURCE_DIR})
message("km2 List dir: " ${CMAKE_CURRENT_LIST_DIR})

set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/ctools/klex.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ctools/kgram.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ctools/kgramvariant.cpp
    ${CMAKE_CURRENT_LIST_DIR}/km2.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ctools/math_patterns.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ctools/kgram_tools.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ctools/tree_view_tools.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ctools/kgram_smp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ctools/km2_asm_tools.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ctools/km2_function.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ctools/km2_token_tools.cpp
    )
set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/ctools/klex.h
    ${CMAKE_CURRENT_LIST_DIR}/ctools/kgram.h
    ${CMAKE_CURRENT_LIST_DIR}/ctools/kgramvariant.h
    ${CMAKE_CURRENT_LIST_DIR}/km2.h
    ${CMAKE_CURRENT_LIST_DIR}/ctools/math_patterns.h
    ${CMAKE_CURRENT_LIST_DIR}/ctools/kgram_tools.h
    ${CMAKE_CURRENT_LIST_DIR}/ctools/tree_view_tools.h
    ${CMAKE_CURRENT_LIST_DIR}/ctools/kgram_smp.h
    ${CMAKE_CURRENT_LIST_DIR}/ctools/km2_asm_tools.h
    ${CMAKE_CURRENT_LIST_DIR}/ctools/km2_function.h
    ${CMAKE_CURRENT_LIST_DIR}/ctools/km2_token_tools.h
    )


add_library(km2 SHARED
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(km2
    curses
    ncurses
    )
