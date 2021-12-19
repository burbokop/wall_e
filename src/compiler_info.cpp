#include "compiler_info.h"

#include <sstream>

const wall_e::compiler_info wall_e::cxx_info = WALL_E_COMPILER_INFO;

std::ostream& wall_e::operator<<(std::ostream& stream, const compiler_info& cinfo) {
    return stream << cinfo.to_string();
}

bool wall_e::compiler_info::is_gcc() const {
    return compiler_name == "g++" || compiler_name == "gcc";
}

bool wall_e::compiler_info::is_clang() const {
    return compiler_name == "clang++" || compiler_name == "clang";
}

std::string wall_e::compiler_info::version_str() const {
    std::ostringstream ss;
    ss << version_major << '.' << version_minor << '.' << patchlevel;
    return ss.str();
}

std::string wall_e::compiler_info::to_string() const {
    return compiler_name + "-" + version_str();
}
