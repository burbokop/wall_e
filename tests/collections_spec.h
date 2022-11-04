#ifndef COLLECTIONS_SPEC_H
#define COLLECTIONS_SPEC_H

#include "../src/testing.h"

namespace wall_e {

class collections_spec {
    static void vec_bench(const testing::benchmark_ctx& ctx) wall_e_bench(collections_spec, vec_bench);
    static void lst_bench(const testing::benchmark_ctx& ctx) wall_e_bench(collections_spec, lst_bench);

    static void box_test() wall_e_test(collections_spec, box_test);
    static void box_list_test() wall_e_test(collections_spec, box_list_test);
    static void defer_test() wall_e_test(collections_spec, defer_test);
};

}

#endif // COLLECTIONS_SPEC_H
