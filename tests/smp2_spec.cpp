#include "smp2_spec.h"

#include "../src/private/gram_private.h"
#include "../src/utility/smp2.h"
#include <iostream>
#include "simplify_tests.h"

void wall_e::smp2_spec::cc_test() {
    simplify_tests::common_test0(smp2::cc);
    simplify_tests::common_test1(smp2::cc);
    simplify_tests::common_test2(smp2::cc);
    simplify_tests::common_test4(smp2::cc);
    simplify_tests::common_test6(smp2::cc);
    simplify_tests::common_test7(smp2::cc);
    simplify_tests::common_test9(smp2::cc);
}

void wall_e::smp2_spec::dd_test() {
    simplify_tests::common_test1(smp2::dd);
    simplify_tests::common_test4(smp2::dd);
    simplify_tests::common_test6(smp2::dd);
    simplify_tests::common_test9(smp2::dd);
}

void wall_e::smp2_spec::cd_test() {
    simplify_tests::common_test1(smp2::cd);
    simplify_tests::common_test4(smp2::cd);
    simplify_tests::common_test6(smp2::cd);
    simplify_tests::common_test9(smp2::cd);
}

void wall_e::smp2_spec::dc_test() {
    simplify_tests::common_test1(smp2::dc);
    simplify_tests::common_test4(smp2::dc);
    simplify_tests::common_test6(smp2::dc);
    simplify_tests::common_test9(smp2::dc);
}

void wall_e::smp2_spec::simplify_test() {
    simplify_tests::common_test0(smp2::simplify);
    simplify_tests::common_test1(smp2::simplify);
    simplify_tests::common_test2(smp2::simplify);
    simplify_tests::common_test3(smp2::simplify);
    simplify_tests::common_test4(smp2::simplify);
    simplify_tests::common_test5(smp2::simplify);
    simplify_tests::common_test6(smp2::simplify);
    simplify_tests::common_test7(smp2::simplify);
    simplify_tests::common_test8(smp2::simplify);
    simplify_tests::common_test9(smp2::simplify);
}
