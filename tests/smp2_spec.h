#ifndef SMP2_SPEC_H
#define SMP2_SPEC_H

#include "../src/testing.h"
#include "../src/private/gram_private.h"

class smp2_spec {
public:
    static void common_test0(const std::function<wall_e::gram::rule(const wall_e::gram::rule&)>& func);
    static void common_test1(const std::function<wall_e::gram::rule(const wall_e::gram::rule&)>& func);
    static void common_test2(const std::function<wall_e::gram::rule(const wall_e::gram::rule&)>& func);
    static void common_test3(const std::function<wall_e::gram::rule(const wall_e::gram::rule&)>& func);
    static void common_test4(const std::function<wall_e::gram::rule(const wall_e::gram::rule&)>& func);
    static void common_test5(const std::function<wall_e::gram::rule(const wall_e::gram::rule&)>& func);
    static void common_test6(const std::function<wall_e::gram::rule(const wall_e::gram::rule&)>& func);
    static void common_test7(const std::function<wall_e::gram::rule(const wall_e::gram::rule&)>& func);
    static void common_test8(const std::function<wall_e::gram::rule(const wall_e::gram::rule&)>& func);
    static void common_test9(const std::function<wall_e::gram::rule(const wall_e::gram::rule&)>& func);

    static void cc_test(); wall_e_test(smp2_spec, cc_test)
    static void dd_test(); wall_e_test(smp2_spec, dd_test)
    static void cd_test(); wall_e_test(smp2_spec, cd_test)
    static void dc_test(); wall_e_test(smp2_spec, dc_test)

    static void simplify_test(); wall_e_test(smp2_spec, simplify_test)
};

#endif // SMP2_SPEC_H
