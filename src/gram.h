#ifndef GRAM_H
#define GRAM_H

#include "private/gram_private.h"
#include "private/gram_smp.h"
#include "models/either.h"
#include "models/error.h"
#include "enums.h"
#include "color.h"

namespace wall_e {

namespace gram {

namespace errors {
enum  {
    Recursion,
    FailedDisjunction,
    UnmatchedPattern
};
}

wall_e_enum(flags,
    UseDefaultParser        = 1,
    UnconditionalTransition = 2
    ///                     = 4
)

typedef wall_e::list<flags> flags_list;

struct __flags_private;


struct log;

/*
struct dbg_context {
    std::ostream& log;
    std::ostream& table_log;
    std::ostream& table_csv_log;
    std::size_t table_col_width;
    std::size_t table_col_count;
    void add_table_log(const wall_e::str_vec& aligned, const wall_e::str_vec& not_aligned);
    void add_table_log(token_iterator *it, const std::string& s, wall_e::color::color_t c = wall_e::color::Black);
};*/

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
        const pattern_list &patterns,
        const wall_e::gram::environment* env,
        const __flags_private &flags,
        index_iter index_it,
        std::size_t *construction_index,
        log *l
        );

call_mono_result disjunction_call(
        const rule_vec &disjunctions,
        token_iterator *it,
        const pattern_list &patterns,
        const wall_e::gram::environment* env,
        const __flags_private &flags,
        index_iter index_it,
        std::size_t *construction_index,
        log *l
        );

call_result conjunction_call(
        const rule_vec &conjunctions,
        token_iterator *it,
        const pattern_list &patterns,
        const wall_e::gram::environment* env,
        const __flags_private &flags,
        index_iter index_it,
        std::size_t *construction_index,
        log *l
        );

call_mono_result text_call(
        const std::string &rule_text,
        token_iterator *it,
        const pattern_list &patterns,
        const wall_e::gram::environment* env,
        const __flags_private &flags,
        index_iter index_it,
        std::size_t *construction_index,
        log *l
        );

call_mono_result null_call(
        token_iterator *it,
        const wall_e::gram::environment* env,
        const __flags_private &flags,
        index_iter index_it,
        std::size_t *construction_index,
        log *l
        );

either<error, argument> exec(
        const pattern_list &patterns,
        const lex::token_vec &tokens,
        const environment* env,
        const flags_list &flags = flags_list(),
        const std::function<rule(const rule&)> &simplification_function = smp::simplify,
        log *l = nullptr
        );


struct log {
    struct step {
        const bool importent;
        const std::string text;
        const std::string short_text;
        const std::string hover_text;
        const std::int64_t token_index;
        const std::size_t level;
        const wall_e::color color;
        inline friend std::ostream& operator<<(std::ostream& stream, const step& l) {
            return stream << "{ importent" << l.importent
                          << ", text" << l.text
                          << ", short_text" << l.short_text
                          << ", hover_text" << l.hover_text
                          << ", token_index" << l.token_index
                          << ", level" << l.level
                          << ", color" << l.color
                          << " }";
        }
    };
private:
    wall_e::vec<step> m_steps;
    lex::token_vec m_tokens;
public:
    void set_tokens(const lex::token_vec& tokens) { m_tokens = tokens; }
    void add_step(
            const bool importent,
            const std::string &text,
            const std::string &short_text,
            const std::string &hover_text,
            const token_iterator *it,
            const std::size_t level,
            const wall_e::color &color = wall_e::black
            ) { m_steps.push_back(step { importent, text, short_text, hover_text, it->offset(), level, color }); }

    const wall_e::vec<step>& steps() const { return m_steps; }
    const lex::token_vec& tokens() const { return m_tokens; };
};


}
}

#endif // GRAM_H
