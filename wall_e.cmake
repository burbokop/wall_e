cmake_minimum_required(VERSION 2.8)

set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/lex.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/kgram.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/variant.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/math_patterns.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/kgram_tools.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/tree_view_tools.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/kgram_smp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/asm_tools.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/function.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/token_tools.cpp
    )
set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/lex.h
    ${CMAKE_CURRENT_LIST_DIR}/src/kgram.h
    ${CMAKE_CURRENT_LIST_DIR}/src/variant.h
    ${CMAKE_CURRENT_LIST_DIR}/src/math_patterns.h
    ${CMAKE_CURRENT_LIST_DIR}/src/kgram_tools.h
    ${CMAKE_CURRENT_LIST_DIR}/src/tree_view_tools.h
    ${CMAKE_CURRENT_LIST_DIR}/src/kgram_smp.h
    ${CMAKE_CURRENT_LIST_DIR}/src/asm_tools.h
    ${CMAKE_CURRENT_LIST_DIR}/src/function.h
    ${CMAKE_CURRENT_LIST_DIR}/src/token_tools.h
    )


add_library(wall_e SHARED
    ${SOURCES}
    ${HEADERS}
)
