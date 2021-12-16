#include "flag.h"
#include <algorithm>


std::string wall_e::flag::data() const { return m_data; }
bool wall_e::flag::bool_data() const { return m_bool_data; }
wall_e::flag::full_name wall_e::flag::name() const { return m_name; }
std::string wall_e::flag::description() const { return m_description; }
wall_e::flag::flag_type wall_e::flag::type() const { return m_type; }
std::string wall_e::flag_provider::preffix() const { return m_preffix; }
void wall_e::flag_provider::set_preffix(const std::string &preffix) { m_preffix = preffix; }


std::string wall_e::flag::name_to_string(const full_name &v) {
    if(v.second.size() > 0) {
        return std::string(1, v.first) + ":" + v.second;
    } else {
        return std::string(1, v.first);
    }
}

wall_e::flag wall_e::flag::make_value_flag(const full_name &name, const std::string &data, const std::string &description) {
    flag result;
    result.m_data = data;
    result.m_name = name;
    result.m_description = description;
    result.m_type = value_flag;
    return result;
}

wall_e::flag wall_e::flag::make_bool_flag(const full_name &name, bool data, const std::string &description) {
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

void wall_e::flag_provider::finish(std::ostream& stream) {
    bool display_help;
    m_help >> display_help;
    if(display_help) {
        print_description(stream);
    }
}

void wall_e::flag_provider::print_description(std::ostream &stream) {
    size_t max = 0;
    for(const auto &f : m_flags) {
        const auto current = f.name().second.size();
        if(current > max) {
            max = current;
        }
    }

    const char* tab = "  ";
    for(const auto &f : m_flags) {
        const auto n = f.name();
        stream << tab << '-' << n.first << tab;
        const auto delta = max - n.second.size();
        if(n.second.size() > 0) {
            stream << "--" << n.second;
        } else {
            stream << "  ";
        }
        stream << std::string(delta, ' ');
        stream << tab << f.description() << "\n";
    }
}

wall_e::flag_provider::flag_provider(int argc, char **argv) {
    m_args.resize(argc);
    for(int i = 0; i < argc; ++i) {
        m_args[i] = argv[i];
    }
    m_help = bool_flag(std::pair { 'h', "help" }, "Display halp");
}

wall_e::flag &wall_e::flag_provider::bool_flag(const flag::full_name &flag_name, const std::string &description) {
    const auto it0 = std::find(m_args.begin(), m_args.end(), m_preffix + flag_name.first);
    const auto it1 = std::find(m_args.begin(), m_args.end(), m_extended_preffix + flag_name.second);
    m_flags.push_back(flag::make_bool_flag(flag_name, it0 != m_args.end() || it1 != m_args.end(), description));
    return m_flags.back();
}

wall_e::flag &wall_e::flag_provider::value_flag(const flag::full_name &flag_name, const std::string &description, const std::string &default_value) {
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

std::ostream& operator << (std::ostream& stream, const wall_e::flag_provider& flag_provider) {
    std::vector<std::string> args = flag_provider.args();
    const char* const delim = " ";
    std::copy(args.begin(), args.end(),
               std::ostream_iterator<std::string>(stream, delim));

    return stream;
}

