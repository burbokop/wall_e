#ifndef NODE_H
#define NODE_H

#include <variant>
#include <vector>
#include <map>
#include <ostream>

namespace wall_e {

template<typename type_t, type_t single_value_type, typename value_t>
class node {
    type_t m_type;
    std::variant<value_t, std::vector<node>> m_content;
    bool m_isNull = true;
    static inline std::map<type_t, char> m_symbols;
public:
    node() {}
    node(const char* value) : node(value_t(value)) {}
    node(const value_t& value) {
        m_type = single_value_type;
        m_content = value;
        m_isNull = false;
    }
    node(type_t type, std::vector<node> value) {
        m_type = type;
        m_content = value;
        m_isNull = false;
    }
    template<typename ...Args>
    node(type_t type, Args... args) : node(type, { args... }) {}

    type_t type() const { return m_type; }
    std::variant<value_t, std::vector<node>> content() const { return m_content; }
    value_t value() const { return m_type == single_value_type ? std::get<value_t>(m_content) : value_t(); }
    std::vector<node> children() const { return std::get<std::vector<node>>(m_content); }
    bool isNull() const { return m_isNull; }
    bool isValue() const { return m_type == single_value_type; }
    bool isContainer() const { return m_type != single_value_type; }

    node &operator[](size_t i) { return std::get<std::vector<node>>(m_content)[i]; }
    node operator[](size_t i) const { return std::get<std::vector<node>>(m_content)[i]; }

    static void assignTypeSymbol(type_t type, char symbol) { m_symbols[type] = symbol; }

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

            output << "(";
            for(auto cc : c) {
                output << cc << ((i == c.size() - 1) ? "" : separator);
                ++i;
            }
            output << ")";
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
inline bool operator !=(const node<type_t, single_value_type, value_t> &node1, const node<type_t, single_value_type, value_t> &node2) {
    return !(node1 == node2);
}

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

}


#endif // NODE_H
