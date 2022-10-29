#ifndef TYPENAME_H
#define TYPENAME_H

#include <string>
#include <cxxabi.h>

namespace wall_e {

template<typename T>
std::string type_name() {
    std::string original_typename = typeid (T).name();
    size_t original_typename_size = original_typename.size();
    int status = 0;
    char *demangle_result = abi::__cxa_demangle(original_typename.c_str(), nullptr, &original_typename_size, &status);
    std::string result = demangle_result;
    if(status == 0) {
        return result;
    } else if(status == -1) {
        printf("wall_e::type: Memory allocation failiure occurred.");
    } else if(status == -2) {
        printf("wall_e::type: Mangled_name is not a valid name under the C++ ABI mangling rules.");
    } else if(status == -3) {
        printf("wall_e::type: One of the arguments is invalid.");
    }
    printf("wall_e::type: Undefined error.");
    return "";
}

}

#endif // TYPENAME_H
