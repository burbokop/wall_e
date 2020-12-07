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

#include "../lex.h"
#include "../variant.h"

namespace wall_e {
namespace gram {

typedef wall_e::variant argument;
typedef std::vector<argument> arg_vector;


template<typename type_t, type_t single_value_type, typename value_t>
class node {
    type_t m_type;
    std::variant<value_t, std::vector<node>> m_content;
    bool m_isNull = true;
    static inline std::map<type_t, char> m_symbols;
public:
    type_t type() const { return m_type; }
    std::variant<value_t, std::vector<node>> content() const { return m_content; }
    value_t value() const { return m_type == single_value_type ? std::get<value_t>(m_content) : value_t(); }
    std::vector<node> children() const { return std::get<std::vector<node>>(m_content); }

    node() {}

    node(const char* str) : node(value_t(str)) {}

    node(value_t value) {
        m_type = single_value_type;
        m_content = value;
        m_isNull = false;
    }

    node(type_t type, std::vector<node> value) {
        m_type = type;
        m_content = value;
        m_isNull = false;
    }

    static void assignTypeSymbol(type_t type, char symbol) { m_symbols[type] = symbol; }

    template<typename ...Args>
    node(type_t type, Args... args) : node(type, { args... }) {}


    bool isNull() const { return m_isNull; }

    friend std::ostream &operator<<(std::ostream &output, const node &node) {
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

    friend std::ostream &operator<<(std::ostream &output, const std::vector<node> &vector) {
        int i = 0;
        output << "{ ";
        for(auto v : vector) {
            output << v << (i == vector.size() - 1 ? "" : ", ");
            ++i;
        }
        output << " }";
        return output;
    }
};


template<typename type_t, type_t single_value_type, typename value_t>
bool operator !=(const node<type_t, single_value_type, value_t> &node1, const node<type_t, single_value_type, value_t> &node2);

template<typename type_t, type_t single_value_type, typename value_t>
bool operator ==(const node<type_t, single_value_type, value_t> &node1, const node<type_t, single_value_type, value_t> &node2) {
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
bool operator !=(const node<type_t, single_value_type, value_t> &node1, const node<type_t, single_value_type, value_t> &node2) {
    return !(node1 == node2);
}

template<typename type_t, type_t single_value_type, typename value_t, typename Func>
void __print_node_internal(node<type_t, single_value_type, value_t> folder, Func function) {
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
            __print_node_internal(f, function);
            if(i < folders.size() - 1) {
                function(", ");
            }
            i++;
        }

        function(")");
    }
}

template<typename type_t, type_t single_value_type, typename value_t, typename Func>
void print_node(node<type_t, single_value_type, value_t> folder, Func function, bool endl = true) {
    __print_node_internal<type_t, single_value_type, value_t, Func>(folder, function);
    if(endl) {
        function("\n");
    }
}

//node end

template<typename T>
struct common_call_result {
    common_call_result() { }
    common_call_result(const T &arg, bool confirmed) { this->arg = arg; this->confirmed = confirmed; }
    T arg;
    bool confirmed = false;
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

std::string rule_to_string(const rule &r);

rule operator &(
        const rule &rule1,
        const rule &rule2);

rule operator |(
        const rule &rule1,
        const rule &rule2);



void print_rule(const rule &r);

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

class kgram_pattern_t {
    std::string m_name;
    rule m_gram_rule;
public:
    const static inline std::function<argument(const arg_vector&)> __default_processor = [](const arg_vector &args) -> argument {
        if(args.size() > 0) {
            if(args.size() > 1) {
                return args;
            }
            return args[0];
        }
        return argument();
    };
private:

    std::function<argument(const arg_vector&)> m_callback = __default_processor;

    bool m_isValid = false;
public:
    kgram_pattern_t() {};
    kgram_pattern_t(const char* name) { m_name = name; m_isValid = true; };
    kgram_pattern_t(std::string name) { m_name = name; m_isValid = true; };
    std::string name() const { return m_name; };

    friend kgram_pattern_t &operator<< (kgram_pattern_t &pattern, const rule &rule);
    friend kgram_pattern_t &operator<< (kgram_pattern_t &pattern, std::function<argument(arg_vector)> callback);

    friend kgram_pattern_t operator<< (kgram_pattern_t pattern, const rule &rule);
    friend kgram_pattern_t operator<< (kgram_pattern_t pattern, std::function<argument(arg_vector)> callback);
    gram::rule gram_rule() const;
    std::function<argument (arg_vector)> callback(bool __default = false) const;

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
//wall_e::lex::token kgram_find_token(const T &tokens, const std::string name) {
//    for(auto t : tokens) {
//        if(t.name == name)
//            return t;
//    }
//    wall_e::lex::token result;
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
    std::variant<wall_e::lex::token, kgram_pattern_t> m_data;
public:
    kgram_item_t() { }
    kgram_item_t(const wall_e::lex::token &token) { m_data = token; m_type = Token; }
    kgram_item_t(const kgram_pattern_t &pattern) { m_data = pattern; m_type = Pattern; }

    wall_e::lex::token token() const {
        if(m_type == Token)
            return std::get<wall_e::lex::token>(m_data);
        return wall_e::lex::token();
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

kgram_pattern_t &operator<< (kgram_pattern_t &pattern, std::function<argument(arg_vector)> callback);
kgram_pattern_t operator<< (kgram_pattern_t pattern, std::function<argument(arg_vector)> callback);

class kgram_token_iterator {
    std::vector<wall_e::lex::token>::const_iterator it;
    std::vector<wall_e::lex::token>::const_iterator begin;
    std::vector<wall_e::lex::token>::const_iterator end;
public:
    kgram_token_iterator(const std::vector<wall_e::lex::token> &container) {
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

    wall_e::lex::token data() const {
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


}
}

#endif // KGRAM_TOOLS_H
