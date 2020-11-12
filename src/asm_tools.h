#ifndef KM2_ASM_TOOLS_H
#define KM2_ASM_TOOLS_H


#include "kgram_tools.h"

namespace wall_e {


struct asm_unit {
    std::string code;

    void operator+=(const asm_unit& unit1);
    void operator+=(const std::string& str);
};

std::ostream &operator<< (std::ostream& stream, const asm_unit& unit);
asm_unit operator+ (const asm_unit& unit1, const asm_unit& unit2);
bool operator== (const asm_unit& unit1, const asm_unit& unit2);

asm_unit produce_asm_unit(const wall_e::variant &variant);


}

#endif // KM2_ASM_TOOLS_H
