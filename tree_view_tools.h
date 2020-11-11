#ifndef TREE_VIEW_TOOLS_H
#define TREE_VIEW_TOOLS_H

#include "kgramvariant.h"
#include <iostream>

int branch_width(const kgram_variant_t &branch);


void print_tree(const kgram_variant_t &input, std::ostream &stream = std::cout);

void print_branch(const kgram_variant_t &branch, int x, int y, std::ostream &stream);

#endif // TREE_VIEW_TOOLS_H
