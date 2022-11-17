#include "simplify_tests.h"

#include "../src/testing.h"
#include <iostream>

void wall_e::simplify_tests::common_test0(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    const auto r0 = wall_e::gram::rule("cmd") & ("SEMICOLON" & (wall_e::gram::rule() | "block"));
    const auto r1 = (wall_e::gram::rule("cmd") & "SEMICOLON") & (wall_e::gram::rule() | "block");

    const auto r0s = func(r0);
    const auto r1s = func(r1);

    wall_e_should_equal(r0s, r1s);
    wall_e_should_equal(r0s.type(), wall_e::gram::rule_type::Conjunction);
    wall_e_should_equal(r0s.size(), 3);
    wall_e_should_equal(r0s[0], wall_e::gram::rule("cmd"));
    wall_e_should_equal(r0s[1], wall_e::gram::rule("SEMICOLON"));
    wall_e_should_equal(r0s[2].type(), wall_e::gram::rule_type::Disjunction);
    wall_e_should_equal(r0s[2].size(), 2);
    wall_e_should_equal(r0s[2][0], wall_e::gram::rule());
    wall_e_should_equal(r0s[2][1], wall_e::gram::rule("block"));
}

void wall_e::simplify_tests::common_test1(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    const auto r = wall_e::gram::rule("block");
    const auto rs = func(r);
    wall_e_should_equal(rs, wall_e::gram::rule("block"));
}

void wall_e::simplify_tests::common_test2(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    const auto r0 = wall_e::gram::rule("cmd") & ("SEMICOLON" & (wall_e::gram::rule("EB") | "internal_block"));
    const auto r1 = ((wall_e::gram::rule("cmd") & "SEMICOLON") & (wall_e::gram::rule("EB") | "internal_block"));

    const auto r0s = func(r0);
    const auto r1s = func(r1);

    wall_e_should_equal(r0s, r1s);
}

void wall_e::simplify_tests::common_test3(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    const auto r = (wall_e::gram::rule("wait") | "curr_time" | "function_call" | "function_declaration" | "asm_insertion");

    const auto rs = func(r);

    wall_e_should_equal(rs.type(), wall_e::gram::rule_type::Disjunction);
    wall_e_should_equal(rs[0], wall_e::gram::rule("wait"));
    wall_e_should_equal(rs[1], wall_e::gram::rule("curr_time"));
    wall_e_should_equal(rs[2], wall_e::gram::rule("function_call"));
    wall_e_should_equal(rs[3], wall_e::gram::rule("function_declaration"));
    wall_e_should_equal(rs[4], wall_e::gram::rule("asm_insertion"));
}

void wall_e::simplify_tests::common_test4(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    const auto r = wall_e::gram::rule("TOK_WAIT") & "NUMBER";

    const auto rs = func(r);

    wall_e_should_equal(rs.type(), wall_e::gram::rule_type::Conjunction);
    wall_e_should_equal(rs[0], wall_e::gram::rule("TOK_WAIT"));
    wall_e_should_equal(rs[1], wall_e::gram::rule("NUMBER"));
}

void wall_e::simplify_tests::common_test5(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    const auto r = (wall_e::gram::rule("ID") & "EQUALS" & "OP" & (wall_e::gram::rule("EP") | "decl_arg_list") & "OB" & (wall_e::gram::rule("EB") | "internal_block"));

    const auto rs = func(r);

    wall_e_should_equal(rs.type(), wall_e::gram::rule_type::Conjunction);
    wall_e_should_equal(rs[0], wall_e::gram::rule("ID"));
    wall_e_should_equal(rs[1], wall_e::gram::rule("EQUALS"));
    wall_e_should_equal(rs[2], wall_e::gram::rule("OP"));
    wall_e_should_equal(rs[3].type(), wall_e::gram::rule_type::Disjunction);
    wall_e_should_equal(rs[3][0], wall_e::gram::rule("EP"));
    wall_e_should_equal(rs[3][1], wall_e::gram::rule("decl_arg_list"));
    wall_e_should_equal(rs[4], wall_e::gram::rule("OB"));
    wall_e_should_equal(rs[5].type(), wall_e::gram::rule_type::Disjunction);
    wall_e_should_equal(rs[5][0], wall_e::gram::rule("EB"));
    wall_e_should_equal(rs[5][1], wall_e::gram::rule("internal_block"));
}

void wall_e::simplify_tests::common_test6(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    const auto r = (wall_e::gram::rule("decl_arg") & (wall_e::gram::rule("EP") | (wall_e::gram::rule("COMA") & "decl_arg_list")));

    const auto rs = func(r);

    wall_e_should_equal(rs.type(), wall_e::gram::rule_type::Conjunction);
    wall_e_should_equal(rs[0], "decl_arg");
    wall_e_should_equal(rs[1].type(), wall_e::gram::rule_type::Disjunction);
    wall_e_should_equal(rs[1][0], "EP");
    wall_e_should_equal(rs[1][1].type(), wall_e::gram::rule_type::Conjunction);
    wall_e_should_equal(rs[1][1][0], "COMA");
    wall_e_should_equal(rs[1][1][1], "decl_arg_list");
}

void wall_e::simplify_tests::common_test7(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    const auto r = (wall_e::gram::rule("ID") & "OP" & (wall_e::gram::rule("EP") | "arg_list"));

    const auto rs = func(r);

    wall_e_should_equal(rs.type(), wall_e::gram::rule_type::Conjunction);
    wall_e_should_equal(rs[0], "ID");
    wall_e_should_equal(rs[1], "OP");
    wall_e_should_equal(rs[2].type(), wall_e::gram::rule_type::Disjunction);
    wall_e_should_equal(rs[2][0], "EP");
    wall_e_should_equal(rs[2][1], "arg_list");
}

void wall_e::simplify_tests::common_test8(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    const auto r = (wall_e::gram::rule("TOK_ASM") & "OP" & "STRING_LITERAL" & "EP");

    const auto rs = func(r);

    wall_e_should_equal(rs.type(), wall_e::gram::rule_type::Conjunction);
    wall_e_should_equal(rs[0], "TOK_ASM");
    wall_e_should_equal(rs[1], "OP");
    wall_e_should_equal(rs[2], "STRING_LITERAL");
    wall_e_should_equal(rs[3], "EP");
}

void wall_e::simplify_tests::common_test9(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    (void)func;
}
