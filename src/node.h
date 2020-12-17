#ifndef NODE_H
#define NODE_H

#include <variant>
#include <vector>
#include <map>
#include <ostream>
#include <list>


namespace wall_e {

struct relation {
    uint64_t vertex0;
    uint64_t vertex1;
    char symbol;
};

typedef std::vector<std::vector<uint64_t>> graph;
typedef std::list<relation> relation_list;
std::ostream& write_graph(std::ostream& stream, const graph& g, const std::string &hseparator = " ", const std::string &vseparator = "\n");
std::ostream& write_relation_list(std::ostream& stream, const relation_list& rl, const std::string &hseparator = " ", const std::string &vseparator = "\n");

template<typename type_type, type_type single_value_type, typename value_type>
class node {
    type_type m_type;
    std::variant<value_type, std::vector<node>> m_content;
    bool m_isNull = true;
    static inline std::map<type_type, char> m_symbols;
public:
    node() {}
    node(const char* value) : node(value_type(value)) {}
    node(const value_type& value) {
        m_type = single_value_type;
        m_content = value;
        m_isNull = false;
    }
    node(type_type type, std::vector<node> value) {
        m_type = type;
        m_content = value;
        m_isNull = false;
    }
    template<typename ...Args>
    node(type_type type, Args... args) : node(type, { args... }) {}

    type_type type() const { return m_type; }
    std::variant<value_type, std::vector<node>> content() const { return m_content; }
    value_type value() const { return m_type == single_value_type ? std::get<value_type>(m_content) : value_type(); }
    std::vector<node> children() const { return std::get<std::vector<node>>(m_content); }
    bool isNull() const { return m_isNull; }
    bool isValue() const { return m_type == single_value_type; }
    bool isContainer() const { return !m_isNull && m_type != single_value_type; }

    inline node &operator[](size_t i) { return std::get<std::vector<node>>(m_content)[i]; }
    inline node operator[](size_t i) const { return std::get<std::vector<node>>(m_content)[i]; }
    inline size_t size() const { return std::get<std::vector<node>>(m_content).size(); }

    static void assignTypeSymbol(type_type type, char symbol) { m_symbols[type] = symbol; }

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

    static void make_relation_list(const node<type_type, single_value_type, value_type>& n, relation_list& rl, uint64_t &next, uint64_t last, char last_char = 0) {
        const auto current = next++;
        rl.push_back({ current, last, last_char });
        if(n.isContainer()) {
            const auto it = m_symbols.find(n.type());
            for(size_t i = 0; i < n.size(); ++i) {
                make_relation_list(n[i], rl, next, current, (it != m_symbols.end()) ? it->second : ' ');
            }
        }
    }

    inline relation_list to_relation_list(uint64_t *count = nullptr) const {
        uint64_t next = 0;
        wall_e::relation_list rl;
        make_relation_list(*this, rl, next, 0);
        if(count)
            *count = next;
        return rl;
    }

    graph to_graph() const {
        uint64_t count;
        const relation_list rl = to_relation_list(&count);
        graph result;
        result.resize(count);
        for(auto &r : result)
            r.resize(count);

        for(const auto &relation : rl) {
            result[relation.vertex0][relation.vertex1] = relation.symbol;
        }
        return result;
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
