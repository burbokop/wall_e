#ifndef KM2_TOKEN_TOOLS_H
#define KM2_TOKEN_TOOLS_H

#include "variant.h"
#include "lex.h"

namespace wall_e {

std::vector<wall_e::lex::token> produce_tokens(const wall_e::variant &variant, const std::list<std::string> names);
wall_e::variant_vector remove_tokens(const wall_e::variant_vector &variant, const std::list<std::string> tokens);
std::vector<std::pair<wall_e::lex::token, wall_e::lex::token>> produce_token_pairs(const wall_e::variant &variant);
std::ostream &operator<< (std::ostream& stream, const std::vector<std::pair<wall_e::lex::token, wall_e::lex::token>>& vector);


}

#endif // KM2_TOKEN_TOOLS_H
