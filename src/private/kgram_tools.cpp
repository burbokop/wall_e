#include "kgram_tools.h"

#include <iostream>
#include <queue>

namespace wall_e {
namespace gram {

std::function<argument (arg_vector)> kgram_pattern_t::callback(bool __default) const {
    if(__default) {
        return __default_processor;
    }
    return m_callback;
}


kgram_pattern_t &operator<<(kgram_pattern_t &pattern, const rule &r) {
    pattern.m_gram_rule = r;
    return pattern;
}

kgram_pattern_t operator<<(kgram_pattern_t pattern, const rule &r) {
    pattern.m_gram_rule = r;
    return pattern;
}

void print_rule(const rule &r) {
    std::cout << "conj: " << rule_type::Conjunction << " disj: " << rule_type::Disjunction << " { ";
    print_node(r, [](auto value) {
        std::cout << value;
    }, false);
    std::cout << " }\n";
}


std::string kgram_to_lowercase(std::string str) {
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


void kgram_print_pattern(const kgram_pattern_t &pattern) {
    std::cout << "parrern { " << pattern.name() << " } << ";
    print_rule(pattern.gram_rule());
}

bool kgram_pattern_t::isValid() const { return m_isValid; }

std::string kgram_pattern_t::to_string(const std::list<kgram_pattern_t> &list) {
    std::string result;
    for(auto l : list) {
        std::stringstream ss;
        ss << simplify_rule(l.gram_rule());
        result += l.name() + "\n\t<< " + ss.str() + "\n\n";
    }
    return result;
}

rule kgram_pattern_t::gram_rule() const { return m_gram_rule; }


rule_transition kgram_simplify_rule_internal__(const rule &r, rule_transition::enum_t method);
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
        return kgram_simplify_rule_internal__(r, method).r;
    }
}

rule_transition kgram_simplify_rule_internal__(const rule &r, rule_transition::enum_t method) {
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

        std::vector<rule> tmpvec;
        rule_transition tmptr;
        bool was = false;

        for(auto cc : c) {
            auto ff = kgram_simplify_rule_internal__(cc, method);
            if(ff.type == rule_transition::ConjunctionDisjunction) {
                tmptr = ff;
                was = true;
            } else if(ff.type == rule_transition::DoubleConjunction) {
                auto ffc = ff.r.children();
                for(auto offa : ffc) {
                    tmpvec.push_back(offa);
                }
            } else {
                tmpvec.push_back(ff.r);
            }
        }

        if(was) {
            std::vector<rule> r2;
            auto ffc = tmptr.r.children();
            for(auto offa : ffc) {
                std::vector<rule> RULE;

                RULE.push_back(offa);
                for(auto rr : tmpvec) {
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

        std::vector<rule> r;
        for(auto cc : c) {
            auto ffg = kgram_simplify_rule_internal__(cc, method);

            if(ffg.type == rule_transition::DoubleDisjunction) {
                auto ffc = ffg.r.children();
                for(auto offa : ffc) {
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


kgram_pattern_t &operator<<(kgram_pattern_t &pattern, std::function<argument (arg_vector)> callback) { pattern.m_callback = callback; return pattern; }
kgram_pattern_t operator<<(kgram_pattern_t pattern, std::function<argument (arg_vector)> callback) { pattern.m_callback = callback; return pattern; }

argument kgram_make_argument() { argument result; return result; }

std::ostream &operator<<(std::ostream &stream, const kgram_token_iterator &it) {
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
        for(auto cc : c) {
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
        for(auto cc : c) {
            result += rule_to_string(cc);
            if(i < c.size() - 1)
                result += " || ";
            ++i;
        }
        result += ")";
    }

    return result;
}

call_result kgram_call_result_cast(const call_mono_result &value) {
    call_result result;
    result.arg.resize(1);
    result.arg[0] = value.arg;
    result.confirmed = value.confirmed;
    return result;
}

call_mono_result kgram_call_mono_result_cast(const call_result &value) {
    call_mono_result result;
    result.arg = value.arg;
    result.confirmed = value.confirmed;
    return result;
}
}
}