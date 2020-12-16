#ifndef FLAG_H
#define FLAG_H

#include <vector>
#include <string>
#include <sstream>
#include <list>
#include <map>

namespace wall_e {

class flag_provider;
class flag {
    friend flag_provider;
    std::string m_data;
    std::string m_description;
    bool m_bool_data;
public:
    typedef std::pair<char, std::string> full_name;
    enum flag_type {
        bool_flag,
        value_flag,
        undefined
    };
private:
    full_name m_name;
    flag_type m_type = undefined;
    static std::string name_to_string(const full_name& v);
    static flag make_value_flag(const full_name &name, const std::string& data, const std::string &description);
    static flag make_bool_flag(const full_name &name, bool data, const std::string &description);
public:
    flag() {}

    template<typename T>
    friend inline flag &operator >>(flag &f, T& value) {
        std::istringstream(f.m_data) >> value;
        return f;
    }
    friend inline flag &operator >>(flag &f, bool& value) {
        if(f.m_type == wall_e::flag::flag_type::bool_flag) {
            value = f.m_bool_data;
        } else {
            std::istringstream(f.m_data) >> value;
        }
        return f;
    }

    friend inline std::ostream& operator<<(std::ostream& stream, const flag& f) {
        if(f.m_type == flag::bool_flag) {
            stream << name_to_string(f.m_name) << ": " << (f.m_bool_data ? "<present>" : "<missing>");
        } else if(f.m_type == flag::value_flag) {
            stream << name_to_string(f.m_name) << ": [" << f.m_data << "]";
        } else {
            stream << "<undefined>";
        }
        return stream;
    }

    full_name name() const;
    std::string complex_name() const { return std::string(1, m_name.first) + ":" + m_name.second; }
    std::string description() const;
    flag_type type() const;
    std::string data() const;
    bool bool_data() const;
};


class flag_provider {
    std::vector<std::string> m_args;
    std::list<flag> m_flags;
    std::string m_preffix = "-";
    std::string m_extended_preffix = "--";
    flag m_help;
public:
    flag_provider(int argc, char **argv);
    flag &bool_flag(const flag::full_name &flag_name, const std::string &description);
    flag &value_flag(const flag::full_name &flag_name, const std::string &description, const std::string &default_value = std::string());
    inline flag &bool_flag(char flag_name, const std::string &description) { return bool_flag({ flag_name, std::string() }, description); }
    inline flag &value_flag(char flag_name, const std::string &description, const std::string &default_value = std::string()) { return value_flag({ flag_name, std::string() }, description, default_value); }

    std::string preffix() const;
    void set_preffix(const std::string &preffix);
    std::string extended_preffix() const;
    void set_extended_preffix(const std::string &extended_preffix);

    std::map<std::string, flag> to_map() const;

    template<typename T>
    inline T to_container() const {
        if constexpr(std::is_same<T, decltype (m_flags)>::value) {
            return m_flags;
        } else {
            return T(m_flags.begin(), m_flags.end());
        }
    }

    inline decltype (m_flags) to_list() const { return to_container<decltype (m_flags)>(); }

    void finish(std::ostream &stream);
    void print_description(std::ostream &stream);
};

}

#endif // FLAG_H
