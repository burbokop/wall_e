#include "enums_spec.h"

#include "../src/enums.h"
#include <iostream>

wall_e_enum(test_enum,
    Value0 = 0,
    Value1,
    Value2,
    Value3,
    Value4 = 10,
    Value5,
    Value6
)

void wall_e::enums_spec::enum_test0() {

    const test_enum mod = test_enum::Value0;

    const auto names = wall_e::enums::reflect_names<test_enum>();

    wall_e_should_equal(wall_e::enums::max_value<test_enum>(), 13);
    wall_e_should_equal(wall_e::enums::max_value<test_enum>(), test_enum::__test_enum_max_value);


    wall_e_should_equal(names.at(Value0), "Value0");
    wall_e_should_equal(names.at(Value1), "Value1");
    wall_e_should_equal(names.at(Value2), "Value2");
    wall_e_should_equal(names.at(Value3), "Value3");
    wall_e_should_equal(names.at(Value4), "Value4");
    wall_e_should_equal(names.at(Value5), "Value5");
    wall_e_should_equal(names.at(Value6), "Value6");

    wall_e_should_equal(0, wall_e::enums::from_string<test_enum>("Value0"));
    wall_e_should_equal(1, wall_e::enums::from_string<test_enum>("Value1"));
    wall_e_should_equal(2, wall_e::enums::from_string<test_enum>("Value2"));
    wall_e_should_equal(3, wall_e::enums::from_string<test_enum>("Value3"));
    wall_e_should_equal(10, wall_e::enums::from_string<test_enum>("Value4"));
    wall_e_should_equal(11, wall_e::enums::from_string<test_enum>("Value5"));
    wall_e_should_equal(12, wall_e::enums::from_string<test_enum>("Value6"));

    wall_e_should_equal(Value0, wall_e::enums::from_string<test_enum>("Value0"));
    wall_e_should_equal(Value1, wall_e::enums::from_string<test_enum>("Value1"));
    wall_e_should_equal(Value2, wall_e::enums::from_string<test_enum>("Value2"));
    wall_e_should_equal(Value3, wall_e::enums::from_string<test_enum>("Value3"));
    wall_e_should_equal(Value4, wall_e::enums::from_string<test_enum>("Value4"));
    wall_e_should_equal(Value5, wall_e::enums::from_string<test_enum>("Value5"));
    wall_e_should_equal(Value6, wall_e::enums::from_string<test_enum>("Value6"));

}

constexpr int aaa = 1;

wall_e_enum(test_enum2,
    Value0_2 = aaa,
    Value1_2,
    Value2_2,
    Value3_2,
    Value4_2 = 10,
    Value5_2,
    Value6_2
)

void wall_e::enums_spec::enum_test1() {
    /// should output warning
    wall_e::enums::reflect_names<test_enum2>();
}
