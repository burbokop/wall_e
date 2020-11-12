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

#include "lex.h"
#include "variant.h"


namespace wall_e {
namespace gram {


typedef wall_e::variant argument;
typedef std::vector<argument> arg_vector;


template<typename type_t, type_t single_value_type, typename value_t>
class Node {
    type_t m_type;
    std::variant<value_t, std::vector<Node>> m_content;
    bool m_isNull = true;
    static inline std::map<type_t, char> m_symbols;
public:
    type_t type() const { return m_type; }
    std::variant<value_t, std::vector<Node>> content() const { return m_content; }
    value_t value() const { return m_type == single_value_type ? std::get<value_t>(m_content) : value_t(); }
    std::vector<Node> children() const { return std::get<std::vector<Node>>(m_content); }

    Node() {}

    Node(const char* str) : Node(value_t(str)) {}

    Node(value_t value) {
        m_type = single_value_type;
        m_content = value;
        m_isNull = false;
    }

    Node(type_t type, std::vector<Node> value) {
        m_type = type;
        m_content = value;
        m_isNull = false;
    }

    static void assignTypeSymbol(type_t type, char symbol) { m_symbols[type] = symbol; }

    template<typename ...Args>
    Node(type_t type, Args... args) : Node(type, { args... }) {}


    bool isNull() const { return m_isNull; }

    friend std::ostream &operator<<(std::ostream &output, const Node &node) {
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

    friend std::ostream &operator<<(std::ostream &output, const std::vector<Node> &vector) {
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
bool operator !=(const Node<type_t, single_value_type, value_t> &node1, const Node<type_t, single_value_type, value_t> &node2);

template<typename type_t, type_t single_value_type, typename value_t>
bool operator ==(const Node<type_t, single_value_type, value_t> &node1, const Node<type_t, single_value_type, value_t> &node2) {
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
bool operator !=(const Node<type_t, single_value_type, value_t> &node1, const Node<type_t, single_value_type, value_t> &node2) {
    return !(node1 == node2);
}

template<typename type_t, type_t single_value_type, typename value_t, typename Func>
void print_node_internal(Node<type_t, single_value_type, value_t> folder, Func function) {
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
            print_node_internal(f, function);
            if(i < folders.size() - 1) {
                function(", ");
            }
            i++;
        }

        function(")");
    }
}

template<typename type_t, type_t single_value_type, typename value_t, typename Func>
void print_node(Node<type_t, single_value_type, value_t> folder, Func function, bool endl = true) {
    print_node_internal<type_t, single_value_type, value_t, Func>(folder, function);
    if(endl) {
        function("\n");
    }
}

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

struct rule_type_t {
    enum enum_t {
        Text = 0,
        Conjunction = 1,
        Disjunction = 2,
        Undefined = 3
    };
};

typedef Node<rule_type_t::enum_t, rule_type_t::Text, std::string> Rule;

std::string rule_to_string(const Rule &rule);

Rule operator &(
        const Rule &rule1,
        const Rule &rule2);

Rule operator |(
        const Rule &rule1,
        const Rule &rule2);



void print_rule(const Rule &rule);

struct rule_transition {
    enum enum_type {
        ConjunctionDisjunction = 0,
        DoubleConjunction,
        DoubleDisjunction,
        Auto,
        Undefined
    };
    Rule rule;
    enum_type type = Undefined;
};

Rule simplify_rule(const Rule &rule, rule_transition::enum_type method = rule_transition::Auto);


class Pattern {
    std::string m_name;
    Rule m_rule;
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
    Pattern() {};
    Pattern(const char* name) { m_name = name; m_isValid = true; };
    Pattern(std::string name) { m_name = name; m_isValid = true; };
    std::string name() const { return m_name; };

    friend Pattern &operator<< (Pattern &pattern, const Rule &rule);
    friend Pattern &operator<< (Pattern &pattern, std::function<argument(arg_vector)> callback);

    friend Pattern operator<< (Pattern pattern, const Rule &rule);
    friend Pattern operator<< (Pattern pattern, std::function<argument(arg_vector)> callback);
    Rule rule() const;
    std::function<argument (arg_vector)> callback(bool __default = false) const;

    friend void print_pattern(const Pattern &pattern);

    bool isValid() const;

    static std::string to_string(const std::list<Pattern> &list);
};

template<typename T>
Pattern find_pattern(const T &pattens, const std::string name) {
    for(auto p : pattens) {
        if(p.name() == name)
            return p;
    }
    return Pattern();
}

//template<typename T>
//wall_e::lex::token find_token(const T &tokens, const std::string name) {
//    for(auto t : tokens) {
//        if(t.name == name)
//            return t;
//    }
//    wall_e::lex::token result;
//    result.meta = 1;
//    return result;
//}

class Item {
public:
    enum Type {
        Token,
        Pattern,
        Undefined,
    };

private:
    Type m_type = Undefined;
    std::variant<wall_e::lex::Token, gram::Pattern> m_data;
public:
    Item() { }
    Item(const wall_e::lex::Token &token) { m_data = token; m_type = Token; }
    Item(const gram::Pattern &pattern) { m_data = pattern; m_type = Type::Pattern; }

    wall_e::lex::Token token() const {
        if(m_type == Token)
            return std::get<wall_e::lex::Token>(m_data);
        return wall_e::lex::Token();
    }
    gram::Pattern pattern() const {
        if(m_type == Type::Pattern)
            return std::get<gram::Pattern>(m_data);
        return gram::Pattern();
    }
    Type type() const { return m_type; };
};

//template<typename tocken_container_T, typename pattern_container_T>
//item find_item(const tocken_container_T &tokens, const pattern_container_T &pattens, const std::string name) {
//    auto t = find_token(tokens, name);
//    if(t.meta == 1) {
//        auto p = find_pattern(pattens, name);
//        if(p.isValid()) {
//            return p;
//        } else {
//            return item();
//        }
//    } else {
//        return t;
//    }
//}

Pattern &operator<< (Pattern &pattern, std::function<argument(arg_vector)> callback);
Pattern operator<< (Pattern pattern, std::function<argument(arg_vector)> callback);

class token_iterator {
    std::vector<wall_e::lex::Token>::const_iterator it;
    std::vector<wall_e::lex::Token>::const_iterator begin;
    std::vector<wall_e::lex::Token>::const_iterator end;
public:
    token_iterator(const std::vector<wall_e::lex::Token> &container) {
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

    wall_e::lex::Token data() const {
        return (*it);
    }

    int offset() const {
        return it - begin;
    }
};

std::ostream &operator<< (std::ostream &stream, const token_iterator &it);

template<typename pattern_container_T>
Item determine_item(const token_iterator *it, const pattern_container_T &pattens, const std::string text) {
    if(it->isValid()) {
        if(it->data().name == text) {
            return it->data();
        }
    }
    auto p = find_pattern(pattens, text);
    if(p.isValid()) {
        return p;
    }
    return Item();
}

std::string to_lowercase(std::string str);

}
}



#endif // KGRAM_TOOLS_H
