#ifndef GRAM_H
#define GRAM_H

#include "private/gram_tools.h"

namespace wall_e {

namespace gram {

struct recursion_error { recursion_error() {}; };
inline bool operator ==(const recursion_error&, const recursion_error&) { return true; }

enum flags {
    use_default_parser,
    verbose,
    unconditional_transition
};


typedef std::list<flags> flags_list;

struct __flags_private;

call_mono_result call(const pattern &p, token_iterator *it, const std::list<pattern> &patterns, const __flags_private &flags);
call_mono_result disjunction_call(const std::vector<rule> &disjunctions, token_iterator *it, const std::list<pattern> &patterns, const __flags_private &flags);
call_result conjunction_call(const std::vector<rule> &conjunctions, token_iterator *it, const std::list<pattern> &patterns, const __flags_private &flags);
call_mono_result text_call(const std::string &rule_text, token_iterator *it, const std::list<pattern> &patterns, const __flags_private &flags);
call_mono_result null_call(token_iterator *it, const __flags_private &flags);
argument exec(const std::list<pattern> &pattens, const std::vector<wall_e::lex::token> &tokens, const flags_list &flags = flags_list());

}
}

#endif // GRAM_H
