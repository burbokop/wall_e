#ifndef KM2_TOKEN_TOOLS_H
#define KM2_TOKEN_TOOLS_H

#include "variant.h"
#include "lex.h"

std::vector<wall_e::lex::Token> km2_produce_tokens(const wall_e::variant &variant, const std::list<std::string> names);
wall_e::variant_vector km2_remove_tokens(const wall_e::variant_vector &variant, const std::list<std::string> tokens);
std::vector<std::pair<wall_e::lex::Token, wall_e::lex::Token>> km2_produce_token_pairs(const wall_e::variant &variant);

std::ostream &operator<< (std::ostream& stream, const std::vector<std::pair<wall_e::lex::Token, wall_e::lex::Token>>& vector);



#endif // KM2_TOKEN_TOOLS_H
