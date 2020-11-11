#ifndef KGRAM_TOOLS_H
#define KGRAM_TOOLS_H

#include <string>
#include <variant>
#include <vector>
#include <functional>
#include <list>
#include <ostream>
#include <cxxabi.h>
#include <cassert>

#include "klex.h"
#include "kgramvariant.h"

typedef kgram_variant_t kgram_argument_t;
typedef std::vector<kgram_argument_t> kgram_arg_vector_t;


template<typename type_t, type_t single_value_type, typename value_t>
class kgram_node_t {
    type_t m_type;
    std::variant<value_t, std::vector<kgram_node_t>> m_content;
    bool m_isNull = true;
    static inline std::map<type_t, char> m_symbols;
public:
    type_t type() const { return m_type; }
    std::variant<value_t, std::vector<kgram_node_t>> content() const { return m_content; }
    value_t value() const { return m_type == single_value_type ? std::get<value_t>(m_content) : value_t(); }
    std::vector<kgram_node_t> children() const { return std::get<std::vector<kgram_node_t>>(m_content); }

    kgram_node_t() {}

    kgram_node_t(const char* str) : kgram_node_t(value_t(str)) {}

    kgram_node_t(value_t value) {
        m_type = single_value_type;
        m_content = value;
        m_isNull = false;
    }

    kgram_node_t(type_t type, std::vector<kgram_node_t> value) {
        m_type = type;
        m_content = value;
        m_isNull = false;
    }

    static void assignTypeSymbol(type_t type, char symbol) { m_symbols[type] = symbol; }

    template<typename ...Args>
    kgram_node_t(type_t type, Args... args) : kgram_node_t(type, { args... }) {}


    bool isNull() const { return m_isNull; }

    friend std::ostream &operator<<(std::ostream &output, const kgram_node_t &node) {
        if(node.isNull()) {
            output << "null";
        } else if(node.type() == single_value_type) {
            output << node.value();
        } else {
            auto c = node.children();
            int i = 0;

            std::string separator = " ";
            auto it = m_symbols.find(node.type());
            if(it != m_symbols.end()) {
                separator = it->second;
                separator = " " + separator + " ";
            }

            output << "( ";
            for(auto cc : c) {
                output << cc << ((i == c.size() - 1) ? "" : separator);
                ++i;
            }
            output << " )";
        }

        return output;
    }    

    friend std::ostream &operator<<(std::ostream &output, const std::vector<kgram_node_t> &vector) {
        int i = 0;
        output << "{ ";
        for(auto v : vector) {
            output << v << (i == vector.size() - 1 ? "" : ", ");
            ++i;
        }
        output << " }";
    }
};


template<typename type_t, type_t single_value_type, typename value_t>
bool operator !=(const kgram_node_t<type_t, single_value_type, value_t> &node1, const kgram_node_t<type_t, single_value_type, value_t> &node2);

template<typename type_t, type_t single_value_type, typename value_t>
bool operator ==(const kgram_node_t<type_t, single_value_type, value_t> &node1, const kgram_node_t<type_t, single_value_type, value_t> &node2) {
    if(node1.isNull() && node2.isNull())
        return true;

    if(node1.type() == node2.type()) {
        if(node1.type() == single_value_type && node1.value() == node2.value()) {
            return true;
        } else {
            auto c1 = node1.children();
            auto c2 = node2.children();

            if(c1.size() == c2.size()) {
                for(size_t i = 0; i < c1.size(); ++i) {
                    if(c1[i] != c2[i])
                        return false;
                }
                return true;
            }
        }
    }

    return false;
}

template<typename type_t, type_t single_value_type, typename value_t>
bool operator !=(const kgram_node_t<type_t, single_value_type, value_t> &node1, const kgram_node_t<type_t, single_value_type, value_t> &node2) {
    return !(node1 == node2);
}

template<typename type_t, type_t single_value_type, typename value_t, typename Func>
void kgram_print_node_internal(kgram_node_t<type_t, single_value_type, value_t> folder, Func function) {
    if(folder.isNull()) {
        function("null");
    } else if(folder.type() == single_value_type) {
        function(folder.value());
    } else {
        auto folders = folder.children();
        function("ctype: "); function(folder.type()); function(" -");
        function("(");
        size_t i = 0;
        for(auto f : folders) {
            kgram_print_node_internal(f, function);
            if(i < folders.size() - 1) {
                function(", ");
            }
            i++;
        }

        function(")");
    }
}

template<typename type_t, type_t single_value_type, typename value_t, typename Func>
void kgram_print_node(kgram_node_t<type_t, single_value_type, value_t> folder, Func function, bool endl = true) {
    kgram_print_node_internal<type_t, single_value_type, value_t, Func>(folder, function);
    if(endl) {
        function("\n");
    }
}

template<typename T>
struct kgram_common_call_result {
    kgram_common_call_result() { }
    kgram_common_call_result(const T &arg, bool confirmed) { this->arg = arg; this->confirmed = confirmed; }
    T arg;
    bool confirmed = false;
};

typedef kgram_common_call_result<kgram_argument_t> kgram_call_mono_result;
typedef kgram_common_call_result<kgram_arg_vector_t> kgram_call_result;

kgram_call_result kgram_call_result_cast(const kgram_call_mono_result &value);
kgram_call_mono_result kgram_call_mono_result_cast(const kgram_call_result &value);

struct kgram_rule_type_t {
    enum enum_t {
        Text = 0,
        Conjunction = 1,
        Disjunction = 2,
        Undefined = 3
    };
};

typedef kgram_node_t<kgram_rule_type_t::enum_t, kgram_rule_type_t::Text, std::string> kgram_rule_t;

std::string kgram_rule_to_string(const kgram_rule_t &rule);

kgram_rule_t operator &(
        const kgram_rule_t &rule1,
        const kgram_rule_t &rule2);

kgram_rule_t operator |(
        const kgram_rule_t &rule1,
        const kgram_rule_t &rule2);



void kgram_print_rule(const kgram_rule_t &rule);

struct kgram_rule_transition_t {
    enum type_t {
        ConjunctionDisjunction = 0,
        DoubleConjunction,
        DoubleDisjunction,
        Auto,
        Undefined
    };
    kgram_rule_t rule;
    type_t type = Undefined;
};

kgram_rule_t kgram_simplify_rule(const kgram_rule_t &rule, kgram_rule_transition_t::type_t method = kgram_rule_transition_t::Auto);


class kgram_pattern_t {
    std::string m_name;
    kgram_rule_t m_rule;
public:
    const static inline std::function<kgram_argument_t(const kgram_arg_vector_t&)> __default_processor = [](const kgram_arg_vector_t &args) -> kgram_argument_t {
        if(args.size() > 0) {
            if(args.size() > 1) {
                return args;
            }
            return args[0];
        }
        return kgram_argument_t();
    };
private:

    std::function<kgram_argument_t(const kgram_arg_vector_t&)> m_callback = __default_processor;

    bool m_isValid = false;
public:
    kgram_pattern_t() {};
    kgram_pattern_t(const char* name) { m_name = name; m_isValid = true; };
    kgram_pattern_t(std::string name) { m_name = name; m_isValid = true; };
    std::string name() const { return m_name; };

    friend kgram_pattern_t &operator<< (kgram_pattern_t &pattern, const kgram_rule_t &rule);
    friend kgram_pattern_t &operator<< (kgram_pattern_t &pattern, std::function<kgram_argument_t(kgram_arg_vector_t)> callback);

    friend kgram_pattern_t operator<< (kgram_pattern_t pattern, const kgram_rule_t &rule);
    friend kgram_pattern_t operator<< (kgram_pattern_t pattern, std::function<kgram_argument_t(kgram_arg_vector_t)> callback);
    kgram_rule_t rule() const;
    std::function<kgram_argument_t (kgram_arg_vector_t)> callback(bool __default = false) const;

    friend void kgram_print_pattern(const kgram_pattern_t &pattern);

    bool isValid() const;

    static std::string to_string(const std::list<kgram_pattern_t> &list);
};

template<typename T>
kgram_pattern_t kgram_find_pattern(const T &pattens, const std::string name) {
    for(auto p : pattens) {
        if(p.name() == name)
            return p;
    }
    return kgram_pattern_t();
}

//template<typename T>
//klex_token_t kgram_find_token(const T &tokens, const std::string name) {
//    for(auto t : tokens) {
//        if(t.name == name)
//            return t;
//    }
//    klex_token_t result;
//    result.meta = 1;
//    return result;
//}

class kgram_item_t {
public:
    enum type_t {
        Token,
        Pattern,
        Undefined,
    };

private:
    type_t m_type = Undefined;
    std::variant<klex_token_t, kgram_pattern_t> m_data;
public:
    kgram_item_t() { }
    kgram_item_t(const klex_token_t &token) { m_data = token; m_type = Token; }
    kgram_item_t(const kgram_pattern_t &pattern) { m_data = pattern; m_type = Pattern; }

    klex_token_t token() const {
        if(m_type == Token)
            return std::get<klex_token_t>(m_data);
        return klex_token_t();
    }
    kgram_pattern_t pattern() const {
        if(m_type == Pattern)
            return std::get<kgram_pattern_t>(m_data);
        return kgram_pattern_t();
    }
    type_t type() const { return m_type; };
};

//template<typename tocken_container_T, typename pattern_container_T>
//kgram_item_t kgram_find_item(const tocken_container_T &tokens, const pattern_container_T &pattens, const std::string name) {
//    auto t = kgram_find_token(tokens, name);
//    if(t.meta == 1) {
//        auto p = kgram_find_pattern(pattens, name);
//        if(p.isValid()) {
//            return p;
//        } else {
//            return kgram_item_t();
//        }
//    } else {
//        return t;
//    }
//}

kgram_pattern_t &operator<< (kgram_pattern_t &pattern, std::function<kgram_argument_t(kgram_arg_vector_t)> callback);
kgram_pattern_t operator<< (kgram_pattern_t pattern, std::function<kgram_argument_t(kgram_arg_vector_t)> callback);

class kgram_token_iterator {
    std::vector<klex_token_t>::const_iterator it;
    std::vector<klex_token_t>::const_iterator begin;
    std::vector<klex_token_t>::const_iterator end;
public:
    kgram_token_iterator(const std::vector<klex_token_t> &container) {
        it = container.begin();
        begin = container.begin();
        end = container.end();
    }

    bool isValid() const { return it >= begin && it < end; }

    bool next() {
        if(isValid()) {
            ++it;
            return true;
        }
        return false;
    }

    bool goBack() {
        if(isValid()) {
            --it;
            return true;
        }
        return false;
    }

    klex_token_t data() const {
        return (*it);
    }

    int offset() const {
        return it - begin;
    }
};

std::ostream &operator<< (std::ostream &stream, const kgram_token_iterator &it);

template<typename pattern_container_T>
kgram_item_t kgram_determine_item(const kgram_token_iterator *it, const pattern_container_T &pattens, const std::string text) {
    if(it->isValid()) {
        if(it->data().name == text) {
            return it->data();
        }
    }
    auto p = kgram_find_pattern(pattens, text);
    if(p.isValid()) {
        return p;
    }
    return kgram_item_t();
}

std::string kgram_to_lowercase(std::string str);





#endif // KGRAM_TOOLS_H
