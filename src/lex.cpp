#include "lex.h"

#include <cassert>
#include <sstream>

std::string wall_e::lex::find_repetition(const pattern_list &pattern_list) {
    std::map<pattern, char> m;
    for(const auto& p : pattern_list) {
        if(p.name == special::ignore) continue;
        auto it = m.find(p);
        if(it != m.end()) {
            return p.name;
        }
        m[p] = 0;
    }
    return std::string();
}

bool wall_e::lex::operator <(const pattern &p1, const pattern &p2) {
    return p1.name < p2.name;
}

std::string wall_e::lex::to_string(const token &token) {
    std::stringstream ss;
    ss << token;
    return ss.str();
}

std::string wall_e::lex::to_string(const wall_e::vec<wall_e::lex::token> &token, char separator) {
    std::stringstream ss;
    for(const auto& t : token) {
        ss << t << separator;
    }
    return ss.str();
}

std::string wall_e::lex::trim(const std::string &string, char delim) {
    size_t first = string.find_first_not_of(delim);
    if (first == std::string::npos) {
        return std::string();
    }
    size_t last = string.find_last_not_of(delim);
    return string.substr(first, (last - first + 1));
}

void wall_e::lex::remove_character(std::string *text, char c) {
    if (text) {
        text->erase(std::remove(text->begin(), text->end(), c), text->end());
    }
}

std::string wall_e::lex::remove_character(std::string text, char c) {
    remove_character(&text, c);
    return text;
}

wall_e::str_vec wall_e::lex::names(const token_vec &tokens) {
    str_vec result;
    result.reserve(tokens.size());
    for(const auto& t : tokens) {
        result.push_back(t.name);
    }
    return result;
}

wall_e::str_vec wall_e::lex::texts(const token_vec &tokens) {
    str_vec result;
    result.reserve(tokens.size());
    for(const auto& t : tokens) {
        result.push_back(t.text);
    }
    return result;
}

wall_e::vec<wall_e::str_pair> wall_e::lex::names(const wall_e::vec<token_pair> &tokens) {
    wall_e::vec<str_pair> result;
    result.reserve(tokens.size());
    for(const auto& t : tokens) {
        result.push_back({ t.first.name, t.second.name });
    }
    return result;
}

wall_e::vec<wall_e::str_pair> wall_e::lex::texts(const wall_e::vec<token_pair> &tokens) {
    wall_e::vec<str_pair> result;
    result.reserve(tokens.size());
    for(const auto& t : tokens) {
        result.push_back({ t.first.text, t.second.text });
    }
    return result;
}

std::string wall_e::lex::parse_string_literal(std::string str, bool remove_quotes, char quotes_char) {
    if (remove_quotes && str.size() > 1) {
        if(str[0] == quotes_char && str[str.size() - 1] == quotes_char) {
            str = str.substr(1, str.size() - 2);
        }
    }
    return convert_special_symbol(str);
}


std::string wall_e::lex::convert_special_symbol(std::string str) {
    const auto&& special_symbol = [](char name) -> char {
        switch (name) {
            case 'n': return '\n';
            case 't': return '\t';
        }
        return name;
    };

    for(auto it = str.begin(); it != str.end(); ++it) {
        if(*it == '\\') {
            const auto next = (it + 1);
            if(it != str.end() && next != str.end()) {
                *it = special_symbol(*next);
                it = str.erase(next) - 1;
            }
        }
    }
    return str;
}

wall_e::text_segment wall_e::lex::token::segment() const {
    if(position >= 0) {
        return text_segment(uri, position, position + text.size());
    } else {
        return text_segment();
    }
}

wall_e::opt<wall_e::error> wall_e::lex::token::undef_error() const {
    if(undefined) {
        return error("undefined token", error::Err, error::Lexic, errors::UndefinedToken, segment());
    } else {
        return std::nullopt;
    }
}

std::string wall_e::lex::replace(std::string str, const wall_e::vec<wall_e::pair<std::string, std::string> > &mapping) {
    const auto&& replace_all = [](std::string& data, const std::string& to_search, const std::string& replace_str) {
        size_t pos = data.find(to_search);
        while(pos != std::string::npos) {
            data.replace(pos, to_search.size(), replace_str);
            pos = data.find(to_search, pos + replace_str.size());
        }
    };

    for(const auto& m : mapping) {
        replace_all(str, m.first, m.second);
    }
    return str;
}

std::string wall_e::lex::encode_special_syms(const std::string &str) {
    return replace(str, {
                       { "\n", "\\n" },
                       { "\t", "\\t" }
                   });
}


wall_e::lex::token_vec wall_e::lex::make_tokents(std::string text, const std::string& uri, const pattern_list &pattern_list, const char replacer) {
    token_vec result;
    const auto rep = find_repetition(pattern_list);
    if(rep != std::string())
        throw std::runtime_error("klex_get_tokents: repetition found (" + rep + ")");

    for(const auto& pattern : pattern_list) {
        std::sregex_token_iterator it(text.begin(), text.end(), pattern.reg);
        std::sregex_token_iterator end;
        while(it != end) {
            if(pattern.name.size() > 0 && pattern.name != special::ignore) {
                wall_e::lex::token token;
                token.uri = uri;
                token.name = pattern.name;
                token.text = *it;
                token.position = it->first - text.begin();
                result.push_back(token);
            }
            text.replace(it->first, it->second, std::string(it->length(), replacer));
            ++it;
        }
    }
    std::sort(result.begin(), result.end(), [](const token& a, const token& b){
        return a.position < b.position;
    });
    return result;
}

