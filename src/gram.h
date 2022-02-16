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

struct index_iter {
    std::size_t x = 0;
    std::size_t level = 0;
    std::size_t pattern_level = 0;
    std::size_t area = 0;
    index_iter next_level() const { return { 0, level + 1, pattern_level, area + 1 }; }
    index_iter next_pattern_level() const { return { 0, level + 1, pattern_level + 1, area + 1 }; }
    index_iter next_x() const { return { x + 1, level, pattern_level, area + 1 }; }
    index to_index(std::size_t construction_index) const { return { x, pattern_level, area, construction_index }; }
};



call_mono_result call(
        const pattern &p,
        token_iterator *it,
        const std::list<pattern> &patterns,
        const __flags_private &flags,
        index_iter index_it,
        std::size_t *construction_index
        );

call_mono_result disjunction_call(
        const std::vector<rule> &disjunctions,
        token_iterator *it,
        const std::list<pattern> &patterns,
        const __flags_private &flags,
        index_iter index_it,
        std::size_t *construction_index
        );

call_result conjunction_call(
        const std::vector<rule> &conjunctions,
        token_iterator *it,
        const std::list<pattern> &patterns,
        const __flags_private &flags,
        index_iter index_it,
        std::size_t *construction_index
        );

call_mono_result text_call(
        const std::string &rule_text,
        token_iterator *it,
        const std::list<pattern> &patterns,
        const __flags_private &flags,
        index_iter index_it,
        std::size_t *construction_index
        );

call_mono_result null_call(
        token_iterator *it,
        const __flags_private &flags,
        index_iter index_it,
        std::size_t *construction_index
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
