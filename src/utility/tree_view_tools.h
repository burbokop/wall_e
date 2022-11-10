#ifndef TREE_VIEW_TOOLS_H
#define TREE_VIEW_TOOLS_H

#include "../models/variant.h"
#include <iostream>

namespace wall_e {

int branch_width(const wall_e::variant &branch);
void print_branch(const wall_e::variant &branch, int x, int y, std::ostream &stream);

void print_branch_simple(const wall_e::variant &branch, std::size_t level, std::ostream &stream);

enum tree_print_format {
    Simple,
    Pretty
};

void print_tree(const wall_e::variant &input, std::ostream &stream = std::cout, tree_print_format format = Pretty);

}

#endif // TREE_VIEW_TOOLS_H
