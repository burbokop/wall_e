#include "tree_view_tools.h"
#include "../lex.h"

#include <iostream>
#include <stdio.h>

static void kgram_ctoxy(int XPos, int YPos) {
    printf("\033[%d;%dH",YPos + 1,XPos + 1);
}

static void kgram_getxy(int* x, int* y) {
   printf("\033[6n");
   scanf_s("\033[%d;%dR", x, y);
}

static int branch_width(const wall_e::variant &branch) {
    if(branch.contains_type<wall_e::variant_vector>()) {
        auto vec = branch.value<wall_e::variant_vector>();
        int count = 0;
        for(size_t i = 0, cnt = vec.size(); i < cnt; ++i) {
            count += branch_width(vec[i]);
        }
        return count;
    } else if(branch.contains_type<wall_e::lex::token>()) {
        return branch.value<wall_e::lex::token>().text.size();
    } else if(branch.contains_type<std::string>()) {
        return branch.value<std::string>().size();
    } else {
        return 3;
    }
}

static void write_branch_simple(const wall_e::variant &branch, size_t level, std::ostream &stream) {
    if(branch.contains_type<wall_e::variant_vector>()) {
        auto vec = branch.value<wall_e::variant_vector>();
        int pos = 0;
        for(size_t i = 0, cnt = vec.size(); i < cnt; ++i) {
            write_branch_simple(vec[i], level + 1, stream);
        }
    } else if(branch.contains_type<wall_e::lex::token>()) {
        auto token = branch.value<wall_e::lex::token>();
        stream << std::string(level, '.') << token.text << std::endl;
    } else if(branch.contains_type<std::string>()) {
        auto string = branch.value<std::string>();
        stream << std::string(level, '.') << string << std::endl;
    } else {
        stream << std::string(level, '.') << "?" << branch.type() << "?" << std::endl;
    }
}

static void write_branch_pretty(const wall_e::variant &branch, int x, int y, std::ostream &stream) {
    kgram_ctoxy(x, y);
    if(branch.contains_type<wall_e::variant_vector>()) {
        auto vec = branch.value<wall_e::variant_vector>();

        int pos = 0;
        for(size_t i = 0, cnt = vec.size(); i < cnt; ++i) {
            kgram_ctoxy(x + pos, y);
            if(i == 0) {
                if(vec.size() > 1) {
                    stream << char(195);
                } else {
                    stream << char(179);
                }
            } else if(i == vec.size() - 1) {
                stream << char(191);
            } else {
                stream << char(194);
            }

            const int w = branch_width(vec[i]) + 1;
            write_branch_pretty(vec[i], x + pos, y + 1, stream);
            if(i != vec.size() - 1) {
                for(int j = 1; j < w; ++j) {
                    kgram_ctoxy(x + pos + j, y);
                    stream << char(196);
                }
            }
            pos += w;
        }
    } else if(branch.contains_type<wall_e::lex::token>()) {
        stream << char(179);
        kgram_ctoxy(x, y + 1);
        auto token = branch.value<wall_e::lex::token>();
        stream << token.text;
    } else if(branch.contains_type<std::string>()) {
        stream << char(179);
        kgram_ctoxy(x, y + 1);
        auto string = branch.value<std::string>();
        stream << string;
    } else {
        stream << "?" << branch.type() << "?";
    }
}

static void write_branch_graphviz(const wall_e::variant &branch, std::string parent, std::size_t &next_point_node, std::ostream &stream) {
    const std::string indent = "   ";
    const auto node_name = "n_" + std::to_string(next_point_node++);
    stream << indent << parent << " -> " << node_name << ";" << std::endl;


    if(branch.contains_type<wall_e::variant_vector>()) {
        auto vec = branch.value<wall_e::variant_vector>();
        int pos = 0;

        stream << indent << node_name << " [shape=point];" << std::endl;

        for(size_t i = 0, cnt = vec.size(); i < cnt; ++i) {
            write_branch_graphviz(vec[i], node_name, next_point_node, stream);
        }
    } else if(branch.contains_type<wall_e::lex::token>()) {
        auto token = branch.value<wall_e::lex::token>();
        stream << indent << node_name << " [label=\"" << token.text << "\"];" << std::endl;
    } else if(branch.contains_type<std::string>()) {
        auto string = branch.value<std::string>();
        stream << indent << node_name << " [label=\"" << string << "\"];" << std::endl;
    } else {
        stream << indent << node_name << " [label=\"?" << branch.type() << "?\"];" << std::endl;
    }

    //[label="A"]
    /*digraph G {

      root -> p1;

      p1 ->

      b2 -> a3;
      a3 -> a0;
      a3 -> end;
      b3 -> end;

      p1 [shape=point];
    }*/

}

void wall_e::write_tree(const wall_e::variant &input, std::ostream &stream, tree_print_format format) {
    if(format == Graphviz) {
        stream << "digraph G { " << std::endl;
        std::size_t n = 0;
        write_branch_graphviz(input, "root", n, stream);
        stream << "}" << std::endl;
    } else if(format == Pretty) {
        //system("clear");
        stream << "wall_e::print_tree: Pretty format need IBM850 encoding\n\nENTRY";
        //initscr();
        int x, y;
        kgram_getxy(&x, &y);
        write_branch_pretty(input, x, y + 2, stream);
        kgram_ctoxy(0, 20);
    } else {
        write_branch_simple(input, 0, stream);
    }
}
