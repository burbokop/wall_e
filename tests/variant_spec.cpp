#include "variant_spec.h"

#include <sstream>
#include <iostream>

#include <wall_e/src/models/error.h>

struct some_struct {
    bool operator==(const some_struct&) const { return false; }
};

std::ostream& operator<<(std::ostream& stream, const some_struct&) {
    return stream << "some_struct {}";
}


struct some_struct2 {
    int a;
    int b;
    bool operator==(const some_struct2& o) const {
        return a == o.a && b == o.b;
    }
};

std::ostream& operator<<(std::ostream& stream, const some_struct2& s) {
    return stream << "some_struct2 "
                  << "{ a: " << s.a
                  << ", b: " << s.b
                  << " }";
}

void wall_e::variant_spec::print_to_stream_test() {
    {
        static constexpr bool has_oper = stream_operator::exists<std::ostream, std::string>::value;
        wall_e_should_equal(has_oper, true);

        wall_e::variant v = "gogadoda";
        std::stringstream ss;
        ss << v;

        wall_e_should_equal(v.type(), wall_e::type_name<std::string>());
        wall_e_should_equal(ss.str(), "gogadoda");
    } {
        static constexpr bool has_oper = stream_operator::exists<std::ostream, some_struct>::value;
        wall_e_should_equal(has_oper, true);

        wall_e::variant v = some_struct {};
        std::stringstream ss;
        ss << v;

        wall_e_should_equal(ss.str(), "some_struct {}");
    } {


        static constexpr bool has_oper = stream_operator::exists<std::ostream, some_struct2>::value;
        wall_e_should_equal(has_oper, true);

        wall_e::variant v = some_struct2 { .a = 2, .b = 3 };
        std::stringstream ss;
        ss << v;

        wall_e_should_equal(ss.str(), "some_struct2 { a: 2, b: 3 }");
    } {
        static constexpr bool has_oper = stream_operator::exists<std::ostream, error>::value;
        wall_e_should_equal(has_oper, true);

        wall_e::variant v = error("some_message", error::err, error::undefined_stage);
        std::stringstream ss;
        ss << v;

        wall_e_should_equal(ss.str(), "err:some_message at { 0, 0(stage: undefined_stage, kind: 0)");
    }
}
