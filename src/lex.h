#ifndef WALL_E_LEX_H
#define WALL_E_LEX_H

#include "models/error.h"
#include "utility/collections.h"

#include <vector>
#include <list>
#include <regex>
#include <string>

namespace wall_e {
namespace lex {

namespace errors {
enum { UndefinedToken };
}

std::string trim(const std::string& string, char delim = ' ');
std::string replace(std::string str, const wall_e::vec<wall_e::pair<std::string, std::string>>& mapping);
std::string encode_special_syms(const std::string& str);

void remove_character(std::string* text, char c);
std::string remove_character(std::string text, char c);

struct pattern {
    std::regex reg;
    std::string name;
};

typedef wall_e::list<pattern> pattern_list;

inline static std::regex bounded(const std::string& w) {
    return std::regex("\\b" + w + "\\b");
}

bool operator<(const pattern &p1, const pattern &p2);

std::string find_repetition(const pattern_list &pattern_list);

struct token {
    std::string uri;
    std::string name;
    std::string text;
    std::string::size_type position = -1;
    uint8_t meta = 0;
    bool undefined = false;

    text_segment segment() const;
    wall_e::opt<error> undef_error() const;
};

typedef wall_e::pair<token, token> token_pair;
typedef wall_e::vec<token> token_vec;
typedef wall_e::list<token> token_list;

inline std::ostream &operator << (std::ostream &stream, const token &token) {
    stream << "{ " << token.position
           << ", " << token.name
           << ", " << token.text
           << ", " << token.segment()
           << " }";
    return stream;
}

std::string to_string(const token &token);
std::string to_string(const token_vec &tokens, char separator = '\n');

inline bool operator <= (const token &token1, const token &token2) { return token1.position <= token2.position; }
inline bool operator >= (const token &token1, const token &token2) { return token1.position >= token2.position; }
inline bool operator < (const token &token1, const token &token2) { return token1.position < token2.position; }
inline bool operator > (const token &token1, const token &token2) { return token1.position > token2.position; }
inline bool operator == (const token &token1, const token &token2) { return token1.position == token2.position; }

namespace special {
constexpr const char* ignore = "ignore";
}

wall_e::lex::token_vec make_tokents(std::string text, const std::string& uri, const wall_e::lex::pattern_list &pattern_list, const char replacer = ' ');

template<typename T>
T split(const std::string &string, const std::regex &regex) {
    std::sregex_token_iterator p(string.begin(), string.end(), regex, -1), end;
    if constexpr(std::is_same<T, wall_e::pair<std::string, std::string>>::value) {
        wall_e::pair<std::string, std::string> result;
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


char special_symbol(char name);
std::string convert_special_symbol(std::string str);

std::string parse_string_literal(std::string str, bool remove_quotes = false, char quotes_char = '\'');

str_vec names(const token_vec& tokens);
str_vec texts(const token_vec& tokens);

wall_e::vec<str_pair> names(const wall_e::vec<lex::token_pair>& tokens);
wall_e::vec<str_pair> texts(const wall_e::vec<lex::token_pair>& tokens);

}
}


#endif // WALL_E_LEX_H
