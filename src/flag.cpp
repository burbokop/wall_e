#include "flag.h"


std::string wall_e::flag::data() const { return m_data; }
bool wall_e::flag::bool_data() const { return m_bool_data; }
std::pair<std::string, std::string> wall_e::flag::name() const { return m_name; }
std::string wall_e::flag::description() const { return m_description; }
wall_e::flag::flag_type wall_e::flag::type() const { return m_type; }
std::string wall_e::flag_provider::preffix() const { return m_preffix; }
void wall_e::flag_provider::set_preffix(const std::string &preffix) { m_preffix = preffix; }


std::string wall_e::flag::name_to_string(const std::pair<std::string, std::string> &v) {
    if(v.second.size() > 0) {
        return v.first + ":" + v.second;
    } else {
        return v.first;
    }
}

wall_e::flag wall_e::flag::make_value_flag(const std::pair<std::string, std::string> &name, const std::string &data, const std::string &description) {
    flag result;
    result.m_data = data;
    result.m_name = name;
    result.m_description = description;
    result.m_type = value_flag;
    return result;
}

wall_e::flag wall_e::flag::make_bool_flag(const std::pair<std::string, std::string> &name, bool data, const std::string &description) {
    flag result;
    result.m_bool_data = data;
    result.m_name = name;
    result.m_description = description;
    result.m_type = bool_flag;
    return result;
}

std::string wall_e::flag_provider::extended_preffix() const {
    return m_extended_preffix;
}

void wall_e::flag_provider::set_extended_preffix(const std::string &extended_preffix) {
    m_extended_preffix = extended_preffix;
}

std::map<std::string, wall_e::flag> wall_e::flag_provider::to_map() const {
    std::map<std::string, wall_e::flag> result;
    for(const auto &f : m_flags) {
        result.insert({ f.complex_name(), f });
    }
    return result;
}

wall_e::flag_provider::flag_provider(int argc, char **argv) {
    m_args.resize(argc);
    for(int i = 0; i < argc; ++i) {
        m_args[i] = argv[i];
    }
}

wall_e::flag &wall_e::flag_provider::bool_flag(const std::pair<std::string, std::string> &flag_name, const std::string &description) {
    const auto it0 = std::find(m_args.begin(), m_args.end(), m_preffix + flag_name.first);
    const auto it1 = std::find(m_args.begin(), m_args.end(), m_extended_preffix + flag_name.second);
    m_flags.push_back(flag::make_bool_flag(flag_name, it0 != m_args.end() || it1 != m_args.end(), description));
    return m_flags.back();
}

wall_e::flag &wall_e::flag_provider::value_flag(const std::pair<std::string, std::string> &flag_name, const std::string &description, const std::string &default_value) {
    bool found = false;
    auto it = std::find(m_args.begin(), m_args.end(), m_preffix + flag_name.first);
    if(it != m_args.end()) {
        if(++it != m_args.end()) {
            found = true;
            m_flags.push_back(flag::make_value_flag(flag_name, *it, description));
        }
    } else {
        it = std::find(m_args.begin(), m_args.end(), m_extended_preffix + flag_name.second);
        if(it != m_args.end()) {
            if(++it != m_args.end()) {
                found = true;
                m_flags.push_back(flag::make_value_flag(flag_name, *it, description));
            }
        }
    }

    if(!found) {
        m_flags.push_back(flag::make_value_flag(flag_name, default_value, description));
    }
    return m_flags.back();
}

