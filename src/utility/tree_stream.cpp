#include "tree_stream.h"



wall_e::stream_writable_func wall_e::graphviz_tree_writer::node_begin(const context &ctx) const {
    return wall_e::stream_writable_func([ctx](std::ostream &stream) -> std::ostream& {
        return stream << indent << "n_" << ctx.current_index() << " [label=\"";
    });
}

wall_e::stream_writable_func wall_e::graphviz_tree_writer::node_end(const context &) const {
    return wall_e::stream_writable_func([](std::ostream &stream) -> std::ostream& {
        return stream << "\"];" << std::endl;
    });
}

wall_e::stream_writable_func wall_e::graphviz_tree_writer::edge(const context &ctx) const {
    return wall_e::stream_writable_func([ctx](std::ostream &stream) -> std::ostream& {
        if(ctx.parent_index()) {
            if(ctx.relation_label().size() > 0) {
                return stream << indent << "n_" << *ctx.parent_index() << " -> n_" << ctx.current_index() << " [label=\"" << ctx.relation_label() << "\"];" << std::endl;
            } else {
                return stream << indent << "n_" << *ctx.parent_index() << " -> n_" << ctx.current_index() << ";" << std::endl;
            }
        } else {
            return stream;
        }
    });
}
