#include "smp2_spec.h"

#include "../src/private/gram_private.h"
#include "../src/private/gram_smp.h"
#include <iostream>
#include "simplify_tests.h"

void wall_e::smp2_spec::cc_test() {
    simplify_tests::common_test0(smp::cc);
    simplify_tests::common_test1(smp::cc);
    simplify_tests::common_test2(smp::cc);
    simplify_tests::common_test4(smp::cc);
    simplify_tests::common_test6(smp::cc);
    simplify_tests::common_test7(smp::cc);
    simplify_tests::common_test9(smp::cc);
}

void wall_e::smp2_spec::dd_test() {
    simplify_tests::common_test1(smp::dd);
    simplify_tests::common_test4(smp::dd);
    simplify_tests::common_test6(smp::dd);
    simplify_tests::common_test9(smp::dd);
}

void wall_e::smp2_spec::cd_test() {
    simplify_tests::common_test1(smp::cd);
    simplify_tests::common_test4(smp::cd);
    simplify_tests::common_test6(smp::cd);
    simplify_tests::common_test9(smp::cd);
}

void wall_e::smp2_spec::dc_test() {
    simplify_tests::common_test1(smp::dc);
    simplify_tests::common_test4(smp::dc);
    simplify_tests::common_test6(smp::dc);
    simplify_tests::common_test9(smp::dc);
}

void wall_e::smp2_spec::simplify_test() {
    simplify_tests::common_test0(smp::simplify);
    simplify_tests::common_test1(smp::simplify);
    simplify_tests::common_test2(smp::simplify);
    simplify_tests::common_test3(smp::simplify);
    simplify_tests::common_test4(smp::simplify);
    simplify_tests::common_test5(smp::simplify);
    simplify_tests::common_test6(smp::simplify);
    simplify_tests::common_test7(smp::simplify);
    simplify_tests::common_test8(smp::simplify);
    simplify_tests::common_test9(smp::simplify);
}
