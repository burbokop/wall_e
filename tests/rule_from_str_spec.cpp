#include "rule_from_str_spec.h"

#include "../src/private/gram_private.h"
#include "../src/utility/smp2.h"
#include <iostream>

using namespace wall_e::gram::literals;

void wall_e::rule_from_str_spec::test0() {
    const auto r0 = "cmd & SEMICOLON & (0 | block)"_rule;

    wall_e_should_equal(r0.type(), gram::rule_type::Conjunction);
    wall_e_should_equal(r0[0], "cmd");
    wall_e_should_equal(r0[1].type(), gram::rule_type::Conjunction);
    wall_e_should_equal(r0[1][0], "SEMICOLON");
    wall_e_should_equal(r0[1][1].type(), gram::rule_type::Disjunction);
    wall_e_should_equal(r0[1][1][0], gram::rule());
    wall_e_should_equal(r0[1][1][1], "block");

    const auto r1 = (wall_e::gram::rule("cmd") & "SEMICOLON") & (wall_e::gram::rule() | "block");

    const auto r0s = smp2::simplify(r0);
    const auto r1s = smp2::simplify(r1);

    wall_e_should_equal(r0s, r1s)

    const auto a = "block << cmd & SEMICOLON & (0 | block)"_pattern;

    const auto b = wall_e::gram::pattern("block")
                        << ((wall_e::gram::rule("cmd") & "SEMICOLON") & (wall_e::gram::rule() | "block"));

    wall_e_should_equal(a.name(), "block");
    wall_e_should_equal(b.name(), "block");

    wall_e_should_equal(a.gram_rule(), r0);
    wall_e_should_equal(b.gram_rule(), r1);
}

void wall_e::rule_from_str_spec::test1() {
    const auto r0 = "cmd & SEMICOLON & (EB | internal_block)"_rule;

    wall_e_should_equal(r0.type(), gram::rule_type::Conjunction);
    wall_e_should_equal(r0[0], "cmd");
    wall_e_should_equal(r0[1].type(), gram::rule_type::Conjunction);
    wall_e_should_equal(r0[1][0], "SEMICOLON");
    wall_e_should_equal(r0[1][1].type(), gram::rule_type::Disjunction);
    wall_e_should_equal(r0[1][1][0], "EB");
    wall_e_should_equal(r0[1][1][1], "internal_block");

    const auto r1 = (wall_e::gram::rule("cmd") & "SEMICOLON") & (wall_e::gram::rule("EB") | "internal_block");

    const auto r0s = smp2::simplify(r0);
    const auto r1s = smp2::simplify(r1);

    wall_e_should_equal(r0s, r1s);


    const auto p = "internal_block << cmd & SEMICOLON & (EB | internal_block)"_pattern;

    wall_e_should_equal(p.name(), "internal_block");
    wall_e_should_equal(p.gram_rule(), r0);
}

void wall_e::rule_from_str_spec::test2() {
    const auto r0 = "wait | curr_time | function_call | function_declaration | asm_insertion"_rule;

    wall_e_should_equal(r0.type(), gram::rule_type::Disjunction);
    wall_e_should_equal(r0[0], "wait");
    wall_e_should_equal(r0[1].type(), gram::rule_type::Disjunction);
    wall_e_should_equal(r0[1][0], "curr_time");
    wall_e_should_equal(r0[1][1].type(), gram::rule_type::Disjunction);
    wall_e_should_equal(r0[1][1][0], "function_call");
    wall_e_should_equal(r0[1][1][1].type(), gram::rule_type::Disjunction);
    wall_e_should_equal(r0[1][1][1][0], "function_declaration");
    wall_e_should_equal(r0[1][1][1][1], "asm_insertion");

    const auto r1 = gram::rule("wait") | "curr_time" | "function_call" | "function_declaration" | "asm_insertion";

    const auto r0s = smp2::simplify(r0);
    const auto r1s = smp2::simplify(r1);

    wall_e_should_equal(r0s, r1s);

    const auto p = "cmd << wait | curr_time | function_call | function_declaration | asm_insertion"_pattern;

    wall_e_should_equal(p.name(), "cmd");
    wall_e_should_equal(p.gram_rule(), r0);
}

void wall_e::rule_from_str_spec::test3() {
    const auto r0 = "TOK_ID & EQUALS & OP & (EP | decl_arg_list) & OB & (EB | internal_block)"_rule;

    std::cout << "ro:" << r0 << std::endl;

    wall_e_should_equal(r0.type(), gram::rule_type::Conjunction);
    wall_e_should_equal(r0.size(), 2);
    wall_e_should_equal(r0[0], "TOK_ID");
    wall_e_should_equal(r0[1].type(), gram::rule_type::Conjunction);
    wall_e_should_equal(r0[1].size(), 2);
    wall_e_should_equal(r0[1][0], "EQUALS");
    wall_e_should_equal(r0[1][1].type(), gram::rule_type::Conjunction);
    wall_e_should_equal(r0[1][1].size(), 2);
    wall_e_should_equal(r0[1][1][0], "OP");
    wall_e_should_equal(r0[1][1][1].type(), gram::rule_type::Conjunction);
    wall_e_should_equal(r0[1][1][1].size(), 2);
    wall_e_should_equal(r0[1][1][1][0].type(), gram::rule_type::Disjunction);
    wall_e_should_equal(r0[1][1][1][0].size(), 2);
    wall_e_should_equal(r0[1][1][1][0][0], "EP");
    wall_e_should_equal(r0[1][1][1][0][1], "decl_arg_list");
    wall_e_should_equal(r0[1][1][1][1].type(), gram::rule_type::Conjunction);
    wall_e_should_equal(r0[1][1][1][1].size(), 2);
    wall_e_should_equal(r0[1][1][1][1][0], "OB");
    wall_e_should_equal(r0[1][1][1][1][1].type(), gram::rule_type::Disjunction);
    wall_e_should_equal(r0[1][1][1][1][1].size(), 2);
    wall_e_should_equal(r0[1][1][1][1][1][0], "EB");
    wall_e_should_equal(r0[1][1][1][1][1][1], "internal_block");

    const auto r1 = wall_e::gram::rule("TOK_ID") & "EQUALS" & "OP" & (wall_e::gram::rule("EP") | "decl_arg_list") & "OB" & (wall_e::gram::rule("EB") | "internal_block");

    const auto r0s = smp2::simplify(r0);
    const auto r1s = smp2::simplify(r1);

    wall_e_should_equal(r0s, r1s);

    const auto p = "function_declaration << TOK_ID & EQUALS & OP & (EP | decl_arg_list) & OB & (EB | internal_block)"_pattern;

    wall_e_should_equal(p.name(), "function_declaration");
    wall_e_should_equal(p.gram_rule(), r0);

}

void wall_e::rule_from_str_spec::test4() {

}
