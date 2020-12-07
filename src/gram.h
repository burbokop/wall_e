#ifndef GRAM_H
#define GRAM_H


#include "private/kgram_tools.h"

namespace wall_e {

namespace gram {

struct recursion_error { recursion_error() {}; };
inline bool operator ==(const recursion_error&, const recursion_error&) { return true; }

enum flags {
    use_default_parser,
    verbose
};

typedef std::list<flags> flags_list;

struct __flags_private;

kgram_call_mono_result call(const kgram_pattern_t &p, kgram_token_iterator *it, const std::list<kgram_pattern_t> &patterns, const __flags_private &flags);
kgram_call_mono_result disjunction_call(const std::vector<kgram_rule_t> &disjunctions, kgram_token_iterator *it, const std::list<kgram_pattern_t> &patterns, const __flags_private &flags);
kgram_call_result conjunction_call(const std::vector<kgram_rule_t> &conjunctions, kgram_token_iterator *it, const std::list<kgram_pattern_t> &patterns, const __flags_private &flags);
kgram_call_mono_result text_call(const std::string &rule_text, kgram_token_iterator *it, const std::list<kgram_pattern_t> &patterns, const __flags_private &flags);
kgram_call_mono_result null_call(kgram_token_iterator *it, const __flags_private &flags);
kgram_argument_t exec(const std::list<kgram_pattern_t> &pattens, const std::vector<wall_e::lex::token> &tokens, const flags_list flags = flags_list());

}
}

#endif // GRAM_H
