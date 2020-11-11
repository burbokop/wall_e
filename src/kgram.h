#ifndef KGRAM_H
#define KGRAM_H


#include "kgram_tools.h"

class kgram_recursion_error {
public:
    kgram_recursion_error() {};
};

inline bool operator ==(const kgram_recursion_error&, const kgram_recursion_error&) { return true; }

enum kgram_flags {
    kgram_use_default_parser,
    kgram_verbose
};

typedef std::list<kgram_flags> kgram_flags_list;

struct __kgram_flags_private;

kgram_call_mono_result kgram_call(const kgram_pattern_t &p, kgram_token_iterator *it, const std::list<kgram_pattern_t> &patterns, const __kgram_flags_private &flags);
kgram_call_mono_result kgram_disjunction_call(const std::vector<kgram_rule_t> &disjunctions, kgram_token_iterator *it, const std::list<kgram_pattern_t> &patterns, const __kgram_flags_private &flags);
kgram_call_result kgram_conjunction_call(const std::vector<kgram_rule_t> &conjunctions, kgram_token_iterator *it, const std::list<kgram_pattern_t> &patterns, const __kgram_flags_private &flags);
kgram_call_mono_result kgram_text_call(const std::string &rule_text, kgram_token_iterator *it, const std::list<kgram_pattern_t> &patterns, const __kgram_flags_private &flags);
kgram_call_mono_result kgram_null_call(kgram_token_iterator *it, const __kgram_flags_private &flags);
kgram_argument_t kgram_exec(const std::list<kgram_pattern_t> &pattens, const std::vector<klex_token_t> &tokens, const std::list<kgram_flags> flags = std::list<kgram_flags>());


#endif // KGRAM_H
