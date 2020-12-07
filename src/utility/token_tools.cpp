#include "token_tools.h"

namespace wall_e {


std::vector<lex::token> produce_tokens(const wall_e::variant &variant, const std::list<std::string> names) {
    std::vector<lex::token> result;
    if(variant.contains_type<variant_vector>()) {
        const auto vec = variant.value<wall_e::variant_vector>();
        for(auto v : vec) {
            auto c = produce_tokens(v, names);
            for(auto token : c) {
                result.push_back(token);
            }
        }
    } else if(variant.contains_type<wall_e::lex::token>()) {
        const auto token = variant.value<wall_e::lex::token>();
        if(std::find(names.begin(), names.end(), token.name) != names.end()) {
            return { token };
        }
    }
    return result;
}

std::vector<std::pair<wall_e::lex::token, wall_e::lex::token> > produce_token_pairs(const wall_e::variant &variant) {
    std::vector<std::pair<wall_e::lex::token, wall_e::lex::token> > result;
    if(variant.contains_type<wall_e::variant_vector>()) {
        const auto vec = variant.value<wall_e::variant_vector>();
        if(vec.size() == 2 && vec[0].contains_type<wall_e::lex::token>() && vec[1].contains_type<wall_e::lex::token>()) {
            return { { vec[0].value<wall_e::lex::token>(), vec[1].value<wall_e::lex::token>() } };
        } else {
            for(auto v : vec) {
                auto c = produce_token_pairs(v);
                for(auto pair : c) {
                    result.push_back(pair);
                }
            }
        }
    }
    return result;
}


std::ostream &operator<<(std::ostream &stream, const std::vector<std::pair<wall_e::lex::token, wall_e::lex::token> > &vector) {
    stream << "klex_token_pair_vector { " << (vector.size() > 3 ? "\n" : "");
    for(size_t i = 0; i < vector.size(); ++i) {
        stream << "{ " << vector[i].first << ", " << vector[i].second << " }" << ((i == vector.size() - 1) ? " " : (vector.size() > 3 ? ",\n" : ", "));
    }
    stream << "}";

    return stream;
}

wall_e::variant_vector remove_tokens(const wall_e::variant_vector &variant, const std::list<std::string> tokens) {
    auto vec = variant;

    auto it = vec.begin();
    while(it != vec.end()) {
        if(it->contains_type<wall_e::lex::token>() && (std::find(tokens.begin(), tokens.end(), it->value<wall_e::lex::token>().name) != tokens.end()))  {
            it = vec.erase(it);
        } else {
            it++;
        }
    }
    return vec;
}
}
