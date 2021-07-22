#ifndef SMP2_SPEC_H
#define SMP2_SPEC_H

#include "../src/testing.h"
#include "../src/private/gram_private.h"

class smp2_spec {
public:
    static void common_test0(const std::function<wall_e::gram::rule(const wall_e::gram::rule&)>& func);
    static void cc_test(); wall_e_test(smp2_spec, cc_test)
    static void simplify_test(); wall_e_test(smp2_spec, simplify_test)
};

#endif // SMP2_SPEC_H
