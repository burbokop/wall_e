#include "smp2_spec.h"

#include "../src/private/gram_private.h"
#include "../src/utility/smp2.h"
#include <iostream>

void smp2_spec::common_test0(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    const auto r0 = wall_e::gram::rule("cmd") & ("SEMICOLON" & (wall_e::gram::rule() | "block"));
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
    wall_e_should_equal(r0s.children()[2].children()[0], wall_e::gram::rule());
    wall_e_should_equal(r0s.children()[2].children()[1], wall_e::gram::rule("block"));
}

void smp2_spec::common_test1(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    const auto r = wall_e::gram::rule("block");
    const auto rs = func(r);
    wall_e_should_equal(rs, wall_e::gram::rule("block"));
}

void smp2_spec::common_test2(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    const auto r0 = wall_e::gram::rule("cmd") & ("SEMICOLON" & (wall_e::gram::rule("EB") | "internal_block"));
    const auto r1 = ((wall_e::gram::rule("cmd") & "SEMICOLON") & (wall_e::gram::rule("EB") | "internal_block"));

    const auto r0s = func(r0);
    const auto r1s = func(r1);

    wall_e_should_equal(r0s, r1s);
}

void smp2_spec::common_test3(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    const auto r = (wall_e::gram::rule("wait") | "curr_time" | "function_call" | "function_declaration" | "asm_insertion");

    const auto rs = func(r);

    wall_e_should_equal(rs.type(), wall_e::gram::rule_type::Disjunction);
    wall_e_should_equal(rs.children()[0], wall_e::gram::rule("wait"));
    wall_e_should_equal(rs.children()[1], wall_e::gram::rule("curr_time"));
    wall_e_should_equal(rs.children()[2], wall_e::gram::rule("function_call"));
    wall_e_should_equal(rs.children()[3], wall_e::gram::rule("function_declaration"));
    wall_e_should_equal(rs.children()[4], wall_e::gram::rule("asm_insertion"));
}

void smp2_spec::common_test4(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    const auto r = wall_e::gram::rule("TOK_WAIT") & "NUMBER";

    const auto rs = func(r);

    wall_e_should_equal(rs.type(), wall_e::gram::rule_type::Conjunction);
    wall_e_should_equal(rs[0], wall_e::gram::rule("TOK_WAIT"));
    wall_e_should_equal(rs[1], wall_e::gram::rule("NUMBER"));
}

void smp2_spec::common_test5(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    const auto r = (wall_e::gram::rule("TOK_ID") & "EQUALS" & "OP" & (wall_e::gram::rule("EP") | "decl_arg_list") & "OB" & (wall_e::gram::rule("EB") | "internal_block"));

    const auto rs = func(r);

    wall_e_should_equal(rs.type(), wall_e::gram::rule_type::Conjunction);
    wall_e_should_equal(rs[0], wall_e::gram::rule("TOK_ID"));
    wall_e_should_equal(rs[1], wall_e::gram::rule("EQUALS"));
    wall_e_should_equal(rs[2], wall_e::gram::rule("OP"));
    wall_e_should_equal(rs[2].type(), wall_e::gram::rule_type::Disjunction);
    wall_e_should_equal(rs[2][0], wall_e::gram::rule("EP"));
    wall_e_should_equal(rs[2][1], wall_e::gram::rule("decl_arg_list"));
    wall_e_should_equal(rs[3], wall_e::gram::rule("OB"));
    wall_e_should_equal(rs[4].type(), wall_e::gram::rule_type::Disjunction);
    wall_e_should_equal(rs[4][0], wall_e::gram::rule("EB"));
    wall_e_should_equal(rs[4][1], wall_e::gram::rule("internal_block"));
}

void smp2_spec::common_test6(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    const auto r0 = wall_e::gram::rule_from_str("decl_arg & (EP | (COMA & decl_arg_list))");
    const auto r1 = (wall_e::gram::rule("decl_arg") & (wall_e::gram::rule("EP") | (wall_e::gram::rule("COMA") & "decl_arg_list")));

    const auto r0s = func(r0);
    const auto r1s = func(r1);

    wall_e_should_equal(r0s, r1s);
}

void smp2_spec::common_test7(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    const auto r0 = wall_e::gram::rule_from_str("TOK_ID & OP & (EP | arg_list)");
    const auto r1 = (wall_e::gram::rule("TOK_ID") & "OP" & (wall_e::gram::rule("EP") | "arg_list"));

    const auto r0s = func(r0);
    const auto r1s = func(r1);

    wall_e_should_equal(r0s, r1s);
}

void smp2_spec::common_test8(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    const auto r0 = wall_e::gram::rule_from_str("arg & (EP | (COMA & arg_list))");
    const auto r1 = (wall_e::gram::rule("arg") & (wall_e::gram::rule("EP") | (wall_e::gram::rule("COMA") & "arg_list")));

    const auto r0s = func(r0);
    const auto r1s = func(r1);

    wall_e_should_equal(r0s, r1s);
}

void smp2_spec::common_test9(const std::function<wall_e::gram::rule (const wall_e::gram::rule &)> &func) {
    const auto r0 = wall_e::gram::rule_from_str("TOK_ASM & OP & STRING_LITERAL & EP");
    const auto r1 = (wall_e::gram::rule("TOK_ASM") & "OP" & "STRING_LITERAL" & "EP");

    const auto r0s = func(r0);
    const auto r1s = func(r1);

    wall_e_should_equal(r0s, r1s);
}

void smp2_spec::cc_test() {
    common_test0(smp2::cc);
    common_test1(smp2::cc);
    common_test2(smp2::cc);
    common_test4(smp2::cc);
    //common_test6(smp2::cc);
    //common_test7(smp2::cc);
    //common_test8(smp2::cc);
    //common_test9(smp2::cc);
}

void smp2_spec::dd_test() {
    common_test1(smp2::dd);
    //common_test4(smp2::dd);
    //common_test5(smp2::dd);
    //common_test6(smp2::dd);
    //common_test7(smp2::dd);
    //common_test8(smp2::dd);
    //common_test9(smp2::dd);
}

void smp2_spec::cd_test() {
    //common_test0(smp2::cd);
    //common_test1(smp2::cd);
    //common_test2(smp2::cd);
    //common_test3(smp2::cd);
    //common_test4(smp2::cd);
    //common_test5(smp2::cd);
    //common_test6(smp2::cd);
    //common_test7(smp2::cd);
    //common_test8(smp2::cd);
    //common_test9(smp2::cd);
}

void smp2_spec::dc_test() {
    //common_test0(smp2::dc);
    //common_test1(smp2::dc);
    //common_test2(smp2::dc);
    //common_test3(smp2::dc);
    //common_test4(smp2::dc);
    //common_test5(smp2::dc);
    //common_test6(smp2::dc);
    //common_test7(smp2::dc);
    //common_test8(smp2::dc);
    //common_test9(smp2::dc);
}

void smp2_spec::simplify_test() {
    common_test0(smp2::simplify);
    //common_test1(smp2::simplify);
    //common_test2(smp2::simplify);
    //common_test3(smp2::simplify);
    //common_test4(smp2::simplify);
    //common_test5(smp2::simplify);
    //common_test6(smp2::simplify);
    //common_test7(smp2::simplify);
    //common_test8(smp2::simplify);
    //common_test9(smp2::simplify);
}
