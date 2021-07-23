#ifndef SMP2_SPEC_H
#define SMP2_SPEC_H

#include "../src/testing.h"

namespace wall_e {

class smp2_spec {
    static void cc_test(); wall_e_test(smp2_spec, cc_test)
    static void dd_test(); wall_e_test(smp2_spec, dd_test)
    static void cd_test(); wall_e_test(smp2_spec, cd_test)
    static void dc_test(); wall_e_test(smp2_spec, dc_test)

    static void simplify_test(); wall_e_test(smp2_spec, simplify_test)
};

}
#endif // SMP2_SPEC_H
