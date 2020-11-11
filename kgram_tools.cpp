#include "kgram_tools.h"

#include <iostream>
#include <queue>


std::function<kgram_argument_t (kgram_arg_vector_t)> kgram_pattern_t::callback(bool __default) const {
    if(__default) {
        return __default_processor;
    }
    return m_callback;
}


kgram_pattern_t &operator<<(kgram_pattern_t &pattern, const kgram_rule_t &rule) {
    pattern.m_rule = rule;
    return pattern;
}

kgram_pattern_t operator<<(kgram_pattern_t pattern, const kgram_rule_t &rule) {
    pattern.m_rule = rule;
    return pattern;
}

void kgram_print_rule(const kgram_rule_t &rule) {
    std::cout << "conj: " << kgram_rule_type_t::Conjunction << " disj: " << kgram_rule_type_t::Disjunction << " { ";
    kgram_print_node(rule, [](auto value) {
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


kgram_rule_t operator &(
        const kgram_rule_t &rule1,
        const kgram_rule_t &rule2) {
    return kgram_rule_t(kgram_rule_type_t::Conjunction, rule1, rule2);
}

kgram_rule_t operator |(
        const kgram_rule_t &rule1,
        const kgram_rule_t &rule2) {
    return kgram_rule_t(kgram_rule_type_t::Disjunction, rule1, rule2);
}


void kgram_print_pattern(const kgram_pattern_t &pattern) {
    std::cout << "parrern { " << pattern.name() << " } << ";
    kgram_print_rule(pattern.rule());
}

bool kgram_pattern_t::isValid() const { return m_isValid; }

std::string kgram_pattern_t::to_string(const std::list<kgram_pattern_t> &list) {
    std::string result;
    for(auto l : list) {
        std::stringstream ss;
        ss << kgram_simplify_rule(l.rule());
        result += l.name() + "\n\t<< " + ss.str() + "\n\n";
    }
    return result;
}

kgram_rule_t kgram_pattern_t::rule() const { return m_rule; }


kgram_rule_transition_t kgram_simplify_rule_internal__(const kgram_rule_t &rule, kgram_rule_transition_t::type_t method);
kgram_rule_type_t::enum_t kgram_simplify_rule_last_rule_type_internal__ = kgram_rule_type_t::Undefined;
kgram_rule_t kgram_simplify_rule(const kgram_rule_t &rule, kgram_rule_transition_t::type_t method) {
    if(method == kgram_rule_transition_t::Auto) {
        kgram_rule_t c = rule;
        std::queue<kgram_rule_transition_t::type_t> queue;
        queue.push(kgram_rule_transition_t::ConjunctionDisjunction);
        queue.push(kgram_rule_transition_t::DoubleConjunction);
        queue.push(kgram_rule_transition_t::DoubleDisjunction);

        while (queue.size() > 0) {
            auto cm = queue.front();
            queue.pop();

            while (true) {
                auto nc = kgram_simplify_rule(c, cm);
                if(c == nc)
                    break;
                c = nc;
            }
        }
        return c;
    } else {
        kgram_simplify_rule_last_rule_type_internal__ = kgram_rule_type_t::Undefined;
        return kgram_simplify_rule_internal__(rule, method).rule;
    }
}

kgram_rule_transition_t kgram_simplify_rule_internal__(const kgram_rule_t &rule, kgram_rule_transition_t::type_t method) {
    kgram_rule_transition_t result;
    if(rule.isNull()) {
        return result;
    }

    switch (rule.type()) {
    case kgram_rule_type_t::Text:
        result.rule = rule.value();
        kgram_simplify_rule_last_rule_type_internal__ = kgram_rule_type_t::Text;
        break;
    case kgram_rule_type_t::Conjunction: {
        auto c = rule.children();
        if(kgram_simplify_rule_last_rule_type_internal__ == kgram_rule_type_t::Disjunction) {

        } else if(kgram_simplify_rule_last_rule_type_internal__ == kgram_rule_type_t::Conjunction) {
            if(method == kgram_rule_transition_t::DoubleConjunction)
                result.type = kgram_rule_transition_t::DoubleConjunction;
        }
        kgram_simplify_rule_last_rule_type_internal__ = kgram_rule_type_t::Conjunction;

        std::vector<kgram_rule_t> tmpvec;
        kgram_rule_transition_t tmptr;
        bool was = false;

        for(auto cc : c) {
            auto ff = kgram_simplify_rule_internal__(cc, method);
            if(ff.type == kgram_rule_transition_t::ConjunctionDisjunction) {
                tmptr = ff;
                was = true;
            } else if(ff.type == kgram_rule_transition_t::DoubleConjunction) {
                auto ffc = ff.rule.children();
                for(auto offa : ffc) {
                    tmpvec.push_back(offa);
                }
            } else {
                tmpvec.push_back(ff.rule);
            }
        }

        if(was) {
            std::vector<kgram_rule_t> r2;
            auto ffc = tmptr.rule.children();
            for(auto offa : ffc) {
                std::vector<kgram_rule_t> RULE;

                RULE.push_back(offa);
                for(auto rr : tmpvec) {
                    RULE.push_back(rr);
                }

                kgram_rule_t ttt(kgram_rule_type_t::Conjunction, RULE);

                r2.push_back(ttt);
            }

            result.rule = kgram_rule_t(kgram_rule_type_t::Disjunction, r2);
        } else {
            result.rule = kgram_rule_t(kgram_rule_type_t::Conjunction, tmpvec);
        }
        break;
    }
    case kgram_rule_type_t::Disjunction: {
        auto c = rule.children();
        if(kgram_simplify_rule_last_rule_type_internal__ == kgram_rule_type_t::Conjunction) {
            if(method == kgram_rule_transition_t::ConjunctionDisjunction)
                result.type = kgram_rule_transition_t::ConjunctionDisjunction;

        } else if(kgram_simplify_rule_last_rule_type_internal__ == kgram_rule_type_t::Disjunction) {
            if(method == kgram_rule_transition_t::DoubleDisjunction)
                result.type = kgram_rule_transition_t::DoubleDisjunction;
        }

        kgram_simplify_rule_last_rule_type_internal__ = kgram_rule_type_t::Disjunction;

        std::vector<kgram_rule_t> r;
        for(auto cc : c) {
            auto ffg = kgram_simplify_rule_internal__(cc, method);

            if(ffg.type == kgram_rule_transition_t::DoubleDisjunction) {
                auto ffc = ffg.rule.children();
                for(auto offa : ffc) {
                    r.push_back(offa);
                }
            } else {
                r.push_back(ffg.rule);
            }
        }
        result.rule = kgram_rule_t(kgram_rule_type_t::Disjunction, r);

        break;
    }
    case kgram_rule_type_t::Undefined:
        return kgram_rule_transition_t();
        break;
    }
    return result;
}


kgram_pattern_t &operator<<(kgram_pattern_t &pattern, std::function<kgram_argument_t (kgram_arg_vector_t)> callback) { pattern.m_callback = callback; return pattern; }
kgram_pattern_t operator<<(kgram_pattern_t pattern, std::function<kgram_argument_t (kgram_arg_vector_t)> callback) { pattern.m_callback = callback; return pattern; }

kgram_argument_t kgram_make_argument() { kgram_argument_t result; return result; }

std::ostream &operator<<(std::ostream &stream, const kgram_token_iterator &it) {
    if(it.isValid()) {
        stream << "(" << it.data().position << ", " << it.data().name << ", " << it.data().text << ", offset: " << it.offset() << ")";
    } else {
        stream << "invalid iterator";
    }
    return stream;
}

std::string kgram_rule_to_string(const kgram_rule_t &rule) {
    std::string result;

    if(rule.type() == kgram_rule_type_t::Text) {
        result = rule.value();
    } else if(rule.type() == kgram_rule_type_t::Conjunction) {
        auto c = rule.children();
        result += "(";
        size_t i = 0;
        for(auto cc : c) {
            result += kgram_rule_to_string(cc);
            if(i < c.size() - 1)
                result += " && ";
            ++i;
        }
        result += ")";
    } else if(rule.type() == kgram_rule_type_t::Disjunction) {
        auto c = rule.children();
        result += "(";
        size_t i = 0;
        for(auto cc : c) {
            result += kgram_rule_to_string(cc);
            if(i < c.size() - 1)
                result += " || ";
            ++i;
        }
        result += ")";
    }

    return result;
}

kgram_call_result kgram_call_result_cast(const kgram_call_mono_result &value) {
    kgram_call_result result;
    result.arg.resize(1);
    result.arg[0] = value.arg;
    result.confirmed = value.confirmed;
    return result;
}

kgram_call_mono_result kgram_call_mono_result_cast(const kgram_call_result &value) {
    kgram_call_mono_result result;
    result.arg = value.arg;
    result.confirmed = value.confirmed;
    return result;
}
