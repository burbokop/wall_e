#include "gram_private.h"

#include <queue>
#include "../gram.h"

namespace wall_e {
namespace gram {

pattern::processor pattern::callback(bool useDefault) const {
    if(useDefault) {
        return default_processor;
    }
    return m_callback;
}


pattern &operator<<(pattern &pattern, const rule &r) {
    pattern.m_gram_rule = r;
    return pattern;
}

pattern operator<<(pattern pattern, const rule &r) {
    pattern.m_gram_rule = r;
    return pattern;
}

std::string to_lowercase(std::string str) {
    std::for_each(str.begin(), str.end(), [](char & c){
        c = ::tolower(c);
    });
    return str;
}


rule operator &(
        const rule &rule1,
        const rule &rule2) {
    return rule(rule_type::Conjunction, rule1, rule2);
}

rule operator |(
        const rule &rule1,
        const rule &rule2) {
    return rule(rule_type::Disjunction, rule1, rule2);
}

bool pattern::isValid() const { return m_isValid; }

std::string pattern::to_string(const wall_e::list<pattern> &list) {
    std::string result;
    for(const auto& l : list) {
        std::stringstream ss;
        ss << simplify_rule(l.gram_rule());
        result += l.name() + "\n\t<< " + ss.str() + "\n\n";
    }
    return result;
}

either<error, pattern> pattern::from_str(const std::string &string) {
    const auto p = lex::split<str_pair>(string, std::regex("[:]|<<"));
    return rule_from_str(lex::trim(lex::remove_character(p.second, '\n'))).map<pattern>([&p](const rule& rule){
        return pattern(lex::trim(lex::remove_character(p.first, '\n')))
                    << rule;
    });
}

wall_e::list<either<error, pattern> > pattern::list_from_str(const std::string &string) {
     const auto lines = lex::split<str_list>(string,  std::regex("[\n]"));
     wall_e::list<either<error, pattern>> result;
     for(const auto& line : lines) {
        result.push_back(from_str(line));
     }
     return result;
}

pattern pattern::simplified() const {
    auto copy = pattern(*this);
    copy.m_gram_rule = smp::simplify(m_gram_rule);
    return copy;
}

wall_e::list<pattern> pattern::simplified(const wall_e::list<pattern> &list) {
    pattern_list result;
    for(const auto& l : list) {
        result.push_back(l.simplified());
    }
    return result;
}

pattern::processor pattern::pass_argument(size_t i) {
    return [i](const arg_vector &args, const index&, const environment*) -> wall_e::gram::argument {
        if(args.size() > i) {
            return args[i];
        }
        return {};
    };
}

std::ostream &operator<<(std::ostream &stream, const pattern &p) {
    return stream << p.name() << " << " << p.gram_rule();
}

rule pattern::gram_rule() const { return m_gram_rule; }


rule_transition __simplify_rule_internal(const rule &r, rule_transition::enum_t method);
rule_type::enum_t kgram_simplify_rule_last_rule_type_internal__ = rule_type::Undefined;
rule simplify_rule(const rule &r, rule_transition::enum_t method) {
    if(method == rule_transition::Auto) {
        rule c = r;
        std::queue<rule_transition::enum_t> queue;
        queue.push(rule_transition::ConjunctionDisjunction);
        queue.push(rule_transition::DoubleConjunction);
        queue.push(rule_transition::DoubleDisjunction);

        while (queue.size() > 0) {
            auto cm = queue.front();
            queue.pop();

            while (true) {
                auto nc = simplify_rule(c, cm);
                if(c == nc)
                    break;
                c = nc;
            }
        }
        return c;
    } else {
        kgram_simplify_rule_last_rule_type_internal__ = rule_type::Undefined;
        return __simplify_rule_internal(r, method).r;
    }
}

rule_transition __simplify_rule_internal(const rule &r, rule_transition::enum_t method) {
    rule_transition result;
    if(r.isNull()) {
        return result;
    }

    switch (r.type()) {
    case rule_type::Text:
        result.r = r.value();
        kgram_simplify_rule_last_rule_type_internal__ = rule_type::Text;
        break;
    case rule_type::Conjunction: {
        auto c = r.children();
        if(kgram_simplify_rule_last_rule_type_internal__ == rule_type::Disjunction) {

        } else if(kgram_simplify_rule_last_rule_type_internal__ == rule_type::Conjunction) {
            if(method == rule_transition::DoubleConjunction)
                result.type = rule_transition::DoubleConjunction;
        }
        kgram_simplify_rule_last_rule_type_internal__ = rule_type::Conjunction;

        rule_vec tmpvec;
        rule_transition tmptr;
        bool was = false;

        for(const auto& cc : c) {
            auto ff = __simplify_rule_internal(cc, method);
            if(ff.type == rule_transition::ConjunctionDisjunction) {
                tmptr = ff;
                was = true;
            } else if(ff.type == rule_transition::DoubleConjunction) {
                auto ffc = ff.r.children();
                for(const auto& offa : ffc) {
                    tmpvec.push_back(offa);
                }
            } else {
                tmpvec.push_back(ff.r);
            }
        }

        if(was) {
            rule_vec r2;
            auto ffc = tmptr.r.children();
            for(const auto& offa : ffc) {
                rule_vec RULE;

                RULE.push_back(offa);
                for(const auto& rr : tmpvec) {
                    RULE.push_back(rr);
                }

                rule ttt(rule_type::Conjunction, RULE);

                r2.push_back(ttt);
            }

            result.r = rule(rule_type::Disjunction, r2);
        } else {
            result.r = rule(rule_type::Conjunction, tmpvec);
        }
        break;
    }
    case rule_type::Disjunction: {
        auto c = r.children();
        if(kgram_simplify_rule_last_rule_type_internal__ == rule_type::Conjunction) {
            if(method == rule_transition::ConjunctionDisjunction)
                result.type = rule_transition::ConjunctionDisjunction;

        } else if(kgram_simplify_rule_last_rule_type_internal__ == rule_type::Disjunction) {
            if(method == rule_transition::DoubleDisjunction)
                result.type = rule_transition::DoubleDisjunction;
        }

        kgram_simplify_rule_last_rule_type_internal__ = rule_type::Disjunction;

        rule_vec r;
        for(const auto& cc : c) {
            auto ffg = __simplify_rule_internal(cc, method);

            if(ffg.type == rule_transition::DoubleDisjunction) {
                auto ffc = ffg.r.children();
                for(const auto& offa : ffc) {
                    r.push_back(offa);
                }
            } else {
                r.push_back(ffg.r);
            }
        }
        result.r = rule(rule_type::Disjunction, r);

        break;
    }
    case rule_type::Undefined:
        return rule_transition();
        break;
    }
    return result;
}

pattern &operator<<(pattern &pattern, const pattern::processor &callback) { pattern.m_callback = callback; return pattern; }
pattern operator<<(pattern pattern, const pattern::processor &callback) { pattern.m_callback = callback; return pattern; }

std::ostream &operator<<(std::ostream &stream, const token_iterator &it) {
    if(it.isValid()) {
        stream << "(" << it.data().position << ", " << it.data().name << ", " << it.data().text << ", offset: " << it.offset() << ")";
    } else {
        stream << "invalid iterator";
    }
    return stream;
}

std::string rule_to_string(const rule &r) {
    std::string result;

    if(r.type() == rule_type::Text) {
        result = r.value();
    } else if(r.type() == rule_type::Conjunction) {
        auto c = r.children();
        result += "(";
        size_t i = 0;
        for(const auto& cc : c) {
            result += rule_to_string(cc);
            if(i < c.size() - 1)
                result += " && ";
            ++i;
        }
        result += ")";
    } else if(r.type() == rule_type::Disjunction) {
        auto c = r.children();
        result += "(";
        size_t i = 0;
        for(const auto& cc : c) {
            result += rule_to_string(cc);
            if(i < c.size() - 1)
                result += " || ";
            ++i;
        }
        result += ")";
    }

    return result;
}

call_result call_result_cast(const call_mono_result &value) {
    call_result result;
    result.arg.resize(1);
    result.arg[0] = value.arg;
    result.confirmed = value.confirmed;
    return result;
}

call_mono_result call_mono_result_cast(const call_result &value) {
    call_mono_result result;
    result.arg = value.arg;
    result.confirmed = value.confirmed;
    return result;
}



either<error, rule> rule_from_str(const std::string &string) {
    return exec({
        pattern("expression")
        << (rule("disj") | rule("term")),
        pattern("disj")
        << (rule("term") & "D" & "expression")
        << [](arg_vector args, const index&, const wall_e::gram::environment* env) -> argument {
            return binary_operator<rule>(args, std::bit_or<rule>(), env);
        },
        pattern("term")
        << (rule("conj") | "factor"),
        pattern("conj")
        << (rule("factor") & "C" & "term")
        << [](arg_vector args, const index&, const wall_e::gram::environment* env) -> argument {
            return binary_operator<rule>(args, std::bit_and<rule>(), env);
        },
        pattern("closed_expr")
        << (rule("OP") & "expression" & "EP")
        << pattern::pass_argument(1),
        pattern("factor")
        << (rule("closed_expr") | "rule"),
        pattern("rule")
        << (rule("W") | rule("NULL") | rule("SKIP"))
        << pattern::pass_token_if<wall_e::gram::rule>({ "W", "SKIP" })
    }, lex::make_tokents(string, std::string(), {
        { std::regex("[0]"), "NULL" },
        { std::regex("[-]"), "SKIP" },
        { std::regex("[a-zA-Z_][a-zA-Z0-9_]*"), "W" },
        { std::regex("[(]"), "OP" },
        { std::regex("[)]"), "EP" },
        { std::regex("[&]"), "C" },
        { std::regex("[|]"), "D" },
        { std::regex("[ \t\n]+"), lex::special::ignore }
    }), nullptr, {
        gram::UnconditionalTransition
    })
            .map<rule>([](const variant& _) { return _.value_or<gram::rule>(); });
}

item item::from_token(const lex::token &token) {
    item result;
    result.m_data = token;
    result.m_type = Token;
    return result;
}

item item::from_pattern(const pattern &pattern) {
    item result;
    result.m_data = pattern;
    result.m_type = Pattern;
    return result;
}

std::string item::typeString() const {
    switch (m_type) {
    case Token: return "Token";
    case Pattern: return "Pattern";
    case Undefined: return "Undefined";
    }
    return "Undefined";
}

std::ostream &operator<<(std::ostream &stream, const item &item) {
    stream << item.typeString();
    switch (item.type()) {
    case gram::item::Token: stream << ":" << item.token(); break;
    case gram::item::Pattern: stream << ":" << item.gram_pattern(); break;
    case gram::item::Undefined: break;
    }
    return stream;
}


}
}
