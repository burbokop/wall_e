#include "simplify_rule_spec.h"


#include "simplify_tests.h"


void wall_e::simplify_rule_spec::simplify_test() {
    simplify_tests::common_test1(wall_e::gram::simplify_rule_default);
    simplify_tests::common_test4(wall_e::gram::simplify_rule_default);
    simplify_tests::common_test5(wall_e::gram::simplify_rule_default);
    simplify_tests::common_test6(wall_e::gram::simplify_rule_default);
    simplify_tests::common_test7(wall_e::gram::simplify_rule_default);
    simplify_tests::common_test8(wall_e::gram::simplify_rule_default);
    simplify_tests::common_test9(wall_e::gram::simplify_rule_default);
}
