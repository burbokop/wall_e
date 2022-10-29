#ifndef WALL_E_ENUMS_H
#define WALL_E_ENUMS_H

#include <vector>
#include <map>
#include "utility/typename.h"

namespace wall_e {

namespace enums {

class __private_tools {
    static void print_err(const std::string& str);
public:
    static std::string trim(const std::string& str);
    static std::vector<std::string> split(const std::string& s, char delimiter);

    template<typename T>
    static const std::map<T, std::string>* va_args_to_map(const std::string &str) {
        auto vec = split(str, ',');
        auto result = new std::map<T, std::string>();
        std::size_t val = 0;
        for(size_t i = 0, count = vec.size(); i < count; ++i) {
            const auto& pair = split(vec[i], '=');
            if(pair.size() > 0) {
                if(pair.size() > 1) {
                    const auto second = trim(pair[1]);
                    try { val = std::stoi(second); } catch (std::exception exception) {
                        print_err("token at right of enumeration must be int decimal number but found: '" + second + "' (enum: " + wall_e::type_name<T>() + ")");
                    }
                }
                result->insert({ static_cast<T>(val), trim(pair[0]) });
            } else {
                result->insert({ static_cast<T>(val), {} });
            }
            ++val;
        }
        return result;
    }
};

template<typename T>
struct reflect {
    const std::map<T, std::string>* names = nullptr;
    T max_value;
};

template<typename T>
struct tag {};

template<typename T>
inline const std::map<T, std::string>& reflect_names() {
    reflect<T> r;
    r << tag<T>();
    return *r.names;
}

template<typename T>
inline T max_value() {
    reflect<T> r;
    r << tag<T>();
    return r.max_value;
}

template<typename T>
inline std::string to_string(T value) {
    const auto& map = reflect_names<T>();
    const auto& it = map.find(value);
    return it != map.end() ? it->second : std::string();
}

template<typename T>
inline std::optional<T> from_string(const std::string& str) {
    for(const auto& p : reflect_names<T>()) {
        if(p.second == str) { return p.first; }
    }
    return std::nullopt;
}

}

}

#define wall_e_enum(TYPE, ...) \
    enum TYPE { __VA_ARGS__, __ ## TYPE ## _max_value }; \
    inline wall_e::enums::reflect<TYPE>& operator<<(wall_e::enums::reflect<TYPE>& refl, wall_e::enums::tag<TYPE>) { \
        static const auto res = wall_e::enums::__private_tools::va_args_to_map<TYPE>(#__VA_ARGS__); \
        refl.names = res; \
        refl.max_value = __ ## TYPE ## _max_value; \
        return refl; \
    } \
    inline std::ostream& operator<<(std::ostream& stream, TYPE value) { return stream << wall_e::enums::to_string(value); }

#define wall_e_enum_member(TYPE, ...) \
    public: \
        enum TYPE { __VA_ARGS__, __ ## TYPE ## _max_value }; \
        friend inline wall_e::enums::reflect<TYPE>& operator<<(wall_e::enums::reflect<TYPE>& refl, wall_e::enums::tag<TYPE>) { \
            static const auto res = wall_e::enums::__private_tools::va_args_to_map<TYPE>(#__VA_ARGS__); \
            refl.names = res; \
            refl.max_value = __ ## TYPE ## _max_value; \
            return refl; \
        } \
        friend inline std::ostream& operator<<(std::ostream& stream, TYPE value) { return stream << wall_e::enums::to_string(value); } \
    private:


#endif // WALL_E_ENUMS_H
