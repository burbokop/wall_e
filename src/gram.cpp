#include "gram.h"

#include <filesystem>
#include <string.h>
#include <cassert>
#include <fstream>
#include <streambuf>
#include "color.h"

namespace wall_e {
namespace gram {

using namespace literals;

const color& pattern_color = magenta;
const color& rule_color = cyan;
const color& warn_color = yellow;
const color& err_color = red;

const uint32_t __recursion_max_level = 500;

//#ifdef __linux__
//std::ofstream flog = std::ofstream("wall_e.gram.ansi");
//boost::iostreams::stream log = boost::iostreams::tee(flog, std::cout);
//#else
//std::ofstream log = std::ofstream("wall_e.gram.ansi");
//std::ofstream table_log = std::ofstream("wall_e.gram_table.ansi");
//#endif

std::string __level_str(std::size_t level) {
    std::string str;
    for(uint32_t i = 0; i < level; ++i) {
        str += " ";
    }
    return str;
}

struct __recursion_error_ubiq {
    inline static const error err = error("recursion error", error::Err, error::Gramatic, errors::Recursion);

    operator call_result () { return call_result::from_error(err); }
    operator call_mono_result () { return call_mono_result::from_error(err); }
};

#define K_GRAM_CHECK_LEVEL \
    if(index_it.level > __recursion_max_level) { \
        return __recursion_error_ubiq(); \
    }

#define K_GRAM_LEVEL __level_str(index_it.level)

//struct to_str {
//
//}


struct __flags_private {
    bool use_default_parser = false;
    bool unconditional_transition = false;
    std::function<rule(const rule&)> simplification_function;
    __flags_private(const flags_list &flags = flags_list(), const std::function<rule(const rule&)> &simplification_function = {}) {
        this->simplification_function = simplification_function;
        for(auto f : flags) {
            if(f == flags::UseDefaultParser) {
                use_default_parser = true;
            } else if(f == flags::UnconditionalTransition) {
                unconditional_transition = true;
            }
        }
    }
};


call_mono_result text_call(const std::string &rule_text,
        token_iterator *it,
        const pattern_list &patterns, const environment* env,
        const __flags_private &flags,
        index_iter index_it,
        std::size_t *construction_index,
        log *l
        ) {
    K_GRAM_CHECK_LEVEL
    if(l) {
        l->add_step(true, (std::stringstream() << rule_color("t ") << *it << " <- " << rule_text).str(), "T: " + rule_text, "", it, index_it.level, rule_color);
    }

    if(rule_text == "-") {
        if(l) l->add_step(false, "item: skip (no increment iter)", "-", "-", it, index_it.level);
        return call_mono_result({}, true, true);
    }

    auto item = determine_item(it, patterns, rule_text);
    if(l) {
        const auto ss = (std::stringstream() << "item: " << item);
        l->add_step(false, ss.str(), ss.str(), ss.str(), it, index_it.level);
    }

    if(item.type() == item::Token) {
        return call_mono_result({ item.token() }, true);
    } else if(item.type() == item::Pattern) {
        return call(item.gram_pattern(), it, patterns, env, flags, index_it.next_level(), construction_index, l);
    }
    return call_mono_result();
    //return call_mono_result::from_error(error("udef token or pattern '" + rule_text + "'", error::err, it->segment()));
}

call_mono_result null_call(token_iterator *it, const environment* env, const __flags_private &flags, index_iter index_it, std::size_t *construction_index, log *log) {
    K_GRAM_CHECK_LEVEL
    if(log) {
        log->add_step(true, rule_color("n"), "N", "", it, index_it.level, rule_color);
    }
    const bool alwaysConfirm = false;
    return call_mono_result(wall_e::variant(), alwaysConfirm ? true : !it->isValid());
}


call_result conjunction_call(const rule_vec &conjunctions,
        token_iterator *it,
        const pattern_list &patterns, const environment* env,
        const __flags_private &flags,
        index_iter index_it,
        std::size_t *construction_index,
        log *l
        ) {
    K_GRAM_CHECK_LEVEL
    if(l) {
        l->add_step(true, (std::stringstream() << rule_color("&") << conjunctions).str(), "&: " + std::to_string(conjunctions.size()), (std::stringstream() << conjunctions).str(), it, index_it.level, rule_color);
    }

    size_t i = 0;
    arg_vector args;
    args.resize(conjunctions.size());

    for(const auto& expected_token : conjunctions) {
        call_mono_result tmp_result;

        if(expected_token.type() == rule_type::Text) {
            tmp_result = text_call(expected_token.value(), it, patterns, env, flags, index_it.next_level(), construction_index, l);
        } else if(expected_token.type() == rule_type::Disjunction) {
            tmp_result = disjunction_call(expected_token.children(), it, patterns, env, flags, index_it.next_level(), construction_index, l);
        } else {
            tmp_result = null_call(it, env, flags, index_it.next_level(), construction_index, l);
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
                if(l) { l->add_step(false, rule_color("++"), "++", "", it, index_it.level, rule_color); }
                it->next();
            } else if(l) {
                l->add_step(false, rule_color("++ aborted as last"), "++ aborted as last", "", it, index_it.level, rule_color);
            }
        } else {
            if(tmp_result.arg.contains_type<wall_e::lex::token>()) {
                if(l) { l->add_step(false, rule_color("++"), "++", "", it, index_it.level, rule_color); }
                it->next();
            } else if(l) {
                l->add_step(true, (std::stringstream() << err_color("++ aborted") << " arg: " << tmp_result.arg).str(), "++ aborted", tmp_result.arg.to_string(), it, index_it.level, err_color);
            }
        }

        if(tmp_result.no_increment_it) {
            if(l) { l->add_step(false, rule_color("--"), "--", "", it, index_it.level, rule_color); }
            it->goBack();
        }

        index_it = index_it.next_x();
        ++i;
    }

    if(l) { l->add_step(false, (std::stringstream() << "end& " << args).str(), "end&", (std::stringstream() << args).str(), it, index_it.level); }
    return call_result(args, true);
}

call_mono_result disjunction_call(const rule_vec &disjunctions,
        token_iterator *it,
        const pattern_list &patterns, const environment* env,
        const __flags_private &flags,
        index_iter index_it,
        std::size_t *construction_index,
        log *l
        ) {
    K_GRAM_CHECK_LEVEL
    if(l) {
        l->add_step(true, rule_color("|"), "|:" + std::to_string(disjunctions.size()), (std::stringstream() << disjunctions).str(), it, index_it.level, rule_color);
    }

    token_iterator it_backup = *it;
    str_vec expectations;

    for(const auto& particular_case : disjunctions) {
        std::stringstream ess;
        ess << '\'' << particular_case << '\'';

        expectations.push_back(ess.str());

        if(l) l->add_step(false, "case: " + ess.str(), "case: " + ess.str(), "", it, index_it.level);

        call_mono_result tmp_result;

        if(particular_case.isNull()) {
            tmp_result = null_call(it, env, flags, index_it.next_level(), construction_index, l);
        } else {
            if(particular_case.type() == rule_type::Text) {
                tmp_result = text_call(particular_case.value(), it, patterns, env, flags, index_it.next_level(), construction_index, l);
            } else if(particular_case.type() == rule_type::Disjunction) {
                if(l) {
                    const auto& msg = "deprecated operation Disjunction in Disjunction";
                    l->add_step(false, warn_color(msg), "deprecated warn", msg, it, index_it.level, warn_color);
                }

                tmp_result = disjunction_call(particular_case.children(), it, patterns, env, flags, index_it.next_level(), construction_index, l);
            } else if(particular_case.type() == rule_type::Conjunction) {
                if(l) {
                    const auto& msg = "deprecated operation Conjunction in Disjunction";
                    l->add_step(false, warn_color(msg), "deprecated warn", msg, it, index_it.level, warn_color);
                }

                //MAY BE REMOVED
                tmp_result = call_mono_result_cast(conjunction_call(particular_case.children(), it, patterns, env, flags, index_it.next_level(), construction_index, l));
                //--- -- -------
            } else {
                tmp_result = null_call(it, env, flags, index_it.next_level(), construction_index, l);
            }
        }

        if(tmp_result.error.has_value()) {
            return call_mono_result::from_error(tmp_result.error.value());
        }

        if(tmp_result.confirmed) {
            if(l) {
                const auto ss = (std::stringstream() << "choise: " << tmp_result.arg);
                l->add_step(false, ss.str(), ss.str(), ss.str(), it, index_it.level);
            }
            return tmp_result;
        } else {
            *it = it_backup;
        }
    }
    if(l) {
        const auto ss = std::stringstream() << err_color("no choise, expectations: ") << err_color(expectations.join(", "));
        l->add_step(true, ss.str(), "no choise", (std::stringstream() << "expectations: " << expectations).str(), it, index_it.level, err_color);
    }
    return call_mono_result::from_error(error("disjunction faild, expectations: " + expectations.join(" or "), error::Err, error::Gramatic, errors::FailedDisjunction, it->segment()));
}


call_mono_result call(const pattern &p,
        token_iterator *it,
        const pattern_list &patterns, const environment* env,
        const __flags_private &flags,
        index_iter index_it,
        std::size_t *construction_index,
        log *l
        ) {
    K_GRAM_CHECK_LEVEL

    auto rule = flags.simplification_function(p.gram_rule());

    if(l) {
        const auto ss = std::stringstream() << ">> pattern: " << pattern_color(p.name()) << " " << p.gram_rule() << " -> " << pattern_color(rule) << " current word: " << pattern_color(*it) << ")";
        l->add_step(true, ss.str(), "P: " + p.name(), (std::stringstream() << p).str(), it, index_it.level, pattern_color);
    }

    call_result result;
    if(rule.type() == rule_type::Text) {
        result = call_result_cast(text_call(rule.value(), it, patterns, env, flags, index_it.next_pattern_level(), construction_index, l));
    } else if(rule.type() == rule_type::Disjunction) {
        result = call_result_cast(disjunction_call(rule.children(), it, patterns, env, flags, index_it.next_pattern_level(), construction_index, l));
    } else if(rule.type() == rule_type::Conjunction) {
        result = conjunction_call(rule.children(), it, patterns, env, flags, index_it.next_pattern_level(), construction_index, l);
    } else if(rule.isNull()) {
        result = call_result_cast(null_call(it, env, flags, index_it.next_pattern_level(), construction_index, l));
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
        if(l) { l->add_step(false, (std::stringstream() << "<< " << result.arg).str(), "<<", (std::stringstream() << result.arg).str(), it, index_it.level); }
        return call_mono_result(p.callback(flags.use_default_parser)(result.arg, index_it.to_index((*construction_index)++), env), true);
    }
    if(l) { l->add_step(true, "<< " + err_color("<(0)_(0)>"), "<< " + p.name(), "No match found for pattern: '" + p.name() + "'. exiting...", it, index_it.level, err_color); }
    return call_mono_result();
}


either<error, argument> exec(
        const pattern_list &patterns,
        const lex::token_vec &tokens,
        const wall_e::gram::environment* env,
        const flags_list &flags,
        const std::function<rule(const rule &)> &simplification_function,
        log *l
        ) {

    /*
    static constexpr std::size_t min_w = 16;
    const auto ms = tokens
            .map_member<std::string>(&lex::token::name)
            .aligned_left(min_w, wall_e::StripAndReplaceByDots);


    static std::size_t next_log_num = 0;
    const auto log_num = next_log_num++;

    std::ofstream log;
    std::ofstream table_log;
    std::ofstream table_csv_log;

    auto dbg_ctx = dbg_context {
        .log = log,
        .table_log = table_log,
        .table_csv_log = table_csv_log,
        .table_col_width = ms.size() > 0 ? ms[0].size() : 0,
        .table_col_count = ms.size()
    };

    if(verbose) {
        std::cout << "GRAM::EXEC: " << log_num << " verbose: " << verbose << std::endl;
        std::filesystem::create_directory("logs");
        log.open("./logs/wall_e.gram_" + std::to_string(log_num) + ".ansi");
        table_log.open("./logs/wall_e.gram_table_" + std::to_string(log_num) + ".ansi");
        table_csv_log.open("./logs/wall_e.gram_table_" + std::to_string(log_num) + ".csv");

        dbg_ctx.log << __header_color("NEW PARSING") << std::endl;
        dbg_ctx.add_table_log(ms, tokens.map_member<std::string>(&lex::token::name));
    }

    const auto& flush_defer = defer([&](){
        if(log.is_open()) log.flush();
        if(table_log.is_open()) table_log.flush();
        if(table_csv_log.is_open()) table_csv_log.flush();
    });
    */

    if(l) l->set_tokens(tokens);

    std::size_t construction_index = 0;

    if(patterns.size() > 0 && tokens.size() > 0) {
        token_iterator it = tokens;
        return call(patterns.front(), &it, patterns, env, __flags_private(flags, simplification_function), index_iter(), &construction_index, l);
    }
    return right(argument());
}
/*
void dbg_context::add_table_log(const wall_e::str_vec& aligned, const wall_e::str_vec& not_aligned) {
    table_log << aligned.join("|") << std::endl;
    table_log << std::string(aligned.size() + aligned.map_member_func<std::size_t>(&std::string::size).sum(), '-') << std::endl;
    table_csv_log << not_aligned.join(";") << std::endl;
}

void dbg_context::add_table_log(token_iterator *it, const std::string &s, wall_e::color::color_t c) {
    wall_e::str_vec aligned;
    wall_e::str_vec not_aligned;
    aligned.resize(table_col_count);
    not_aligned.resize(table_col_count);
    for(std::size_t i = 0; i < table_col_count; ++i) {
        if(i == it->offset()) {
            if(c != wall_e::color::Black) {
                aligned[i] = c.cover(align_left(s, table_col_width, wall_e::StripAndReplaceByDots));
            } else {
                aligned[i] = align_left(s, table_col_width, wall_e::StripAndReplaceByDots);
            }
            not_aligned[i] = s;
        } else {
            aligned[i] = std::string(table_col_width, ' ');
        }
    }
    add_table_log(aligned, not_aligned);
}
*/
}
}
