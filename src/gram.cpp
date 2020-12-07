#include "gram.h"

#include <iostream>
#include <string.h>
#include <cassert>

#include "color.h"

namespace wall_e {
namespace gram {

const color::color_t __err_color = color::Red;
const color::color_t __header_color = color::Magenta;
const color::color_t __warning_color = color::Yellow;

uint32_t __level = 0;
const uint32_t __recursion_max_level = 500;
bool __recursion_error = false;
bool __recursion_error_call = false;

class __end_call {
    std::function<void()> m_f;
public:
    __end_call(const std::function<void()> &f) { m_f = f; }
    ~__end_call() { m_f(); }
};

std::string __level_str() {
    std::string str;
    for(uint32_t i = 0; i < __level; ++i) {
        str += " ";
    }
    return str;
}

struct __recursion_error_ubiq {
    operator call_result () { return call_result({ recursion_error() }, true); }
    operator call_mono_result () { return call_mono_result(recursion_error(), true); }
};

#define K_GRAM_USE_LEVEL \
    if(__level > __recursion_max_level || __recursion_error) { \
        __recursion_error = true; \
        return __recursion_error_ubiq(); \
    } \
    __level++; \
    __end_call __level_end_call([&](){ __level--; });

#define K_GRAM_ENTER_USE_LEVEL \
    if(__level > __recursion_max_level || __recursion_error) { \
        __recursion_error = true; \
        if(!__recursion_error_call) { \
            return call_mono_result(p.callback(flags.use_default_parser)({ gram::recursion_error() }), true); \
        } else { \
            return call_mono_result(gram::recursion_error(), true); \
        } \
        \
    } \
    __level++; \
    __end_call __level_end_call([&](){ __level--; });


#define K_GRAM_LEVEL __level_str()


struct __flags_private {
    bool use_default_parser = false;
    bool verbose = false;
};


call_mono_result text_call(const std::string &rule_text, kgram_token_iterator *it, const std::list<kgram_pattern_t> &patterns, const __flags_private &flags) {
    K_GRAM_USE_LEVEL
    if(flags.verbose)
        std::cout << K_GRAM_LEVEL << __warning_color("t ") << *it << " <- " << rule_text << "\n";

    auto item = kgram_determine_item(it, patterns, rule_text);
    if(item.type() == kgram_item_t::Token) {
        return call_mono_result({ argument(item.token()) }, true);
    } else if(item.type() == kgram_item_t::Pattern) {
        return call(item.pattern(), it, patterns, flags);
    }
    return call_mono_result();
}

call_mono_result null_call(kgram_token_iterator *it, const __flags_private &flags) {
    K_GRAM_USE_LEVEL
    if(flags.verbose)
        std::cout << K_GRAM_LEVEL << __warning_color("n") << "\n";

    const bool alwaysConfirm = false;
    return call_mono_result(wall_e::variant(), alwaysConfirm ? true : !it->isValid());
}


call_result conjunction_call(const std::vector<rule> &conjunctions, kgram_token_iterator *it, const std::list<kgram_pattern_t> &patterns, const __flags_private &flags) {
    K_GRAM_USE_LEVEL
    if(flags.verbose)
        std::cout << K_GRAM_LEVEL << __warning_color("&") << "\n";

    size_t i = 0;
    arg_vector args;
    args.resize(conjunctions.size());

    for(auto expected_token : conjunctions) {
        call_mono_result tmp_result;

        if(expected_token.type() == rule_type::Text) {
            tmp_result = text_call(expected_token.value(), it, patterns, flags);
        } else if(expected_token.type() == rule_type::Disjunction) {
            tmp_result = disjunction_call(expected_token.children(), it, patterns, flags);
        } else {
            tmp_result = null_call(it, flags);
        }

        if(!tmp_result.confirmed) {
            return call_result();
        }

        args[i] = tmp_result.arg;

        if(tmp_result.arg.contains_type<wall_e::lex::token>()) {
            if(flags.verbose)
                std::cout << K_GRAM_LEVEL << __warning_color("++") << "\n";
            it->next();
        }
        ++i;
    }

    if(flags.verbose)
        std::cout << K_GRAM_LEVEL << args << "\n";
    return call_result(args, true);
}

call_mono_result disjunction_call(const std::vector<rule> &disjunctions, kgram_token_iterator *it, const std::list<kgram_pattern_t> &patterns, const __flags_private &flags) {
    K_GRAM_USE_LEVEL
    if(flags.verbose)
        std::cout << K_GRAM_LEVEL << __warning_color("|") << "\n";

    kgram_token_iterator it_backup = *it;
    for(auto particular_case : disjunctions) {
        if(flags.verbose)
            std::cout << K_GRAM_LEVEL << "curr it: " << *it << "\n";

        call_mono_result tmp_result;
        if(particular_case.type() == rule_type::Text) {
            tmp_result = text_call(particular_case.value(), it, patterns, flags);
        } else if(particular_case.type() == rule_type::Disjunction) {
            if(flags.verbose)
                std::cout << K_GRAM_LEVEL << __err_color("deprecated operation Disjunction in Disjunction\n");

            tmp_result = disjunction_call(particular_case.children(), it, patterns, flags);
        } else if(particular_case.type() == rule_type::Conjunction) {
            if(flags.verbose)
                std::cout << K_GRAM_LEVEL << __err_color("deprecated operation Conjunction in Disjunction\n");

            //MAY BE REMOVED
            tmp_result = kgram_call_mono_result_cast(conjunction_call(particular_case.children(), it, patterns, flags));
            //--- -- -------
        } else {
            tmp_result = null_call(it, flags);
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
    return call_mono_result();
}


call_mono_result call(const kgram_pattern_t &p, kgram_token_iterator *it, const std::list<kgram_pattern_t> &patterns, const __flags_private &flags) {
    K_GRAM_ENTER_USE_LEVEL

    if(flags.verbose && __recursion_error)
        std::cout << K_GRAM_LEVEL << __err_color("kgram_recursion_error (pattern: " + p.name() + ")") << "\n";

    auto rule = simplify_rule(p.gram_rule(), rule_transition::DoubleConjunction);

    if(flags.verbose)
        std::cout << K_GRAM_LEVEL << ">> pattern: " << p.name() << " origin rule: " << p.gram_rule() << " sinplified rule: " << __header_color(rule) << " current word: " << *it << ")\n";

    call_result result;
    if(rule.type() == rule_type::Text) {
        result = kgram_call_result_cast(text_call(rule.value(), it, patterns, flags));
    } else if(rule.type() == rule_type::Disjunction) {
        result = kgram_call_result_cast(disjunction_call(rule.children(), it, patterns, flags));
    } else if(rule.type() == rule_type::Conjunction) {
        result = conjunction_call(rule.children(), it, patterns, flags);
    } else if(rule.isNull()) {
        result = kgram_call_result_cast(null_call(it, flags));
    }

    if(result.confirmed) {
        if(flags.verbose)
            std::cout << K_GRAM_LEVEL << "<< " << result.arg << "\n";
        return call_mono_result(p.callback(flags.use_default_parser)(result.arg), true);
    }
    if(flags.verbose)
        std::cout << K_GRAM_LEVEL << "<< ----" << "\n";
    return call_mono_result();
}


argument exec(const std::list<kgram_pattern_t> &pattens, const std::vector<wall_e::lex::token> &tokens, const flags_list flags) {
    __flags_private __flags;

    for(auto f : flags) {
        if(f == flags::use_default_parser) {
            __flags.use_default_parser = true;
        } else if(f == flags::verbose) {
            __flags.verbose = true;
        }
    }

    __recursion_error = false;
    if(pattens.size() > 0 && tokens.size() > 0) {
        kgram_token_iterator it = tokens;
        auto result = call(pattens.front(), &it, pattens, __flags).arg;
        if(__recursion_error) {
            return recursion_error();
        }
        return result;
    }
    return argument();
}
}
}