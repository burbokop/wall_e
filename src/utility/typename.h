#ifndef WALL_E_TYPENAME_H
#define WALL_E_TYPENAME_H

#include <string>
#include <typeinfo>

namespace wall_e {

std::string demangle(const std::string& mangled);

template<typename T>
std::string type_name() { return demangle(typeid (T).name()); }

}

#endif // WALL_E_TYPENAME_H
