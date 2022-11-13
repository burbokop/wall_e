#ifndef TREE_STREAM_H
#define TREE_STREAM_H

#include "collections.h"
namespace wall_e {

// TODO
class tree_stream {
public:
    tree_stream();
};


struct tree_writer {
    class context {
        friend tree_writer;
        std::size_t m_level = 0;
        std::size_t m_current_index;
        wall_e::opt<std::size_t> m_parent_index;
        std::string m_relation_label;
        tree_writer* m_writer;
        context(
                std::size_t level,
                std::size_t current_index,
                const wall_e::opt<std::size_t>& parent_index,
                const std::string& relation_label,
                tree_writer* writer
                )
            : m_level(level),
              m_current_index(current_index),
              m_parent_index(parent_index),
              m_relation_label(relation_label),
              m_writer(writer) {}

    public:
        static context detached() { return context(0, 0, std::nullopt, "", nullptr); }

        inline std::size_t level() const { return m_level; }
        inline wall_e::opt<std::size_t> parent_index() const { return m_parent_index; };
        inline std::size_t current_index() const { return m_current_index; };
        inline const std::string& relation_label() const { return m_relation_label; }

        inline context new_child(const std::string& relation_label) const {
            return context(m_level + 1, m_writer ? m_writer->m_next_index++ : 0, m_current_index, relation_label, m_writer);
        }
        inline stream_writable_func node_begin() const { return m_writer ? m_writer->node_begin(*this) : stream_writable_func::identity(); }
        inline stream_writable_func node_end() const { return m_writer ? m_writer->node_end(*this) : stream_writable_func::identity(); }
        inline stream_writable_func edge() const { return m_writer ? m_writer->edge(*this) : stream_writable_func::identity(); }
    };
private:
    std::size_t m_next_index = 1;
public:
    tree_writer() {}
    context root() { return context(0, 0, std::nullopt, "", this); }

    virtual stream_writable_func node_begin(const context& ctx) const = 0;
    virtual stream_writable_func node_end(const context& ctx) const = 0;
    virtual stream_writable_func edge(const context& ctx) const = 0;

};


class graphviz_tree_writer : public tree_writer {

public:
    static constexpr const char* indent = "   ";
    graphviz_tree_writer() {}

    // tree_writer interface
public:
    virtual stream_writable_func node_begin(const context& ctx) const override;
    virtual stream_writable_func node_end(const context& ctx) const override;
    virtual stream_writable_func edge(const context& ctx) const override;
};

}

#endif // TREE_STREAM_H
