cmake_minimum_required(VERSION 2.8)

set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/lex.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/color.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gram.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/variant.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/flag.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/private/gram_private.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/math_patterns.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/tree_view_tools.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/asm_tools.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/function.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/token_tools.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/smp2.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/testing.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/lex.h
    ${CMAKE_CURRENT_LIST_DIR}/src/color.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gram.h
    ${CMAKE_CURRENT_LIST_DIR}/src/variant.h
    ${CMAKE_CURRENT_LIST_DIR}/src/node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/flag.h
    ${CMAKE_CURRENT_LIST_DIR}/src/private/gram_private.h
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/math_patterns.h
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/tree_view_tools.h
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/asm_tools.h
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/function.h
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/token_tools.h
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/smp2.h
    ${CMAKE_CURRENT_LIST_DIR}/src/testing.h
    )


add_library(wall_e SHARED
    ${SOURCES}
    ${HEADERS}
)


add_executable(wall_e_tests
    ${CMAKE_CURRENT_LIST_DIR}/tests/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tests/smp2_spec.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tests/smp2_spec.h
    )

target_link_libraries(wall_e_tests
    wall_e
    )

enable_testing()
add_test(wall_e_tests wall_e_tests)
add_custom_command(
     TARGET wall_e_tests
     COMMENT "wall_e_tests"
     POST_BUILD
     COMMAND wall_e_tests
)


