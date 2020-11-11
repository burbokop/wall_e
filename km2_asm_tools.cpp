#include "km2_asm_tools.h"

std::ostream &operator<<(std::ostream &stream, const km2_asm_unit &unit) {
    if(std::find(unit.code.begin(), unit.code.end(), '\n') != unit.code.end()) {
        stream << "code {\n" << unit.code << "}";
    } else {
        stream << "code {" << unit.code << "}";
    }
    return stream;
}

km2_asm_unit km2_produce_asm_unit(const kgram_variant_t &variant) {
    km2_asm_unit unit;
    if(variant.contains_type<kgram_arg_vector_t>()) {
        const auto vec = variant.value<kgram_arg_vector_t>();
        for(auto v : vec) {
            unit += km2_produce_asm_unit(v);
        }
    } else if(variant.contains_type<km2_asm_unit>()) {
        return variant.value<km2_asm_unit>();
    }
    return unit;
}

km2_asm_unit operator+(const km2_asm_unit &unit1, const km2_asm_unit &unit2) {
    return km2_asm_unit { unit1.code + unit2.code };
}

void km2_asm_unit::operator+=(const km2_asm_unit &unit1) {
    code += unit1.code;
}

void km2_asm_unit::operator+=(const std::string &str) {
    code += str;
}

bool operator==(const km2_asm_unit &unit1, const km2_asm_unit &unit2) {
    return unit1.code == unit2.code;
}
