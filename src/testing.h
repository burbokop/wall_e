#ifndef TESTING_H
#define TESTING_H

#include <cassert>
#include <functional>
#include <string>
#include <chrono>
#include "models/variant.h"
#include "wall_e/src/macro.h"




#define wall_e_should_equal(actual, expected) \
    if(actual != expected) { \
        wall_e::testing::err_stream() << __FILE__ << ":" << __LINE__ << ": " << actual << " is not " << expected << " in " << wall_e_this_function << std::endl; \
        exit(1); \
    }

#define wall_e_should_be_defined(option_or_ptr) \
    if(!option_or_ptr) { \
        wall_e::testing::err_stream() << __FILE__ << ":" << __LINE__ << ": option or ptr is not defined in " << wall_e_this_function << std::endl; \
        exit(1); \
    }

#define wall_e_should_be_right(_either) \
    if(!_either.defined()) { \
        wall_e::testing::err_stream() << __FILE__ << ":" << __LINE__ << ": either should be right but " << _either << " in " << wall_e_this_function << std::endl; \
        exit(1); \
    }

namespace wall_e {
namespace testing {

std::ostream& err_stream();

int exec();

typedef std::chrono::high_resolution_clock::duration dur;

class benchmark_ctx {
    friend int exec();
    std::string m_spec;
    std::string m_bench_name;
    std::size_t m_loops_num;
    dur m_max_durarion;
    benchmark_ctx(
            const std::string& spec,
            const std::string& bench_name,
            const std::size_t loops_num,
            const dur& max_durarion
            )
        : m_spec(spec),
          m_bench_name(bench_name),
          m_loops_num(loops_num),
          m_max_durarion(max_durarion) {}

public:
    void each(const std::string& name, const std::function<void()>& f) const;
};

int register_test(const char* name, const char* spec, const std::function<void()> &test_func);
int register_benchmark(const char* name, const char* spec, const std::function<void(const benchmark_ctx&)> &benchmark_func);

template<typename T>
class class_registerer {
    static const inline int static_call = registerTestClass(wall_e::type_name<T>());
public:
    constexpr class_registerer() { (void)static_call; }
};


}
}

#define wall_e_test(SPEC, TEST) ; static int inline __ ## TEST ## _test_registration = wall_e::testing::register_test(#TEST, #SPEC, &TEST)
#define wall_e_bench(SPEC, TEST) ; static int inline __ ## TEST ## _test_registration = wall_e::testing::register_benchmark(#TEST, #SPEC, &TEST)

#define wall_e_register_class(TAG) auto __ ## TAG ## _class_registration() { \
    static constexpr wall_e::testing::class_registerer<std::remove_pointer<decltype (this)>::type> result; \
    return result; \
}


#endif // TESTING_H
