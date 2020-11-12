#include "tree_view_tools.h"
#include "lex.h"

#include <iostream>

#include <stdio.h>
#include <curses.h>

namespace wall_e {


void kgram_ctoxy(int XPos, int YPos) {
    printf("\033[%d;%dH",YPos + 1,XPos + 1);
}

void kgram_getxy(int* x, int* y) {
   printf("\033[6n");
   scanf("\033[%d;%dR", x, y);
}


void print_tree(const wall_e::variant &input, std::ostream &stream) {
    //system("clear");
    stream << "please install encoding: IBM850\n\nENTRY";
    initscr();
    int x, y;
    kgram_getxy(&x, &y);
    print_branch(input, x, y + 2, stream);
    kgram_ctoxy(0, 20);
}

void print_branch(const wall_e::variant &branch, int x, int y, std::ostream &stream) {
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
            print_branch(vec[i], x + pos, y + 1, stream);
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
        stream << "???";
    }
}

int branch_width(const wall_e::variant &branch) {
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
}
