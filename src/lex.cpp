#include "lex.h"

#include <cassert>

namespace wall_e {
namespace lex {

const std::string ignore = "ignore";

std::vector<std::string> match(const std::regex &reg, const std::string &text) {
    std::sregex_token_iterator it(text.begin(), text.end(), reg);
    std::sregex_token_iterator end;
    std::vector<std::string> result;
    while(it != end) {
        result.push_back(it.operator*());
        ++it;
    }
    return result;
}


void remove_substrs(std::string *text, const std::string& pattern) {
    assert(text);
    std::string::size_type n = pattern.length();
    for (std::string::size_type i = text->find(pattern); i != std::string::npos; i = text->find(pattern))
        text->erase(i, n);
}

void wipe_substrs(std::string *text, const std::string& pattern, char c) {
    assert(text);
    std::string cc;
    cc.resize(1, c);
    if(pattern == "" || pattern == cc)
        return;

    std::string::size_type n = pattern.length();
    for (std::string::size_type i = text->find(pattern); i != std::string::npos; i = text->find(pattern)) {
        text->erase(i, n);
        text->insert(i, n, c);
    }
}

std::vector<token> make_tokents(std::string text, const std::list<pattern> &patternlist) {
    std::vector<token> result;
    const auto rep = find_repetition(patternlist);
    if(rep != std::string())
        throw std::runtime_error("klex_get_tokents: repetition found (" + rep + ")");

    for(const auto& pattern : patternlist) {
        auto l = match(pattern.reg, text);
        for(const auto& ll : l) {
            remove_substrs(&text, ll);
            //klex_wipe_substrs(&text, ll);
            token token;
            token.name = pattern.name;
            token.text = ll;
            if(pattern.name.size() > 0 && pattern.name != "" && pattern.name != "ignore" && pattern.name != "null") {
                result.push_back(token);
            }
        }
    }

    for(auto c : text) {
        token errt;
        errt.name = "undefined";
        errt.undefined = true;
        errt.text = c;
        result.push_back(errt);
    }

    std::sort(result.begin(), result.end(), [](const token &a, const token &b) {
        return a.text.size() > b.text.size();
    });

    return result;
}


std::vector<std::string::size_type> find_all_occurrences(const std::string &text, const std::string &substring) {
    std::vector<std::string::size_type> result;

    size_t pos = text.find(substring, 0);
    while(pos != std::string::npos) {
        result.push_back(pos);
        pos = text.find(substring, pos + 1);
    }

    return result;
}

std::vector<token> sort_tokens(std::vector<token> tokens, std::string text) {
    std::sort(tokens.begin(), tokens.end(), [](const token &a, const token &b) { return a.text.size() > b.text.size(); });

    std::vector<token> result;
    std::map<std::string::size_type, token> tokmap;
    for(auto t : tokens) {
        auto o = find_all_occurrences(text, t.text);
        wipe_substrs(&text, t.text);
        for(auto index : o) {
            if(index >= 0) {
                t.position = index;
                tokmap[index] = t;
            }
        }
    }

    for(auto it = tokmap.begin(); it != tokmap.end(); ++it) {
        result.push_back(it->second);
    }

    std::sort(result.begin(), result.end());
    return result;
}

std::string find_repetition(const std::list<pattern> &patternlist) {
    std::map<pattern, char> m;
    for(const auto& p : patternlist) {
        auto it = m.find(p);
        if(it != m.end()) {
            return p.name;
        }
        m[p] = 0;
    }
    return std::string();
}

bool operator <(const pattern &p1, const pattern &p2) {
    return p1.name < p2.name;
}

std::string to_string(const token &token) {
    std::stringstream ss;
    ss << token;
    return ss.str();
}

std::string to_string(const std::vector<token> &token, char separator) {
    std::stringstream ss;
    for(const auto& t : token) {
        ss << t << separator;
    }
    return ss.str();
}

std::string trim(const std::string &string, char delim) {
    size_t first = string.find_first_not_of(delim);
    if (first == std::string::npos) {
        return std::string();
    }
    size_t last = string.find_last_not_of(delim);
    return string.substr(first, (last - first + 1));
}

void remove_character(std::string *text, char c) {
    if (text) {
        text->erase(std::remove(text->begin(), text->end(), c), text->end());
    }
}

std::string remove_character(std::string text, char c) {
    remove_character(&text, c);
    return text;
}

str_vec names(const std::vector<token> &tokens) {
    std::vector<std::string> result;
    result.reserve(tokens.size());
    for(const auto& t : tokens) {
        result.push_back(t.name);
    }
    return result;
}

str_vec texts(const std::vector<token> &tokens) {
    std::vector<std::string> result;
    result.reserve(tokens.size());
    for(const auto& t : tokens) {
        result.push_back(t.text);
    }
    return result;
}

std::vector<str_pair> names(const std::vector<token_pair> &tokens) {
    std::vector<str_pair> result;
    result.reserve(tokens.size());
    for(const auto& t : tokens) {
        result.push_back({ t.first.name, t.second.name });
    }
    return result;
}

std::vector<str_pair> texts(const std::vector<token_pair> &tokens) {
    std::vector<str_pair> result;
    result.reserve(tokens.size());
    for(const auto& t : tokens) {
        result.push_back({ t.first.text, t.second.text });
    }
    return result;
}

std::string parse_string_literal(std::string str, bool remove_quotes, char quotes_char) {
    if (remove_quotes && str.size() > 1) {
        if(str[0] == quotes_char && str[str.size() - 1] == quotes_char) {
            str = str.substr(1, str.size() - 2);
        }
    }
    return convert_special_symbol(str);
}

char special_symbol(char name) {
    switch (name) {
        case 'n': return '\n';
        case 't': return '\t';
    }
    return name;
}

std::string convert_special_symbol(std::string str) {
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

text_segment token::segment() const {
    if(position >= 0) {
        return text_segment(position, position + text.size());
    } else {
        return text_segment();
    }
}

std::optional<error> token::undef_error() const {
    if(undefined) {
        return error("undefined token", error::err, error::lexic, errors::undefined_token, segment());
    } else {
        return std::nullopt;
    }
}


}
}

