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
enum  { undefined_token };
}

extern const std::string ignore;

wall_e::vec<std::string> match(const std::regex &reg, const std::string &text);
void remove_substrs(std::string *text, const std::string& pattern);
void wipe_substrs(std::string *text, const std::string& pattern, char c = ' ');
std::string trim(const std::string& string, char delim = ' ');
std::string encode_special_syms(std::string str);

template<template<typename, typename> typename C>
std::string join(
        const C<std::string, std::allocator<std::string>>& collection,
        const std::string& separator,
        const std::function<std::string(const std::string&)>& mapper = nullptr
        ) {
    std::string result;
    for(const auto& str : collection)
        result += (mapper ? mapper(str) : str) + separator;

    return result.substr(0, result.size() - separator.size());
}

void remove_character(std::string* text, char c);
std::string remove_character(std::string text, char c);

struct pattern {
    std::regex reg;
    std::string name;
};

typedef wall_e::list<pattern> pattern_list;

bool operator <(const pattern &p1, const pattern &p2);

std::string find_repetition(const pattern_list &patternlist);

struct token {
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

token_vec make_tokents(std::string text, const pattern_list &patternlist);
wall_e::vec<std::string::size_type> find_all_occurrences(const std::string &text, const std::string &substring);
token_vec sort_tokens(token_vec tokens, std::string text);

inline token_vec parse(const std::string &text, const pattern_list &patternlist) {
    return wall_e::lex::sort_tokens(wall_e::lex::make_tokents(text, patternlist), text);
}


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
