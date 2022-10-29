cmake_minimum_required(VERSION 3.18.4)

set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/lex.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/color.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gram.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/models/variant.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/models/node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/flag.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/models/either.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/private/gram_private.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/private/gram_smp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/tree_view_tools.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/token_tools.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/testing.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/models/text_segment.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/models/compiler_info.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/models/error.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/models/index.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/stack.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/enums.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/lex.h
    ${CMAKE_CURRENT_LIST_DIR}/src/color.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gram.h
    ${CMAKE_CURRENT_LIST_DIR}/src/models/variant.h
    ${CMAKE_CURRENT_LIST_DIR}/src/models/node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/flag.h
    ${CMAKE_CURRENT_LIST_DIR}/src/models/either.h
    ${CMAKE_CURRENT_LIST_DIR}/src/private/gram_private.h
    ${CMAKE_CURRENT_LIST_DIR}/src/private/gram_smp.h
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/tree_view_tools.h
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/token_tools.h
    ${CMAKE_CURRENT_LIST_DIR}/src/testing.h
    ${CMAKE_CURRENT_LIST_DIR}/src/models/text_segment.h
    ${CMAKE_CURRENT_LIST_DIR}/src/models/compiler_info.h
    ${CMAKE_CURRENT_LIST_DIR}/src/models/error.h
    ${CMAKE_CURRENT_LIST_DIR}/src/models/index.h
    ${CMAKE_CURRENT_LIST_DIR}/src/stack.h
    ${CMAKE_CURRENT_LIST_DIR}/src/enums.h
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/typename.h
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/collections.h
    ${CMAKE_CURRENT_LIST_DIR}/src/utility/collections.cpp
    )

add_library(wall_e SHARED
    ${SOURCES}
    ${HEADERS}
)
set_property(TARGET wall_e PROPERTY AUTOMOC OFF)
set_property(TARGET wall_e PROPERTY AUTOUIC OFF)

add_executable(wall_e_tests
    ${CMAKE_CURRENT_LIST_DIR}/tests/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tests/simplify_tests.h
    ${CMAKE_CURRENT_LIST_DIR}/tests/simplify_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tests/smp2_spec.h
    ${CMAKE_CURRENT_LIST_DIR}/tests/smp2_spec.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tests/simplify_rule_spec.h
    ${CMAKE_CURRENT_LIST_DIR}/tests/simplify_rule_spec.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tests/rule_from_str_spec.h
    ${CMAKE_CURRENT_LIST_DIR}/tests/rule_from_str_spec.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tests/enums_spec.h
    ${CMAKE_CURRENT_LIST_DIR}/tests/enums_spec.cpp
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



install(TARGETS wall_e DESTINATION lib)

install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/src/"
        DESTINATION include/wall_e
        FILES_MATCHING
        PATTERN "*.h"
)
