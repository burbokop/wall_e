#include "node.h"

#include <set>

std::ostream &wall_e::write_graph(std::ostream &stream, const wall_e::graph &g, const std::string &hseparator, const std::string &vseparator) {
    for(size_t y = 0; y < g.size(); ++y) {
        const auto sx = g[y].size();
        for(size_t x = 0; x < sx; ++x) {
            stream << g[y][x];
            if(x < sx - 1)
                stream << hseparator;
        }
        if(y < g.size() - 1)
            stream << vseparator;
    }
    stream << std::endl;
    return stream;
}

std::ostream &wall_e::write_relation_list(std::ostream &stream, const wall_e::relation_list &rl, const std::string &hseparator, const std::string &vseparator) {
    for(const auto& e : rl) {
        stream << e.vertex0 << hseparator << e.vertex1 << vseparator;
    }
    return stream;
}

wall_e::graph wall_e::make_graph(size_t n, char c) {
    return graph(n, std::vector<char>(n, c));
}
