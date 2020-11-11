#include "km2_token_tools.h"

std::vector<klex_token_t> km2_produce_tokens(const kgram_variant_t &variant, const std::list<std::string> names) {
    std::vector<klex_token_t> result;
    if(variant.contains_type<kgram_variant_vector>()) {
        const auto vec = variant.value<kgram_variant_vector>();
        for(auto v : vec) {
            auto c = km2_produce_tokens(v, names);
            for(auto token : c) {
                result.push_back(token);
            }
        }
    } else if(variant.contains_type<klex_token_t>()) {
        const auto token = variant.value<klex_token_t>();
        if(std::find(names.begin(), names.end(), token.name) != names.end()) {
            return { token };
        }
    }
    return result;
}

std::vector<std::pair<klex_token_t, klex_token_t> > km2_produce_token_pairs(const kgram_variant_t &variant) {
    std::vector<std::pair<klex_token_t, klex_token_t> > result;
    if(variant.contains_type<kgram_variant_vector>()) {
        const auto vec = variant.value<kgram_variant_vector>();
        if(vec.size() == 2 && vec[0].contains_type<klex_token_t>() && vec[1].contains_type<klex_token_t>()) {
            return { { vec[0].value<klex_token_t>(), vec[1].value<klex_token_t>() } };
        } else {
            for(auto v : vec) {
                auto c = km2_produce_token_pairs(v);
                for(auto pair : c) {
                    result.push_back(pair);
                }
            }
        }
    }
    return result;
}


std::ostream &operator<<(std::ostream &stream, const std::vector<std::pair<klex_token_t, klex_token_t> > &vector) {
    stream << "klex_token_pair_vector { " << (vector.size() > 3 ? "\n" : "");
    for(size_t i = 0; i < vector.size(); ++i) {
        stream << "{ " << vector[i].first << ", " << vector[i].second << " }" << ((i == vector.size() - 1) ? " " : (vector.size() > 3 ? ",\n" : ", "));
    }
    stream << "}";

    return stream;
}

kgram_variant_vector km2_remove_tokens(const kgram_variant_vector &variant, const std::list<std::string> tokens) {
    auto vec = variant;

    auto it = vec.begin();
    while(it != vec.end()) {
        if(it->contains_type<klex_token_t>() && (std::find(tokens.begin(), tokens.end(), it->value<klex_token_t>().name) != tokens.end()))  {
            it = vec.erase(it);
        } else {
            it++;
        }
    }
    return vec;
}
