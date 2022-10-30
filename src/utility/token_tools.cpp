#include "token_tools.h"

wall_e::lex::token_vec wall_e::produce_tokens(const wall_e::variant &variant, const str_list& names) {
    lex::token_vec result;
    if(variant.contains_type<variant_vector>()) {
        const auto vec = variant.value<wall_e::variant_vector>();
        for(const auto &v : vec) {
            auto c = produce_tokens(v, names);
            for(const auto &token : c) {
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

wall_e::vec<wall_e::pair<wall_e::lex::token, wall_e::lex::token>> wall_e::produce_token_pairs(const wall_e::variant &variant) {
    wall_e::vec<wall_e::pair<wall_e::lex::token, wall_e::lex::token>> result;
    if(variant.contains_type<wall_e::variant_vector>()) {
        const auto vec = variant.value<wall_e::variant_vector>();
        if(vec.size() == 2 && vec[0].contains_type<wall_e::lex::token>() && vec[1].contains_type<wall_e::lex::token>()) {
            return { { vec[0].value<wall_e::lex::token>(), vec[1].value<wall_e::lex::token>() } };
        } else {
            for(const auto& v : vec) {
                auto c = produce_token_pairs(v);
                for(const auto& pair : c) {
                    result.push_back(pair);
                }
            }
        }
    }
    return result;
}

wall_e::variant_vector wall_e::remove_tokens(const wall_e::variant_vector &variant, const str_list& tokens) {
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

wall_e::vec<wall_e::pair<wall_e::lex::token, wall_e::lex::token>> wall_e::invert(const wall_e::vec<wall_e::pair<lex::token, lex::token> > &a) {
    wall_e::vec<wall_e::pair<lex::token, lex::token>> result;
    for(const auto& aa : a) {
        result.push_back({ aa.second, aa.first });
    }
    return result;
}

wall_e::lex::token_vec wall_e::left_every_second(const variant &variant) {
    lex::token_vec result;
    for(const auto& pair : produce_token_pairs(variant)) {
        result.push_back(pair.first);
    }
    return result;
}

wall_e::lex::token_vec wall_e::right_every_second(const variant &variant) {
    lex::token_vec result;
    for(const auto& pair : produce_token_pairs(variant)) {
        result.push_back(pair.second);
    }
    return result;
}
