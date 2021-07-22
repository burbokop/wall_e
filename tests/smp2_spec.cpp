#include "smp2_spec.h"

#include "../src/private/gram_private.h"
#include "../src/utility/smp2.h"
#include <iostream>

void smp2_spec::common_test0(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    const auto r0 = wall_e::gram::rule_from_str("cmd & SEMICOLON & (0 | block)");
    const auto r1 = (wall_e::gram::rule("cmd") & "SEMICOLON") & (wall_e::gram::rule() | "block");

    const auto r0s = func(r0);
    const auto r1s = func(r1);

    wall_e_should_equal(r0s, r1s);
    wall_e_should_equal(r0s.type(), wall_e::gram::rule_type::Conjunction);
    wall_e_should_equal(r0s.children().size(), 3);
    wall_e_should_equal(r0s.children()[0], wall_e::gram::rule("cmd"));
    wall_e_should_equal(r0s.children()[1], wall_e::gram::rule("SEMICOLON"));
    wall_e_should_equal(r0s.children()[2].type(), wall_e::gram::rule_type::Disjunction);
    wall_e_should_equal(r0s.children()[2].children().size(), 2);
    wall_e_should_equal(r0s.children()[2].children()[0], wall_e::gram::rule("0"));
    wall_e_should_equal(r0s.children()[2].children()[1], wall_e::gram::rule("block"));
}

void smp2_spec::cc_test() {
    common_test0(smp2::cc);
}

void smp2_spec::simplify_test() {
    common_test0(smp2::simplify);
}
