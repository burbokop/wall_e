#ifndef KM2_TOKEN_TOOLS_H
#define KM2_TOKEN_TOOLS_H

#include "kgramvariant.h"
#include "klex.h"

std::vector<klex_token_t> km2_produce_tokens(const kgram_variant_t &variant, const std::list<std::string> names);
kgram_variant_vector km2_remove_tokens(const kgram_variant_vector &variant, const std::list<std::string> tokens);
std::vector<std::pair<klex_token_t, klex_token_t>> km2_produce_token_pairs(const kgram_variant_t &variant);

std::ostream &operator<< (std::ostream& stream, const std::vector<std::pair<klex_token_t, klex_token_t>>& vector);



#endif // KM2_TOKEN_TOOLS_H
