#ifndef TREE_VIEW_TOOLS_H
#define TREE_VIEW_TOOLS_H

#include "../variant.h"
#include <iostream>

namespace wall_e {

int branch_width(const wall_e::variant &branch);
void print_tree(const wall_e::variant &input, std::ostream &stream = std::cout);
void print_branch(const wall_e::variant &branch, int x, int y, std::ostream &stream);

}

#endif // TREE_VIEW_TOOLS_H
