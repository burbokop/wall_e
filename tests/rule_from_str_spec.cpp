#include "rule_from_str_spec.h"

#include "../src/private/gram_private.h"
#include "../src/utility/smp2.h"
#include <iostream>

void wall_e::rule_from_str_spec::test0() {
    const auto r0 = gram::rule_from_str("cmd & SEMICOLON & (0 | block)");
    const auto r1 = (wall_e::gram::rule("cmd") & "SEMICOLON") & (wall_e::gram::rule() | "block");

    const auto r0s = smp2::simplify(r0);
    const auto r1s = smp2::simplify(r1);

    wall_e_should_equal(r0s, r1s)

    wall_e_should_equal(r0.type(), gram::rule_type::Conjunction);
    wall_e_should_equal(r0[0], "cmd");
    wall_e_should_equal(r0[1].type(), gram::rule_type::Conjunction);
    wall_e_should_equal(r0[1][0], "SEMICOLON");
    wall_e_should_equal(r0[1][1].type(), gram::rule_type::Disjunction);
    wall_e_should_equal(r0[1][1][0], gram::rule());
    wall_e_should_equal(r0[1][1][1], "block");





    const auto a = wall_e::gram::pattern::from_str("block << cmd & SEMICOLON & (0 | block)");

    const auto b = wall_e::gram::pattern("block")
                        << ((wall_e::gram::rule("cmd") & "SEMICOLON") & (wall_e::gram::rule() | "block"));

    wall_e_should_equal(a.name(), "block");
    wall_e_should_equal(b.name(), "block");

    wall_e_should_equal(a.gram_rule(), r0);
    wall_e_should_equal(b.gram_rule(), r1);
}
