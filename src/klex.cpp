#include "klex.h"

#include <cassert>

std::vector<std::string> klex_get_reg(const std::regex &reg, const std::string &text) {
    std::sregex_token_iterator it(text.begin(), text.end(), reg);
    std::sregex_token_iterator end;
    std::vector<std::string> result;
    while(it != end) {
        result.push_back(it.operator*());
        ++it;
    }
    return result;
}


void klex_remove_substrs(std::string *text, const std::string& pattern) {
    assert(text);
    std::string::size_type n = pattern.length();
    for (std::string::size_type i = text->find(pattern); i != std::string::npos; i = text->find(pattern))
        text->erase(i, n);
}

void klex_wipe_substrs(std::string *text, const std::string& pattern, char c) {
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

std::vector<klex_token_t> klex_get_tokents(std::string text, const std::list<klex_patern_t> &paternlist) {
    std::vector<klex_token_t> result;
    const auto rep = klex_find_repetition(paternlist);
    if(rep != std::string())
        throw std::runtime_error("klex_get_tokents: repetition found (" + rep + ")");

    for(auto patern : paternlist) {
        auto l = klex_get_reg(patern.reg, text);
        for(auto ll : l) {
            klex_remove_substrs(&text, ll);
            //klex_wipe_substrs(&text, ll);
            klex_token_t token;
            token.name = patern.name;
            token.text = ll;
            if(patern.name.size() > 0 && patern.name != "" && patern.name != "ignore" && patern.name != "null") {
                result.push_back(token);
            }
        }
    }

    for(auto c : text) {
        klex_token_t errt;
        errt.name = "error";
        errt.text = c;
        result.push_back(errt);
    }

    std::sort(result.begin(), result.end(), [](const klex_token_t &a, const klex_token_t &b) {
        return a.text.size() > b.text.size();
    });

    return result;
}


std::vector<std::string::size_type> klex_get_all_occurrences(const std::string &text, const std::string &substring) {
    std::vector<std::string::size_type> result;

    size_t pos = text.find(substring, 0);
    while(pos != std::string::npos) {
        result.push_back(pos);
        pos = text.find(substring, pos + 1);
    }

    return result;
}

std::vector<klex_token_t> klex_sort_tokens(std::vector<klex_token_t> tokens, std::string text) {
    std::sort(tokens.begin(), tokens.end(), [](const klex_token_t &a, const klex_token_t &b) { return a.text.size() > b.text.size(); });

    std::vector<klex_token_t> result;
    std::map<std::string::size_type, klex_token_t> tokmap;
    for(auto t : tokens) {
        auto o = klex_get_all_occurrences(text, t.text);
        klex_wipe_substrs(&text, t.text);
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

std::string klex_find_repetition(const std::list<klex_patern_t> &patternlist) {
    std::map<klex_patern_t, char> m;
    for(auto p : patternlist) {
        auto it = m.find(p);
        if(it != m.end()) {
            return p.name;
        }
        m[p] = 0;
    }
    return std::string();
}

bool operator <(const klex_patern_t &p1, const klex_patern_t &p2) {
    return p1.name < p2.name;
}

std::string klex_to_string(const klex_token_t &token) {
    std::stringstream ss;
    ss << token;
    return ss.str();
}

std::string klex_to_string(const std::vector<klex_token_t> &token, char separator) {
    std::stringstream ss;
    for(auto t : token) {
        ss << t << separator;
    }
    return ss.str();
}
