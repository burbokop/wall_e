#ifndef LEX_H
#define LEX_H

#include <vector>
#include <list>
#include <regex>
#include <string>

namespace wall_e {
namespace lex {

std::vector<std::string> match(const std::regex &reg, const std::string &text);
void remove_substrs(std::string *text, const std::string& pattern);
void wipe_substrs(std::string *text, const std::string& pattern, char c = ' ');

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

std::vector<token> get_tokents(std::string text, const std::list<pattern> &patternlist);
std::vector<std::string::size_type> get_all_occurrences(const std::string &text, const std::string &substring);
std::vector<token> sort_tokens(std::vector<token> tokens, std::string text);

}
}
#endif // LEX_H
