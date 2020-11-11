#include "lex.h"

#include <cassert>

namespace wall_e {
namespace lex {


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

std::vector<token> get_tokents(std::string text, const std::list<pattern> &patternlist) {
    std::vector<token> result;
    const auto rep = find_repetition(patternlist);
    if(rep != std::string())
        throw std::runtime_error("klex_get_tokents: repetition found (" + rep + ")");

    for(auto pattern : patternlist) {
        auto l = match(pattern.reg, text);
        for(auto ll : l) {
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
        errt.name = "error";
        errt.text = c;
        result.push_back(errt);
    }

    std::sort(result.begin(), result.end(), [](const token &a, const token &b) {
        return a.text.size() > b.text.size();
    });

    return result;
}


std::vector<std::string::size_type> get_all_occurrences(const std::string &text, const std::string &substring) {
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
        auto o = get_all_occurrences(text, t.text);
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
    for(auto p : patternlist) {
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
    for(auto t : token) {
        ss << t << separator;
    }
    return ss.str();
}

}
}
