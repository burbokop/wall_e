#include "testing.h"

#include <map>
#include <iostream>
#include <chrono>

namespace wall_e {
namespace testing {

struct test {
    std::string name;
    std::function<void()> test_func;
};

struct benchmark {
    std::string name;
    std::function<void(const benchmark_ctx&)> benchmark_func;
};

struct spec {
    wall_e::list<test> tests;
    wall_e::list<benchmark> benchmarks;
};

std::map<std::string, spec> specs;

template<typename Rep, typename Period>
class pretty_dur {
    std::chrono::duration<Rep, Period> m_val;
public:
    pretty_dur(const std::chrono::duration<Rep, Period>& val) : m_val(val) { }
    friend std::ostream& operator<<(std::ostream& stream, const pretty_dur& mms) {
        if(mms.m_val > std::chrono::minutes(1)) {
            return stream << std::chrono::duration_cast<std::chrono::minutes>(mms.m_val).count() << " m";
        } else if(mms.m_val > std::chrono::seconds(1)) {
            return stream << std::chrono::duration_cast<std::chrono::seconds>(mms.m_val).count() << " s";
        } else if(mms.m_val > std::chrono::milliseconds(1)) {
            return stream << std::chrono::duration_cast<std::chrono::milliseconds>(mms.m_val).count() << " ms";
        } else {
            return stream << std::chrono::duration_cast<std::chrono::nanoseconds>(mms.m_val).count() << " ns";
        }
    }
};



struct min_max_sum {
    typedef std::chrono::nanoseconds::rep nanos;

    nanos min = std::numeric_limits<decltype(min)>::max();
    nanos max = std::numeric_limits<decltype(max)>::min();
    nanos sum = 0;
    std::size_t count = 0;


    friend std::ostream& operator<<(std::ostream& stream, const min_max_sum& mms) {

        return stream << "{ min: " << pretty_dur(std::chrono::nanoseconds(mms.min))
                      << ", max: " << pretty_dur(std::chrono::nanoseconds(mms.max))
                      << ", avr: " << pretty_dur(std::chrono::nanoseconds(mms.max / mms.count))
                      << " }";
    }
};

std::ostream &err_stream() {
    return std::cerr;
}

}
}

void wall_e::testing::benchmark_ctx::each(const std::string &name, const std::function<void ()> &f) const {
    min_max_sum mms;
    const auto main_instant = std::chrono::high_resolution_clock::now();
    for(std::size_t i = 0; i < m_loops_num; ++i) {
        const auto instant = std::chrono::high_resolution_clock::now();
        f();
        const auto mow = std::chrono::high_resolution_clock::now();
        const auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>((mow - instant)).count();
        mms.max = std::max(mms.max, nanos);
        mms.min = std::min(mms.min, nanos);
        mms.sum += nanos;
        ++mms.count;
        if(mow - main_instant > m_max_durarion) {
            break;
        }
    }
    std::cout << "loop: " << m_spec << "." << m_bench_name << "." << name << " -> " << mms << std::endl;
}

int wall_e::testing::register_test(const std::string &name, const std::string &spec, const std::function<void ()> &test_func) {
    specs[spec].tests.push_back({ name, test_func });
    return 0;
}

int wall_e::testing::register_benchmark(const std::string &name, const std::string &spec, const std::function<void (const benchmark_ctx &)> &benchmark_func) {
    specs[spec].benchmarks.push_back({ name, benchmark_func });
    return 0;
}

int wall_e::testing::exec() {
    for(const auto& s : specs) {
        if(s.second.tests.size() > 0) {
            std::cout << "---- " << s.first << " TESTS ----" << std::endl;
            for(const auto& t : s.second.tests) {
                std::cout << "test: " << t.name << std::endl;
                t.test_func();
                std::cout << "      " << t.name << " OK" << std::endl;
            }
        }
        if(s.second.benchmarks.size() > 0) {
            std::cout << "---- " << s.first << " BENCHMARKS ----" << std::endl;
            for(const auto& b : s.second.benchmarks) {
                std::cout << "benchmarks: " << b.name << std::endl;
                benchmark_ctx ctx(s.first, b.name, 1000, std::chrono::duration_cast<dur>(std::chrono::seconds(1)));
                b.benchmark_func(ctx);
                std::cout << "      " << b.name << " OK" << std::endl;
            }
        }
    }
    return 0;
}

