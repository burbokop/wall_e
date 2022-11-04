#include "typename.h"

std::string wall_e::demangle(const std::string &mangled) {
    size_t mangled_size = mangled.size();
    int status = 0;
    const char *demangle_result = abi::__cxa_demangle(mangled.c_str(), nullptr, &mangled_size, &status);
    std::string result = demangle_result;
    if(status == 0) {
        return result;
    } else if(status == -1) {
        printf("wall_e::demangle: Memory allocation failiure occurred.");
    } else if(status == -2) {
        printf("wall_e::demangle: Mangled_name is not a valid name under the C++ ABI mangling rules.");
    } else if(status == -3) {
        printf("wall_e::demangle: One of the arguments is invalid.");
    }
    printf("wall_e::demangle: Undefined error.");
    return {};
}
