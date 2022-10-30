#include "enums.h"

#include <sstream>
#include <iostream>

void wall_e::enums::__private_tools::print_err(const std::string &str) {
    std::cerr << str << std::endl;
}

std::string wall_e::enums::__private_tools::trim(const std::string &str) {
    std::size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) {
        return str;
    }
    std::size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

wall_e::vec<std::string> wall_e::enums::__private_tools::split(const std::string &s, char delimiter) {
    wall_e::vec<std::string> tokens;
    std::string token;
    std::istringstream token_stream(s);
    while (std::getline(token_stream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}
