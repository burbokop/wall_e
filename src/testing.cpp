#include "testing.h"

#include <map>
#include <iostream>

namespace wall_e {
namespace testing {

struct test {
    std::string name;
    std::function<void()> testFunc;
};

struct spec {
    std::list<test> tests;
};

std::map<std::string, spec> specs;

}
}

int wall_e::testing::registerTest(const std::string &name, const std::string &spec, const std::function<void ()> &testFunc) {
    specs[spec].tests.push_back({ name, testFunc });
    return 0;
}

int wall_e::testing::exec() {
    for(const auto& s : specs) {
        std::cout << "---- " << s.first << " ----" << std::endl;
        for(const auto& t : s.second.tests) {
            std::cout << "test: " << t.name << std::endl;
            t.testFunc();
            std::cout << "      " << t.name << " OK" << std::endl;
        }
    }
    return 0;
}

