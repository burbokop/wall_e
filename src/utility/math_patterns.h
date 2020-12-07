#ifndef MATH_PATTERNS_H
#define MATH_PATTERNS_H

#include <list>
#include <string>
#include "../gram.h"

namespace wall_e {

namespace gram { class pattern; }

class math_patterns {
public:
    static gram::rule add_to(std::list<gram::pattern> *patterns, const std::string &preffix);
    static gram::argument add_sub_processor(const gram::arg_vector &args);
    static gram::argument mul_div_processor(const gram::arg_vector &args);
    static gram::argument binary_int_operator(const gram::arg_vector &args, std::list<std::pair<std::string, std::function<int(int, int)>>> l);

    static bool extract_number_str(const std::string &arg, int *number);
    static bool extract_number(const gram::argument &arg, int *number);
};

}
#endif // MATH_PATTERNS_H
