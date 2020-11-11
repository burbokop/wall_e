#ifndef KM2_ASM_TOOLS_H
#define KM2_ASM_TOOLS_H


#include "kgram_tools.h"

struct km2_asm_unit {
    std::string code;

    void operator+=(const km2_asm_unit& unit1);
    void operator+=(const std::string& str);
};

std::ostream &operator<< (std::ostream& stream, const km2_asm_unit& unit);
km2_asm_unit operator+ (const km2_asm_unit& unit1, const km2_asm_unit& unit2);

bool operator== (const km2_asm_unit& unit1, const km2_asm_unit& unit2);

km2_asm_unit km2_produce_asm_unit(const kgram_variant_t &variant);



#endif // KM2_ASM_TOOLS_H
