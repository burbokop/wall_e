#ifndef GRAM_H
#define GRAM_H

#include "private/gram_private.h"
#include "private/gram_smp.h"
#include "models/either.h"
#include "models/error.h"

namespace wall_e {

namespace gram {

namespace errors {
enum  {
    recursion,
    failed_disjunction,
    unmatched_pattern
};
}

enum flags {
    use_default_parser,
    verbose,
    unconditional_transition
};


typedef std::list<flags> flags_list;

struct __flags_private;


call_mono_result call(
        const pattern &p,
        token_iterator *it,
        const std::list<pattern> &patterns,
        const __flags_private &flags,
        std::size_t level
        );

call_mono_result disjunction_call(
        const std::vector<rule> &disjunctions,
        token_iterator *it,
        const std::list<pattern> &patterns,
        const __flags_private &flags,
        std::size_t level
        );

call_result conjunction_call(
        const std::vector<rule> &conjunctions,
        token_iterator *it,
        const std::list<pattern> &patterns,
        const __flags_private &flags,
        std::size_t level
        );

call_mono_result text_call(
        const std::string &rule_text,
        token_iterator *it,
        const std::list<pattern> &patterns,
        const __flags_private &flags,
        std::size_t level
        );

call_mono_result null_call(
        token_iterator *it,
        const __flags_private &flags,
        std::size_t level
        );

either<error, argument> exec(
        const std::list<pattern> &patterns,
        const std::vector<wall_e::lex::token> &tokens,
        const flags_list &flags = flags_list(),
        const std::function<rule(const rule&)> &simplification_function = smp::simplify
        );

}
}

#endif // GRAM_H
