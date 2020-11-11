#ifndef KLEX_H
#define KLEX_H

#include <vector>
#include <list>
#include <regex>
#include <string>

std::vector<std::string> klex_get_reg(const std::regex &reg, const std::string &text);
void klex_remove_substrs(std::string *text, const std::string& pattern);
void klex_wipe_substrs(std::string *text, const std::string& pattern, char c = ' ');

struct klex_patern_t {
    std::regex reg;
    std::string name;
};

bool operator <(const klex_patern_t &p1, const klex_patern_t &p2);

std::string klex_find_repetition(const std::list<klex_patern_t> &patternlist);

struct klex_token_t {
    std::string name;
    std::string text;
    std::string::size_type position = -1;
    uint8_t meta = 0;
};

inline std::ostream &operator << (std::ostream &stream, const klex_token_t &token) {
    stream << "{ " << token.position << ", " << token.name << ", " << token.text << " }";
    return stream;
}

std::string klex_to_string(const klex_token_t &token);
std::string klex_to_string(const std::vector<klex_token_t> &token, char separator = '\n');

inline std::ostream &operator << (std::ostream &stream, const std::vector<klex_token_t> &vector) {
    stream << "std::vector<klex_token_t> [";
    std::vector<klex_token_t>::size_type i = 0;
    for(auto v : vector) {
        stream << v;
        if(i < vector.size() - 1) {
            stream << ", ";
        }
    }
    stream << "]";
    return stream;
}


inline bool operator <= (const klex_token_t &token1, const klex_token_t &token2) { return token1.position <= token2.position; }
inline bool operator >= (const klex_token_t &token1, const klex_token_t &token2) { return token1.position >= token2.position; }
inline bool operator < (const klex_token_t &token1, const klex_token_t &token2) { return token1.position < token2.position; }
inline bool operator > (const klex_token_t &token1, const klex_token_t &token2) { return token1.position > token2.position; }
inline bool operator == (const klex_token_t &token1, const klex_token_t &token2) { return token1.position == token2.position; }

std::vector<klex_token_t> klex_get_tokents(std::string text, const std::list<klex_patern_t> &paternlist);
std::vector<std::string::size_type> klex_get_all_occurrences(const std::string &text, const std::string &substring);
std::vector<klex_token_t> klex_sort_tokens(std::vector<klex_token_t> tokens, std::string text);


#endif // KLEX_H
