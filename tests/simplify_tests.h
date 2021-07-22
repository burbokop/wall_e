#ifndef SIMPLIFY_TESTS_H
#define SIMPLIFY_TESTS_H

#include "../src/private/gram_private.h"

namespace wall_e {

struct simplify_tests {
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
};

}
#endif // SIMPLIFY_TESTS_H
