cmake_minimum_required(VERSION 2.8)

set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/lex.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/color.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gram.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/variant.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/private/gram_tools.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/math_patterns.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/tree_view_tools.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/asm_tools.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/function.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/token_tools.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/lex.h
    ${CMAKE_CURRENT_LIST_DIR}/src/color.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gram.h
    ${CMAKE_CURRENT_LIST_DIR}/src/variant.h
    ${CMAKE_CURRENT_LIST_DIR}/src/private/gram_tools.h
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/math_patterns.h
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/tree_view_tools.h
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/asm_tools.h
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/function.h
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/token_tools.h
    )


add_library(wall_e SHARED
    ${SOURCES}
    ${HEADERS}
)
