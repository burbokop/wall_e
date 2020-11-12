#ifndef KGRAM_H
#define KGRAM_H


#include "kgram_tools.h"

namespace wall_e {
namespace gram {


class recursion_error {
public:
    recursion_error() {};
};

inline bool operator ==(const recursion_error&, const recursion_error&) { return true; }

enum flags {
    use_default_parser,
    verbose
};

typedef std::list<flags> flags_list;

struct __flags_private;

call_mono_result call(const Pattern &p, token_iterator *it, const std::list<Pattern> &patterns, const __flags_private &flags);
call_mono_result disjunction_call(const std::vector<Rule> &disjunctions, token_iterator *it, const std::list<Pattern> &patterns, const __flags_private &flags);
call_result conjunction_call(const std::vector<Rule> &conjunctions, token_iterator *it, const std::list<Pattern> &patterns, const __flags_private &flags);
call_mono_result text_call(const std::string &rule_text, token_iterator *it, const std::list<Pattern> &patterns, const __flags_private &flags);
call_mono_result null_call(token_iterator *it, const __flags_private &flags);
argument exec(const std::list<Pattern> &pattens, const std::vector<wall_e::lex::Token> &tokens, const std::list<flags> flags = std::list<flags>());

}
}

#endif // KGRAM_H
