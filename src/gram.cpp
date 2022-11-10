#include "gram.h"

#include <iostream>
#include <string.h>
#include <cassert>
#include <fstream>
#include <streambuf>
#include "color.h"
#ifdef __linux__
#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/stream.hpp>
#endif

namespace wall_e {
namespace gram {

using namespace literals;

const color::color_t& __err_color = color::BrightRed;
const color::color_t& __header_color = color::BrightMagenta;
const color::color_t& __warning_color = color::BrightYellow;

const uint32_t __recursion_max_level = 500;

#ifdef __linux__
std::ofstream flog = std::ofstream("wall_e.gram.ansi");
boost::iostreams::stream log = boost::iostreams::tee(flog, std::cout);
#else
std::ofstream log = std::ofstream("wall_e.gram.ansi");
#endif

class __defer {
    std::function<void()> m_f;
public:
    __defer(const std::function<void()> &f) { m_f = f; }
    ~__defer() { m_f(); }
};

std::string __level_str(std::size_t level) {
    std::string str;
    for(uint32_t i = 0; i < level; ++i) {
        str += " ";
    }
    return str;
}

struct __recursion_error_ubiq {
    inline static const error err = error("recursion error", error::err, error::gramatic, errors::recursion);

    operator call_result () { return call_result::from_error(err); }
    operator call_mono_result () { return call_mono_result::from_error(err); }
};

#define K_GRAM_CHECK_LEVEL \
    if(index_it.level > __recursion_max_level) { \
        return __recursion_error_ubiq(); \
    }

#define K_GRAM_LEVEL __level_str(index_it.level)


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


call_mono_result text_call(const std::string &rule_text, token_iterator *it, const pattern_list &patterns, const __flags_private &flags, index_iter index_it, std::size_t *construction_index) {
    K_GRAM_CHECK_LEVEL
    if(flags.verbose) { log << K_GRAM_LEVEL << __warning_color("t ") << *it << " <- " << rule_text << std::endl; }

    if(rule_text == "-") {
        if(flags.verbose)
            log << K_GRAM_LEVEL << "  item: skip (no increment iter)\n";
        return call_mono_result({}, true, true);
    }

    auto item = determine_item(it, patterns, rule_text);
    if(flags.verbose)
        log << K_GRAM_LEVEL << "  item: " << item << "\n";

    if(item.type() == item::Token) {
        return call_mono_result({ item.token() }, true);
    } else if(item.type() == item::Pattern) {
        return call(item.gram_pattern(), it, patterns, flags, index_it.next_level(), construction_index);
    }
    return call_mono_result();
    //return call_mono_result::from_error(error("udef token or pattern '" + rule_text + "'", error::err, it->segment()));
}

call_mono_result null_call(token_iterator *it, const __flags_private &flags, index_iter index_it, std::size_t *construction_index) {
    K_GRAM_CHECK_LEVEL
    if(flags.verbose) { log << K_GRAM_LEVEL << __warning_color("n") << "\n"; }

    const bool alwaysConfirm = false;
    return call_mono_result(wall_e::variant(), alwaysConfirm ? true : !it->isValid());
}


call_result conjunction_call(const rule_vec &conjunctions, token_iterator *it, const pattern_list &patterns, const __flags_private &flags, index_iter index_it, std::size_t *construction_index) {
    K_GRAM_CHECK_LEVEL
    if(flags.verbose) { log << K_GRAM_LEVEL << __warning_color("&") << "\n"; }

    size_t i = 0;
    arg_vector args;
    args.resize(conjunctions.size());

    for(const auto& expected_token : conjunctions) {
        call_mono_result tmp_result;

        if(expected_token.type() == rule_type::Text) {
            tmp_result = text_call(expected_token.value(), it, patterns, flags, index_it.next_level(), construction_index);
        } else if(expected_token.type() == rule_type::Disjunction) {
            tmp_result = disjunction_call(expected_token.children(), it, patterns, flags, index_it.next_level(), construction_index);
        } else {
            tmp_result = null_call(it, flags, index_it.next_level(), construction_index);
        }

        if(tmp_result.error.has_value()) {
            return call_result::from_error(tmp_result.error.value());
        }

        if(!tmp_result.confirmed) {
            return call_result();
        }

        args[i] = tmp_result.arg;



        if(flags.unconditional_transition) {
            if(i < conjunctions.size() - 1) {
                if(flags.verbose) { log << K_GRAM_LEVEL << __warning_color("++") << "\n"; }
                it->next();
            } else if(flags.verbose) {
                log << K_GRAM_LEVEL << __warning_color("++ aborted as last") << "\n";
            }
        } else {
            if(tmp_result.arg.contains_type<wall_e::lex::token>()) {
                if(flags.verbose) { log << K_GRAM_LEVEL << __warning_color("++") << "\n"; }
                it->next();
            } else if(flags.verbose) {
                log << K_GRAM_LEVEL << __err_color("++ aborted") << " arg: " << tmp_result.arg << "\n";
            }
        }

        if(tmp_result.no_increment_it) {
            if(flags.verbose) { log << K_GRAM_LEVEL << __warning_color("--") << "\n"; }
            it->goBack();
        }

        index_it = index_it.next_x();
        ++i;
    }

    if(flags.verbose) { log << K_GRAM_LEVEL << args << "\n"; }
    return call_result(args, true);
}

call_mono_result disjunction_call(const rule_vec &disjunctions, token_iterator *it, const pattern_list &patterns, const __flags_private &flags, index_iter index_it, std::size_t *construction_index) {
    K_GRAM_CHECK_LEVEL
    if(flags.verbose) { log << K_GRAM_LEVEL << __warning_color("|") << "\n"; }

    token_iterator it_backup = *it;
    str_vec expectations;

    for(const auto& particular_case : disjunctions) {
        std::stringstream ess;
        ess << '\'' << particular_case << '\'';

        expectations.push_back(ess.str());

        if(flags.verbose) {
            log << K_GRAM_LEVEL << "case: " << ess.str() << std::endl;
            log << K_GRAM_LEVEL << "curr it: " << *it << std::endl;
        }

        call_mono_result tmp_result;

        if(particular_case.isNull()) {
            tmp_result = null_call(it, flags, index_it.next_level(), construction_index);
        } else {
            if(particular_case.type() == rule_type::Text) {
                tmp_result = text_call(particular_case.value(), it, patterns, flags, index_it.next_level(), construction_index);
            } else if(particular_case.type() == rule_type::Disjunction) {
                if(flags.verbose) { log << K_GRAM_LEVEL << __err_color("deprecated operation Disjunction in Disjunction\n"); }

                tmp_result = disjunction_call(particular_case.children(), it, patterns, flags, index_it.next_level(), construction_index);
            } else if(particular_case.type() == rule_type::Conjunction) {
                if(flags.verbose) { log << K_GRAM_LEVEL << __err_color("deprecated operation Conjunction in Disjunction\n"); }

                //MAY BE REMOVED
                tmp_result = call_mono_result_cast(conjunction_call(particular_case.children(), it, patterns, flags, index_it.next_level(), construction_index));
                //--- -- -------
            } else {
                tmp_result = null_call(it, flags, index_it.next_level(), construction_index);
            }
        }

        if(tmp_result.error.has_value()) {
            return call_mono_result::from_error(tmp_result.error.value());
        }

        if(tmp_result.confirmed) {
            if(flags.verbose) { log << K_GRAM_LEVEL << "choise: " << tmp_result.arg << "\n"; }
            return tmp_result;
        } else {
            *it = it_backup;
        }
    }
    if(flags.verbose) { log << K_GRAM_LEVEL << __err_color("no choise, expectations: ") << __err_color(lex::join(expectations, ", ")) << "\n"; }
    return call_mono_result::from_error(error("disj faild, expectations: " + lex::join(expectations, " or "), error::err, error::gramatic, errors::failed_disjunction, it->segment()));
}


call_mono_result call(const pattern &p, token_iterator *it, const pattern_list &patterns, const __flags_private &flags, index_iter index_it, std::size_t *construction_index) {
    K_GRAM_CHECK_LEVEL

    auto rule = flags.simplification_function(p.gram_rule());

    if(flags.verbose) { log << K_GRAM_LEVEL << ">> pattern: " << __header_color(p.name()) << " " << p.gram_rule() << " -> " << __header_color(rule) << " current word: " << __warning_color(*it) << ")\n"; }

    call_result result;
    if(rule.type() == rule_type::Text) {
        result = call_result_cast(text_call(rule.value(), it, patterns, flags, index_it.next_pattern_level(), construction_index));
    } else if(rule.type() == rule_type::Disjunction) {
        result = call_result_cast(disjunction_call(rule.children(), it, patterns, flags, index_it.next_pattern_level(), construction_index));
    } else if(rule.type() == rule_type::Conjunction) {
        result = conjunction_call(rule.children(), it, patterns, flags, index_it.next_pattern_level(), construction_index);
    } else if(rule.isNull()) {
        result = call_result_cast(null_call(it, flags, index_it.next_pattern_level(), construction_index));
    }

    if(result.error.has_value()) {
        const auto err = result.error.value();
        //if(err.kind() == errors::failed_disjunction) {
        //    return call_mono_result::from_error(error("unmatched pattern. expected: " + p.name(), error::err, error::gramatic, errors::unmatched_pattern, it->segment()));
        //} else {
            return call_mono_result::from_error(err);
        //}
    }

    if(result.confirmed) {
        if(flags.verbose) { log << K_GRAM_LEVEL << "<< " << result.arg << std::endl; }
        return call_mono_result(p.callback(flags.use_default_parser)(result.arg, index_it.to_index((*construction_index)++)), true);
    }
    if(flags.verbose) { log << K_GRAM_LEVEL << "<< " << __err_color("<(0)_(0)>") << std::endl; }
    return call_mono_result();
}


either<error, argument> exec(const pattern_list &patterns, const lex::token_vec &tokens, const flags_list &flags, const std::function<rule(const rule &)> &simplification_function) {
    const auto verbose = __flags_private(flags, simplification_function).verbose;

    if(verbose) {
        log << __header_color("NEW PARSING") << std::endl;
    }

    __defer defer = __defer([](){ log.flush(); });
    std::size_t construction_index = 0;

    if(patterns.size() > 0 && tokens.size() > 0) {
        token_iterator it = tokens;
        auto result = call(patterns.front(), &it, patterns, __flags_private(flags, simplification_function), index_iter(), &construction_index);
        if(verbose && result.error.has_value()) {
            log << "PARSING ERR: " << result.error << std::endl;
        }
        return result;
    }
    return right(argument());
}

}
}
