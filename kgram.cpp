#include "kgram.h"

#include <iostream>
#include <string.h>
#include <cassert>

const char *__kgram_c_err = "\033[1;31m";
const char *__kgram_c_end = "\033[0m";
const char *__kgram_c_header = "\033[1;35m";
const char *__kgram_c_ending = "\033[1;36m";
const char *__kgram_c_warning = "\033[1;43m";

class kgram_end_call {
    std::function<void()> m_f;
public:
    kgram_end_call(const std::function<void()> &f) { m_f = f; }
    ~kgram_end_call() { m_f(); }
};

uint32_t __kgram_level = 0;

std::string __kgram_level_str() {
    std::string str;
    for(uint32_t i = 0; i < __kgram_level; ++i) {
        str += " ";
    }
    return str;
}

uint32_t __kgram_recursion_max_level = 500;
bool __kgram_recursion_error = false;
bool __kgram_recursion_error_call = false;

struct __kgram_recursion_error_ubiq {
    operator kgram_call_result () { return kgram_call_result({ kgram_recursion_error() }, true); }
    operator kgram_call_mono_result () { return kgram_call_mono_result(kgram_recursion_error(), true); }
};

#define K_GRAM_USE_LEVEL \
    if(__kgram_level > __kgram_recursion_max_level || __kgram_recursion_error) { \
        __kgram_recursion_error = true; \
        return __kgram_recursion_error_ubiq(); \
    } \
    __kgram_level++; \
    kgram_end_call __level_end_call([&](){ __kgram_level--; });

#define K_GRAM_ENTER_USE_LEVEL \
    if(__kgram_level > __kgram_recursion_max_level || __kgram_recursion_error) { \
        __kgram_recursion_error = true; \
        if(!__kgram_recursion_error_call) { \
            return kgram_call_mono_result(p.callback(flags.use_default_parser)({ kgram_recursion_error() }), true); \
        } else { \
            return kgram_call_mono_result(kgram_recursion_error(), true); \
        } \
        \
    } \
    __kgram_level++; \
    kgram_end_call __level_end_call([&](){ __kgram_level--; });


#define K_GRAM_LEVEL __kgram_level_str()


struct __kgram_flags_private {
    bool use_default_parser = false;
    bool verbose = false;
};


kgram_call_mono_result kgram_text_call(const std::string &rule_text, kgram_token_iterator *it, const std::list<kgram_pattern_t> &patterns, const __kgram_flags_private &flags) {
    K_GRAM_USE_LEVEL
    if(flags.verbose)
        std::cout << K_GRAM_LEVEL << __kgram_c_warning << "t " << __kgram_c_end << *it << " <- " << rule_text << "\n";

    auto item = kgram_determine_item(it, patterns, rule_text);
    if(item.type() == kgram_item_t::Token) {
        return kgram_call_mono_result({ kgram_argument_t(item.token()) }, true);
    } else if(item.type() == kgram_item_t::Pattern) {
        return kgram_call(item.pattern(), it, patterns, flags);
    }
    return kgram_call_mono_result();
}

kgram_call_mono_result kgram_null_call(kgram_token_iterator *it, const __kgram_flags_private &flags) {
    K_GRAM_USE_LEVEL
    if(flags.verbose)
        std::cout << K_GRAM_LEVEL << __kgram_c_warning << "n" << __kgram_c_end << "\n";

    const bool alwaysConfirm = false;
    return kgram_call_mono_result(kgram_variant_t(), alwaysConfirm ? true : !it->isValid());
}


kgram_call_result kgram_conjunction_call(const std::vector<kgram_rule_t> &conjunctions, kgram_token_iterator *it, const std::list<kgram_pattern_t> &patterns, const __kgram_flags_private &flags) {
    K_GRAM_USE_LEVEL
    if(flags.verbose)
        std::cout << K_GRAM_LEVEL << __kgram_c_warning << "&" << __kgram_c_end << "\n";

    size_t i = 0;
    kgram_arg_vector_t args;
    args.resize(conjunctions.size());

    for(auto expected_token : conjunctions) {
        kgram_call_mono_result tmp_result;

        if(expected_token.type() == kgram_rule_type_t::Text) {
            tmp_result = kgram_text_call(expected_token.value(), it, patterns, flags);
        } else if(expected_token.type() == kgram_rule_type_t::Disjunction) {
            tmp_result = kgram_disjunction_call(expected_token.children(), it, patterns, flags);
        } else {
            tmp_result = kgram_null_call(it, flags);
        }

        if(!tmp_result.confirmed) {
            return kgram_call_result();
        }

        args[i] = tmp_result.arg;

        if(tmp_result.arg.contains_type<klex_token_t>()) {
            if(flags.verbose)
                std::cout << K_GRAM_LEVEL << __kgram_c_warning << "++" << __kgram_c_end << "\n";
            it->next();
        }
        ++i;
    }

    if(flags.verbose)
        std::cout << K_GRAM_LEVEL << args << "\n";
    return kgram_call_result(args, true);
}

kgram_call_mono_result kgram_disjunction_call(const std::vector<kgram_rule_t> &disjunctions, kgram_token_iterator *it, const std::list<kgram_pattern_t> &patterns, const __kgram_flags_private &flags) {
    K_GRAM_USE_LEVEL
    if(flags.verbose)
        std::cout << K_GRAM_LEVEL << __kgram_c_warning << "|" << __kgram_c_end << "\n";

    kgram_token_iterator it_backup = *it;
    for(auto particular_case : disjunctions) {
        if(flags.verbose)
            std::cout << K_GRAM_LEVEL << "curr it: " << *it << "\n";

        kgram_call_mono_result tmp_result;
        if(particular_case.type() == kgram_rule_type_t::Text) {
            tmp_result = kgram_text_call(particular_case.value(), it, patterns, flags);
        } else if(particular_case.type() == kgram_rule_type_t::Disjunction) {
            if(flags.verbose)
                std::cout << K_GRAM_LEVEL << __kgram_c_err << "deprecated operation Disjunction in Disjunction\n";

            tmp_result = kgram_disjunction_call(particular_case.children(), it, patterns, flags);
        } else if(particular_case.type() == kgram_rule_type_t::Conjunction) {
            if(flags.verbose)
                std::cout << K_GRAM_LEVEL << __kgram_c_err << "deprecated operation Conjunction in Disjunction\n";

            //MAY BE REMOVED
            tmp_result = kgram_call_mono_result_cast(kgram_conjunction_call(particular_case.children(), it, patterns, flags));
            //--- -- -------
        } else {
            tmp_result = kgram_null_call(it, flags);
        }

        if(tmp_result.confirmed) {
            if(flags.verbose)
                std::cout << K_GRAM_LEVEL << "choise: " << tmp_result.arg << "\n";
            return tmp_result;
        } else {
            *it = it_backup;
        }
    }
    if(flags.verbose)
        std::cout << K_GRAM_LEVEL << "no choise" << "\n";
    return kgram_call_mono_result();
}


kgram_call_mono_result kgram_call(const kgram_pattern_t &p, kgram_token_iterator *it, const std::list<kgram_pattern_t> &patterns, const __kgram_flags_private &flags) {
    K_GRAM_ENTER_USE_LEVEL

    if(flags.verbose && __kgram_recursion_error)
        std::cout << K_GRAM_LEVEL << __kgram_c_err << "kgram_recursion_error (pattern: " << p.name() << __kgram_c_end << ")\n"; \

    auto rule = kgram_simplify_rule(p.rule(), kgram_rule_transition_t::DoubleConjunction);

    if(flags.verbose)
        std::cout << K_GRAM_LEVEL << ">> pattern: " << p.name() << " origin rule: " << p.rule() << " sinplified rule: " << __kgram_c_header << rule << __kgram_c_end << " current word: " << *it << ")\n";

    kgram_call_result result;
    if(rule.type() == kgram_rule_type_t::Text) {
        result = kgram_call_result_cast(kgram_text_call(rule.value(), it, patterns, flags));
    } else if(rule.type() == kgram_rule_type_t::Disjunction) {
        result = kgram_call_result_cast(kgram_disjunction_call(rule.children(), it, patterns, flags));
    } else if(rule.type() == kgram_rule_type_t::Conjunction) {
        result = kgram_conjunction_call(rule.children(), it, patterns, flags);
    } else if(rule.isNull()) {
        result = kgram_call_result_cast(kgram_null_call(it, flags));
    }

    if(result.confirmed) {
        if(flags.verbose)
            std::cout << K_GRAM_LEVEL << "<< " << result.arg << "\n";
        return kgram_call_mono_result(p.callback(flags.use_default_parser)(result.arg), true);
    }
    if(flags.verbose)
        std::cout << K_GRAM_LEVEL << "<< ----" << "\n";
    return kgram_call_mono_result();
}


kgram_argument_t kgram_exec(const std::list<kgram_pattern_t> &pattens, const std::vector<klex_token_t> &tokens, const std::list<kgram_flags> flags) {
    __kgram_flags_private __flags;

    for(auto f : flags) {
        if(f == kgram_flags::kgram_use_default_parser) {
            __flags.use_default_parser = true;
        } else if(f == kgram_flags::kgram_verbose) {
            __flags.verbose = true;
        }
    }

    __kgram_recursion_error = false;
    if(pattens.size() > 0 && tokens.size() > 0) {
        kgram_token_iterator it = tokens;
        auto result = kgram_call(pattens.front(), &it, pattens, __flags).arg;
        if(__kgram_recursion_error) {
            return kgram_recursion_error();
        }
        return result;
    }
    return kgram_argument_t();
}
