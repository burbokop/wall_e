#include "variant.h"
#include "../lex.h"


namespace wall_e {

std::ostream &operator<<(std::ostream &stream, const variant &arg) {
    if(arg.single_print()) {
        stream << "single print";
        return stream;
    }

    auto s = arg.to_string();
    if(s.size() > 0) {
        stream << s;
    } else {
        stream << "type { " << arg.type() << " }";
    }
    return stream;
}


variant_vector constrain_variant(const variant &variant) {
    variant_vector result;
    if(variant.contains_type<variant_vector>()) {
        const auto vec = variant.value<variant_vector>();
        for(const auto& v : vec) {
            auto c = constrain_variant(v);
            for(const auto& item : c) {
                result.push_back(item);
            }
        }
    } else if(variant.contains_type<wall_e::variant>()) {
        return { variant.value<wall_e::variant>() };
    } else {
        return { variant };
    }
    return result;
}

bool operator==(const variant &varian0, const variant &varian1) {
    if(varian0.type() != varian1.type())
        return false;

    if(varian0.m_data == nullptr && varian1.m_data == nullptr)
        return true;

    if(varian0.m_data == nullptr || varian1.m_data == nullptr)
        return false;

    if(!varian0.m_comparator)
        return false;

    return varian0.m_comparator(varian0.m_data, varian1.m_data);
}

double to_double(const wall_e::variant &variant, bool *ok) {
    if(ok)
        *ok = true;

    if(variant.contains_type<bool>()) {
        return variant.value<bool>();
    } else if(variant.contains_type<char>()) {
        return variant.value<char>();
    } else if(variant.contains_type<signed char>()) {
        return variant.value<signed char>();
    } else if(variant.contains_type<unsigned char>()) {
        return variant.value<unsigned char>();
    } else if(variant.contains_type<wchar_t>()) {
        return variant.value<wchar_t>();
    } else if(variant.contains_type<char16_t>()) {
        return variant.value<char16_t>();
    } else if(variant.contains_type<char32_t>()) {
        return variant.value<char32_t>();
    } else if(variant.contains_type<short>()) {
        return variant.value<short>();
    } else if(variant.contains_type<unsigned short>()) {
        return variant.value<unsigned short>();
    } else if(variant.contains_type<unsigned int>()) {
        return variant.value<unsigned int>();
    } else if(variant.contains_type<unsigned long>()) {
        return variant.value<unsigned long>();
    } else if(variant.contains_type<long long>()) {
        return variant.value<long long>();
    } else if(variant.contains_type<unsigned long long>()) {
        return variant.value<unsigned long long>();
    } else if(variant.contains_type<float>()) {
        return variant.value<float>();
    } else if(variant.contains_type<double>()) {
        return variant.value<double>();
    } else if(variant.contains_type<long double>()) {
        return variant.value<long double>();
    } else if(variant.contains_type<std::string>()) {
        try {
            return std::stod(variant.value<std::string>());
        } catch (std::invalid_argument) {
            if(ok)
                *ok = false;
            return 0;
        }
    } else if(variant.contains_type<wall_e::lex::token>()) {
        try {
            return std::stod(variant.value<wall_e::lex::token>().text);
        } catch (std::invalid_argument) {
            if(ok)
                *ok = false;
            return 0;
        }
    }

    if(ok)
        *ok = false;
    return 0;
}

bool is_number(const variant &variant, std::string expected_token) {
    if(variant.contains_type<bool>()
    || variant.contains_type<char>()
    || variant.contains_type<signed char>()
    || variant.contains_type<unsigned char>()
    || variant.contains_type<wchar_t>()
    || variant.contains_type<char16_t>()
    || variant.contains_type<char32_t>()
    || variant.contains_type<short>()
    || variant.contains_type<unsigned short>()
    || variant.contains_type<unsigned int>()
    || variant.contains_type<unsigned long>()
    || variant.contains_type<long long>()
    || variant.contains_type<unsigned long long>()
    || variant.contains_type<float>()
    || variant.contains_type<double>()
    || variant.contains_type<long double>()) {
        return true;
    } else if(variant.contains_type<std::string>()) {
        return is_number(variant.value<std::string>());
    } else if(variant.contains_type<wall_e::lex::token>()) {
        if(expected_token.size() > 0) {
            return expected_token == variant.value<wall_e::lex::token>().name;
        } else {
            return is_number(variant.value<wall_e::lex::token>().text);
        }
    }
    return 0;
}

bool is_number(const std::string &string) {
    std::string::const_iterator it = string.begin();
    while (it != string.end() && std::isdigit(*it)) ++it;
    return !string.empty() && it == string.end();
}

}
