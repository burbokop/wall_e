#ifndef GRAM_TOOLS_H
#define GRAM_TOOLS_H

#include <string>
#include <variant>
#include <vector>
#include <functional>
#include <list>
#include <ostream>
#include <cassert>

#include "../lex.h"
#include "../models/variant.h"
#include "../models/node.h"
#include "../models/either.h"
#include "../models/index.h"

namespace wall_e {

namespace gram {

typedef wall_e::variant argument;
typedef wall_e::vec<argument> arg_vector;

template<typename T>
struct common_call_result {
    common_call_result() { }
    common_call_result(const T &arg, bool confirmed, bool no_increment_it = false) {
        this->arg = arg;
        this->confirmed = confirmed;
        this->no_increment_it = no_increment_it;
    }
    T arg;
    wall_e::opt<wall_e::error> error;
    bool confirmed = false;
    bool no_increment_it = false;

    static common_call_result<T> from_error(const wall_e::error& err) {
        common_call_result result;
        result.error = err;
        return result;
    }

    operator either<wall_e::error, T>() const {
        if(error) {
            return left(error.value());
        } else {
            return right(arg);
        }
    }
};

typedef common_call_result<argument> call_mono_result;
typedef common_call_result<arg_vector> call_result;

call_result call_result_cast(const call_mono_result &value);
call_mono_result call_mono_result_cast(const call_result &value);

struct rule_type {
    enum enum_t {
        Text = 0,
        Conjunction = 1,
        Disjunction = 2,
        Undefined = 3
    };
};

typedef node<rule_type::enum_t, rule_type::Text, std::string> rule;
typedef wall_e::vec<rule> rule_vec;

std::string rule_to_string(const rule &r);

rule operator &(
        const rule &rule1,
        const rule &rule2);

rule operator |(
        const rule &rule1,
        const rule &rule2);

struct rule_transition {
    enum enum_t {
        ConjunctionDisjunction = 0,
        DoubleConjunction,
        DoubleDisjunction,
        Auto,
        Undefined
    };
    rule r;
    enum_t type = Undefined;
};

rule simplify_rule(const rule &r, rule_transition::enum_t method = rule_transition::Auto);
inline rule simplify_rule_default(const rule &r) { return simplify_rule(r, rule_transition::DoubleConjunction); }

class environment {
    const std::string& m_uri;
public:
    environment(const std::string& uri) : m_uri(uri) {}
    const std::string& uri() const { return m_uri; }
    virtual bool verbose() const = 0;
    virtual ~environment() {}
};

class pattern {
    std::string m_name;
    rule m_gram_rule;
public:
    typedef std::function<argument(const arg_vector&, const wall_e::index&, const environment*)> processor;
    const static inline processor default_processor = [](const arg_vector &args, const wall_e::index&, const environment*) -> argument {
        if(args.size() > 0) {
            if(args.size() > 1) {
                return args;
            }
            return args[0];
        }
        return argument();
    };
    static processor pass_argument(size_t i);
    template<typename T>
    static processor pass_token(size_t i = 0) {
        return [i](const arg_vector &args) -> wall_e::gram::argument {
            if(args.size() > i) {
                if(args[i].contains_type<wall_e::lex::token>()) {
                    return T(args[i].value<wall_e::lex::token>().text);
                }
                return args[i];
            }
            return {};
        };
    }
    template<typename T>
    static processor pass_token_if(const wall_e::vec<std::string>& token_names, size_t i = 0) {
        return [i, token_names](const arg_vector &args, const index&, const environment*) -> argument {
            if(args.size() > i) {
                if(args[i].contains_type<lex::token>()) {
                    const auto token = args[i].value<lex::token>();
                    if(token_names.contains(token.name)) {
                        return T(token.text);
                    } else {
                        return T();
                    }
                }
                return args[i];
            }
            return {};
        };
    }

private:
    processor m_callback = default_processor;
    bool m_isValid = false;
public:
    pattern() {};
    pattern(const char* name) { m_name = name; m_isValid = true; };
    pattern(const std::string &name) { m_name = name; m_isValid = true; };
    std::string name() const { return m_name; };

    friend pattern &operator<< (pattern &pattern, const rule &rule);
    friend pattern &operator<< (pattern &pattern, const processor& callback);

    friend pattern operator<< (pattern pattern, const rule &rule);
    friend pattern operator<< (pattern pattern, const processor& callback);

    gram::rule gram_rule() const;
    processor callback(bool useDefault = false) const;
    bool isValid() const;
    static std::string to_string(const wall_e::list<pattern> &list);
    static either<error, pattern> from_str(const std::string &string);
    static wall_e::list<either<error, pattern> > list_from_str(const std::string &string);
    pattern simplified() const;

    static wall_e::list<pattern> simplified(const wall_e::list<pattern>& list);
};

typedef wall_e::list<pattern> pattern_list;

std::ostream &operator<<(std::ostream &stream, const pattern& p);

template<typename T>
pattern find_pattern(const T &pattens, const std::string name) {
    for(auto p : pattens) {
        if(p.name() == name)
            return p;
    }
    return pattern();
}

class item {
public:
    enum type_t {
        Token,
        Pattern,
        Undefined,
    };

private:
    type_t m_type = Undefined;
    std::variant<wall_e::lex::token, pattern> m_data;
public:
    item() { }

    static item from_token(const wall_e::lex::token &token);
    static item from_pattern(const pattern &pattern);

    wall_e::lex::token token() const {
        if(m_type == Token)
            return std::get<wall_e::lex::token>(m_data);
        return wall_e::lex::token();
    }
    gram::pattern gram_pattern() const {
        if(m_type == Pattern)
            return std::get<gram::pattern>(m_data);
        return pattern();
    }
    type_t type() const { return m_type; };
    std::string typeString() const;
};

std::ostream &operator<<(std::ostream &stream, const item& item);

class token_iterator {
    lex::token_vec::const_iterator it;
    lex::token_vec::const_iterator begin;
    lex::token_vec::const_iterator end;
public:
    inline token_iterator(const lex::token_vec &container) {
        it = container.begin();
        begin = container.begin();
        end = container.end();
    }

    inline bool isValid() const { return it >= begin && it < end; }

    inline bool next() {
        if(isValid()) {
            ++it;
            return true;
        }
        return false;
    }

    inline bool goBack() {
        if(isValid()) {
            --it;
            return true;
        }
        return false;
    }

    inline text_segment segment() const {
        if(isValid()) {
            return text_segment(it->segment());
        }
        return {};
    }

    inline wall_e::lex::token data() const { return (*it); }
    inline std::int64_t offset() const { return it - begin; }
};

std::ostream &operator<< (std::ostream &stream, const token_iterator &it);

template<typename pattern_container_T>
item determine_item(const token_iterator *it, const pattern_container_T &pattens, const std::string text) {
    if(it->isValid()) {
        if(it->data().name == text) {
            return item::from_token(it->data());
        }
    }
    auto p = find_pattern(pattens, text);
    if(p.isValid()) {
        return item::from_pattern(p);
    }
    return item();
}

std::string to_lowercase(std::string str);
either<error, rule> rule_from_str(const std::string &string);


template<typename T>
wall_e::gram::argument binary_operator(const wall_e::gram::arg_vector &args, const wall_e::list<wall_e::pair<std::string, std::function<T (T, T)>>> &processors, const environment* env) {
    if(args.size() > 2) {
        const auto val = [](const wall_e::gram::argument &arg, T& value) -> bool {
            if(arg.contains_type<T>()) {
                value = arg.value<T>();
                return true;
            } else if(arg.contains_type<wall_e::lex::token>()) {
                value = arg.value<wall_e::lex::token>().text;
                return true;
            }
            return false;
        };

        T val0, val1;
        if(val(args[0], val0) && args[1].contains_type<wall_e::lex::token>() && val(args[2], val1)) {
            const auto tok = args[1].value<wall_e::lex::token>().name;
            for(const auto processor : processors) {
                if(processor.first == tok) {
                    return processor.second(val0, val1);
                }
            }
        }
    }
    return wall_e::gram::pattern::default_processor(args, index(0, 0, 0, 0), env);
}

template<typename T>
wall_e::gram::argument binary_operator(const wall_e::gram::arg_vector &args, const std::function<T (T, T)> &processor, const wall_e::gram::environment* env) {
    if(args.size() > 2) {
        const auto val = [](const wall_e::gram::argument &arg, T& value) -> bool {
            if(arg.contains_type<T>()) {
                value = arg.value<T>();
                return true;
            } else if(arg.contains_type<wall_e::lex::token>()) {
                value = arg.value<wall_e::lex::token>().text;
                return true;
            }
            return false;
        };

        T val0, val1;
        if(val(args[0], val0) && val(args[2], val1)) {
            return processor(val0, val1);
        }
    }
    return wall_e::gram::pattern::default_processor(args, index(0, 0, 0, 0), env);
}

template<typename T>
wall_e::pair<T, T> binary_operator(const wall_e::gram::arg_vector &args) {
    if(args.size() > 2) {
        const auto val = [](const wall_e::gram::argument &arg, T& value) -> bool {
            if(arg.contains_type<T>()) {
                value = arg.value<T>();
                return true;
            } else if(arg.contains_type<wall_e::lex::token>()) {
                value = arg.value<wall_e::lex::token>().text;
                return true;
            }
            return false;
        };

        T val0, val1;
        if(val(args[0], val0) && val(args[2], val1)) {
            return { val0, val1 };
        }
    }
    return {};
}


namespace literals {

inline auto operator "" _rule(const char* c, size_t s) {
    return rule_from_str(std::string().assign(c, s))
            .get_or();
}

inline auto operator "" _pattern(const char* c, size_t s) {
    return pattern::from_str(std::string().assign(c, s))
            .get_or();
}

inline auto operator "" _patterns(const char* c, size_t s) {
    return wall_e::partition<wall_e::list, error, pattern>(pattern::list_from_str(std::string().assign(c, s)))
            .get_or();
}

} // namespace literals
} // namespace gram
} // namespace wall_e



#endif // GRAM_TOOLS_H
