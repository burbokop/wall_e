#ifndef TESTING_H
#define TESTING_H

#include <cassert>
#include <functional>
#include <string>
#include "variant.h"

#define wall_e_should_equal(actual, expected) \
    if(actual != expected) { \
        std::cout << actual << " is not " << expected << " here " << __FILE__ << ":" << __LINE__ << " " << __ASSERT_FUNCTION << std::endl; \
    }

#define wall_e_should_be_defined(option) \
    if(option.has_value()) { \
        std::cout << "option is not defined here " << __FILE__ << ":" << __LINE__ << " " << __ASSERT_FUNCTION << std::endl; \
    }

namespace wall_e {
namespace testing {

int registerTest(const std::string& name, const std::string& spec, const std::function<void()> &testFunc);
int exec();

template<typename T>
class class_registerer {
    static const inline int static_call = registerTestClass(wall_e::type_name<T>());
public:
    constexpr class_registerer() { (void)static_call; }
};


}
}

#define wall_e_test(SPEC, TEST) static int inline __ ## TEST ## _test_registration = wall_e::testing::registerTest(#TEST, #SPEC, &TEST);

#define wall_e_register_class(TAG) auto __ ## TAG ## _class_registration() { \
    static constexpr wall_e::testing::class_registerer<std::remove_pointer<decltype (this)>::type> result; \
    return result; \
}


#endif // TESTING_H
