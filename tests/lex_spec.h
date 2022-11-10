#ifndef WALL_LEX_SPEC_H
#define WALL_LEX_SPEC_H

#include "../src/testing.h"

namespace wall_e {

class lex_spec {
    static void make_tokens_bench(const testing::benchmark_ctx& ctx) wall_e_bench(lex_spec, make_tokens_bench);

    static void make_tokens_v1_test() wall_e_test(lex_spec, make_tokens_v1_test);
    static void make_tokens_v2_test() wall_e_test(lex_spec, make_tokens_v2_test);

};

}

#endif // WALL_LEX_SPEC_H
