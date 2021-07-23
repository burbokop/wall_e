#include "rule_from_str_spec.h"

#include "../src/private/gram_private.h"
#include <iostream>

void wall_e::rule_from_str_spec::test0() {
    const auto r = gram::rule_from_str("cmd & SEMICOLON & (0 | block)");

    wall_e_should_equal(r.type(), gram::rule_type::Conjunction);
    wall_e_should_equal(r[0], "cmd");
    wall_e_should_equal(r[1].type(), gram::rule_type::Conjunction);
    wall_e_should_equal(r[1][0], "SEMICOLON");
    wall_e_should_equal(r[1][1].type(), gram::rule_type::Disjunction);
    wall_e_should_equal(r[1][1][0], gram::rule());
    wall_e_should_equal(r[1][1][1], "block");
}
