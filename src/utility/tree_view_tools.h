#ifndef TREE_VIEW_TOOLS_H
#define TREE_VIEW_TOOLS_H

#include "../models/variant.h"
#include <iostream>

namespace wall_e {

enum tree_print_format {
    Simple,
    Pretty,
    Graphviz
};

void write_tree(const wall_e::variant &input, std::ostream &stream = std::cout, tree_print_format format = Pretty);

}

#endif // TREE_VIEW_TOOLS_H
