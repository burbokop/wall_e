#include "asm_tools.h"

namespace wall_e {

std::ostream &operator<<(std::ostream &stream, const asm_unit &unit) {
    if(std::find(unit.code.begin(), unit.code.end(), '\n') != unit.code.end()) {
        stream << "code {\n" << unit.code << "}";
    } else {
        stream << "code {" << unit.code << "}";
    }
    return stream;
}

asm_unit produce_asm_unit(const wall_e::variant &variant) {
    asm_unit unit;
    if(variant.contains_type<gram::arg_vector>()) {
        const auto vec = variant.value<gram::arg_vector>();
        for(auto v : vec) {
            unit += produce_asm_unit(v);
        }
    } else if(variant.contains_type<asm_unit>()) {
        return variant.value<asm_unit>();
    }
    return unit;
}

asm_unit operator+(const asm_unit &unit1, const asm_unit &unit2) {
    return asm_unit { unit1.code + unit2.code };
}

void asm_unit::operator+=(const asm_unit &unit1) {
    code += unit1.code;
}

void asm_unit::operator+=(const std::string &str) {
    code += str;
}

bool operator==(const asm_unit &unit1, const asm_unit &unit2) {
    return unit1.code == unit2.code;
}
}
