#include "gram.h"

#include <iostream>
#include <string.h>
#include <cassert>

#include "color.h"

namespace wall_e {
namespace gram {

using namespace literals;

const color::color_t __err_color = color::BrightRed;
const color::color_t __header_color = color::BrightMagenta;
const color::color_t __warning_color = color::BrightYellow;

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
    bool unconditional_transition = false;
    std::function<rule(const rule&)> simplification_function;
    __flags_private(const flags_list &flags = flags_list(), const std::function<rule(const rule&)> &simplification_function = {}) {
        this->simplification_function = simplification_function;
        for(auto f : flags) {
            if(f == flags::use_default_parser) {
                use_default_parser = true;
            } else if(f == flags::verbose) {
                verbose = true;
            } else if(f == flags::unconditional_transition) {
                unconditional_transition = true;
            }
        }
    }
};


call_mono_result text_call(const std::string &rule_text, token_iterator *it, const std::list<pattern> &patterns, const __flags_private &flags) {
    K_GRAM_USE_LEVEL
            if(flags.verbose)
            std::cout << K_GRAM_LEVEL << __warning_color("t ") << *it << " <- " << rule_text << "\n";


    auto item = determine_item(it, patterns, rule_text);
    if(flags.verbose)
        std::cout << K_GRAM_LEVEL << "  item: " << item << "\n";

    if(item.type() == item::Token) {
        return call_mono_result({ argument(item.token()) }, true);
    } else if(item.type() == item::Pattern) {
        return call(item.gram_pattern(), it, patterns, flags);
    }
    return call_mono_result();
}

call_mono_result null_call(token_iterator *it, const __flags_private &flags) {
    K_GRAM_USE_LEVEL
            if(flags.verbose)
            std::cout << K_GRAM_LEVEL << __warning_color("n") << "\n";

    const bool alwaysConfirm = false;
    return call_mono_result(wall_e::variant(), alwaysConfirm ? true : !it->isValid());
}


call_result conjunction_call(const std::vector<rule> &conjunctions, token_iterator *it, const std::list<pattern> &patterns, const __flags_private &flags) {
    K_GRAM_USE_LEVEL
            if(flags.verbose)
            std::cout << K_GRAM_LEVEL << __warning_color("&") << "\n";

    size_t i = 0;
    arg_vector args;
    args.resize(conjunctions.size());

    for(const auto& expected_token : conjunctions) {
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

        if(flags.unconditional_transition) {
            if(i < conjunctions.size() - 1) {
                if(flags.verbose)
                    std::cout << K_GRAM_LEVEL << __warning_color("++") << "\n";
                it->next();
            } else if(flags.verbose) {
                std::cout << K_GRAM_LEVEL << __warning_color("++ aborted as last") << "\n";
            }
        } else {
            if(tmp_result.arg.contains_type<wall_e::lex::token>()) {
                if(flags.verbose)
                    std::cout << K_GRAM_LEVEL << __warning_color("++") << "\n";
                it->next();
            } else if(flags.verbose) {
                std::cout << K_GRAM_LEVEL << __err_color("++ aborted") << " arg: " << tmp_result.arg << "\n";
            }
        }

        ++i;
    }

    if(flags.verbose)
        std::cout << K_GRAM_LEVEL << args << "\n";
    return call_result(args, true);
}

call_mono_result disjunction_call(const std::vector<rule> &disjunctions, token_iterator *it, const std::list<pattern> &patterns, const __flags_private &flags) {
    K_GRAM_USE_LEVEL
            if(flags.verbose)
            std::cout << K_GRAM_LEVEL << __warning_color("|") << "\n";

    token_iterator it_backup = *it;
    for(const auto& particular_case : disjunctions) {
        if(flags.verbose)
            std::cout << K_GRAM_LEVEL << "curr it: " << *it << "\n";

        call_mono_result tmp_result;

        if(particular_case.isNull()) {
            tmp_result = null_call(it, flags);
        } else {
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
                tmp_result = call_mono_result_cast(conjunction_call(particular_case.children(), it, patterns, flags));
                //--- -- -------
            } else {
                tmp_result = null_call(it, flags);
            }
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


call_mono_result call(const pattern &p, token_iterator *it, const std::list<pattern> &patterns, const __flags_private &flags) {
    K_GRAM_ENTER_USE_LEVEL

            if(flags.verbose && __recursion_error)
            std::cout << K_GRAM_LEVEL << __err_color("kgram_recursion_error (pattern: " + p.name() + ")") << "\n";

    auto rule = flags.simplification_function(p.gram_rule());

    if(flags.verbose)
        std::cout << K_GRAM_LEVEL << ">> pattern: " << __header_color(p.name()) << " " << p.gram_rule() << " -> " << __header_color(rule) << " current word: " << __warning_color(*it) << ")\n";

    call_result result;
    if(rule.type() == rule_type::Text) {
        result = call_result_cast(text_call(rule.value(), it, patterns, flags));
    } else if(rule.type() == rule_type::Disjunction) {
        result = call_result_cast(disjunction_call(rule.children(), it, patterns, flags));
    } else if(rule.type() == rule_type::Conjunction) {
        result = conjunction_call(rule.children(), it, patterns, flags);
    } else if(rule.isNull()) {
        result = call_result_cast(null_call(it, flags));
    }

    if(result.confirmed) {
        if(flags.verbose)
            std::cout << K_GRAM_LEVEL << "<< " << result.arg << "\n";
        return call_mono_result(p.callback(flags.use_default_parser)(result.arg), true);
    }
    if(flags.verbose)
        std::cout << K_GRAM_LEVEL << "<< " << __err_color("<(0)_(0)>") << "\n";
    return call_mono_result();
}


argument exec(const std::list<pattern> &patterns, const std::vector<wall_e::lex::token> &tokens, const flags_list &flags, const std::function<rule(const rule &)> &simplification_function) {
    __recursion_error = false;
    if(patterns.size() > 0 && tokens.size() > 0) {
        token_iterator it = tokens;
        auto result = call(patterns.front(), &it, patterns, __flags_private(flags, simplification_function)).arg;
        if(__recursion_error) {
            return recursion_error();
        }
        return result;
    }
    return argument();
}
}
}
