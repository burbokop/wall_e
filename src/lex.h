#ifndef LEX_H
#define LEX_H

#include <vector>
#include <list>
#include <regex>
#include <string>

namespace wall_e {
namespace lex {

extern const std::string ignore;

std::vector<std::string> match(const std::regex &reg, const std::string &text);
void remove_substrs(std::string *text, const std::string& pattern);
void wipe_substrs(std::string *text, const std::string& pattern, char c = ' ');
std::string trim(const std::string& string, char delim = ' ');

struct pattern {
    std::regex reg;
    std::string name;
};

bool operator <(const pattern &p1, const pattern &p2);

std::string find_repetition(const std::list<pattern> &patternlist);

struct token {
    std::string name;
    std::string text;
    std::string::size_type position = -1;
    uint8_t meta = 0;
};

inline std::ostream &operator << (std::ostream &stream, const token &token) {
    stream << "{ " << token.position << ", " << token.name << ", " << token.text << " }";
    return stream;
}

std::string to_string(const token &token);
std::string to_string(const std::vector<token> &token, char separator = '\n');

inline std::ostream &operator << (std::ostream &stream, const std::vector<token> &vector) {
    stream << "std::vector<wall_e::lex::token> [";
    std::vector<token>::size_type i = 0;
    for(auto v : vector) {
        stream << v;
        if(i < vector.size() - 1) {
            stream << ", ";
        }
    }
    stream << "]";
    return stream;
}


inline bool operator <= (const token &token1, const token &token2) { return token1.position <= token2.position; }
inline bool operator >= (const token &token1, const token &token2) { return token1.position >= token2.position; }
inline bool operator < (const token &token1, const token &token2) { return token1.position < token2.position; }
inline bool operator > (const token &token1, const token &token2) { return token1.position > token2.position; }
inline bool operator == (const token &token1, const token &token2) { return token1.position == token2.position; }

std::vector<token> make_tokents(std::string text, const std::list<pattern> &patternlist);
std::vector<std::string::size_type> find_all_occurrences(const std::string &text, const std::string &substring);
std::vector<token> sort_tokens(std::vector<token> tokens, std::string text);

inline std::vector<token> parse(const std::string &text, const std::list<pattern> &patternlist) {
    return wall_e::lex::sort_tokens(wall_e::lex::make_tokents(text, patternlist), text);
}

typedef std::pair<std::string, std::string> str_pair;

template<typename T>
T split(const std::string &string, const std::regex &regex) {
    std::sregex_token_iterator p(string.begin(), string.end(), regex, -1), end;
    if constexpr(std::is_same<T, std::pair<std::string, std::string>>::value) {
        std::pair<std::string, std::string> result;
        if(p != end)
            result.first = *p++;

        if(p != end)
            result.second = *p++;
        return result;
    } else {
        T result;
        while (p != end) {
            result.push_back(*p++);
        }
        return result;
    }
}



}
}


#endif // LEX_H
