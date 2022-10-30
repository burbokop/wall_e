#ifndef KM2_TOKEN_TOOLS_H
#define KM2_TOKEN_TOOLS_H

#include "../models/variant.h"
#include "../lex.h"

namespace wall_e {

lex::token_vec produce_tokens(const wall_e::variant &variant, const str_list& names);
wall_e::variant_vector remove_tokens(const wall_e::variant_vector &variant, const str_list& tokens);

wall_e::vec<wall_e::pair<wall_e::lex::token, wall_e::lex::token>> produce_token_pairs(const wall_e::variant &variant);
lex::token_vec left_every_second(const wall_e::variant &variant);
lex::token_vec right_every_second(const wall_e::variant &variant);

wall_e::vec<wall_e::pair<wall_e::lex::token, wall_e::lex::token>> invert(const wall_e::vec<wall_e::pair<wall_e::lex::token, wall_e::lex::token>>& a);

}

#endif // KM2_TOKEN_TOOLS_H
