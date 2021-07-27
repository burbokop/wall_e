cmake_minimum_required(VERSION 2.8)

set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/lex.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/color.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gram.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/variant.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/flag.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/either.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/private/gram_private.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/private/gram_smp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/tree_view_tools.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/token_tools.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/testing.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/text_segment.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/lex.h
    ${CMAKE_CURRENT_LIST_DIR}/src/color.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gram.h
    ${CMAKE_CURRENT_LIST_DIR}/src/variant.h
    ${CMAKE_CURRENT_LIST_DIR}/src/node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/flag.h
    ${CMAKE_CURRENT_LIST_DIR}/src/either.h
    ${CMAKE_CURRENT_LIST_DIR}/src/private/gram_private.h
    ${CMAKE_CURRENT_LIST_DIR}/src/private/gram_smp.h
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/tree_view_tools.h
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/token_tools.h
    ${CMAKE_CURRENT_LIST_DIR}/src/testing.h
    ${CMAKE_CURRENT_LIST_DIR}/src/text_segment.h
    )

add_library(wall_e SHARED
    ${SOURCES}
    ${HEADERS}
)

add_executable(wall_e_tests
    ${CMAKE_CURRENT_LIST_DIR}/tests/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tests/simplify_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tests/simplify_tests.h
    ${CMAKE_CURRENT_LIST_DIR}/tests/smp2_spec.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tests/smp2_spec.h
    ${CMAKE_CURRENT_LIST_DIR}/tests/simplify_rule_spec.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tests/simplify_rule_spec.h
    ${CMAKE_CURRENT_LIST_DIR}/tests/rule_from_str_spec.h
    ${CMAKE_CURRENT_LIST_DIR}/tests/rule_from_str_spec.cpp
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


