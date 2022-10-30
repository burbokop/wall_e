#ifndef COLLECTIONS_SPEC_H
#define COLLECTIONS_SPEC_H

#include "../src/testing.h"

namespace wall_e {

class collections_spec {
    static void vec_bench(const testing::benchmark_ctx& ctx); wall_e_bench(enums_spec, vec_bench)
    static void lst_bench(const testing::benchmark_ctx& ctx); wall_e_bench(enums_spec, lst_bench)
};

}

#endif // COLLECTIONS_SPEC_H
