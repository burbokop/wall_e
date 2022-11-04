#ifndef TYPENAME_H
#define TYPENAME_H

#include <string>
#include <cxxabi.h>

namespace wall_e {

std::string demangle(const std::string& mangled);

template<typename T>
std::string type_name() { return demangle(typeid (T).name()); }

}

#endif // TYPENAME_H
