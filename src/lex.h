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

struct Pattern {
    std::regex reg;
    std::string name;
};

bool operator <(const Pattern &p1, const Pattern &p2);

std::string find_repetition(const std::list<Pattern> &patternlist);

struct Token {
    std::string name;
    std::string text;
    std::string::size_type position = -1;
    uint8_t meta = 0;
};

inline std::ostream &operator << (std::ostream &stream, const Token &token) {
    stream << "{ " << token.position << ", " << token.name << ", " << token.text << " }";
    return stream;
}

std::string to_string(const Token &token);
std::string to_string(const std::vector<Token> &token, char separator = '\n');

inline std::ostream &operator << (std::ostream &stream, const std::vector<Token> &vector) {
    stream << "std::vector<wall_e::lex::token> [";
    std::vector<Token>::size_type i = 0;
    for(auto v : vector) {
        stream << v;
        if(i < vector.size() - 1) {
            stream << ", ";
        }
    }
    stream << "]";
    return stream;
}


inline bool operator <= (const Token &token1, const Token &token2) { return token1.position <= token2.position; }
inline bool operator >= (const Token &token1, const Token &token2) { return token1.position >= token2.position; }
inline bool operator < (const Token &token1, const Token &token2) { return token1.position < token2.position; }
inline bool operator > (const Token &token1, const Token &token2) { return token1.position > token2.position; }
inline bool operator == (const Token &token1, const Token &token2) { return token1.position == token2.position; }

std::vector<Token> get_tokents(std::string text, const std::list<Pattern> &patternlist);
std::vector<std::string::size_type> get_all_occurrences(const std::string &text, const std::string &substring);
std::vector<Token> sort_tokens(std::vector<Token> tokens, std::string text);

}
}
#endif // LEX_H
