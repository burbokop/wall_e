#ifndef MATH_PATTERNS_H
#define MATH_PATTERNS_H

#include <list>
#include <string>
#include "kgram.h"

class kgram_pattern_t;

class math_patterns {
public:
    static kgram_rule_t add_to(std::list<kgram_pattern_t> *patterns, const std::string &preffix);
    static kgram_argument_t add_sub_processor(const kgram_arg_vector_t &args);
    static kgram_argument_t mul_div_processor(const kgram_arg_vector_t &args);
    static kgram_argument_t binary_int_operator(const kgram_arg_vector_t &args, std::list<std::pair<std::string, std::function<int(int, int)>>> l);

    static bool extract_number_str(const std::string &arg, int *number);
    static bool extract_number(const kgram_argument_t &arg, int *number);
};

#endif // MATH_PATTERNS_H
