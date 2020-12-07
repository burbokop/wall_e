#include "../gram.h"
#include "math_patterns.h"
#include <iostream>

namespace wall_e {

gram::rule math_patterns::add_to(
        std::list<gram::pattern> *patterns, const std::string &preffix) {
    gram::rule term_id = preffix + "_term";
    gram::rule expr_id = preffix + "_expr";
    gram::rule factor_id = preffix + "_factor";
    gram::rule set_id = preffix + "_set";
    gram::rule expr_list_id = preffix + "_expr_list";
    gram::rule value_id = preffix + "_value";
    gram::rule stmt_id = preffix + "_stmt";

    gram::rule plus_munus_id = preffix + "_plus_munus";
    gram::rule mul_div_id = preffix + "_mul_div";
    gram::rule f_id = preffix + "_f";

    patterns->push_back(gram::pattern(expr_id.value())
                        << (plus_munus_id | term_id));

    patterns->push_back(gram::pattern(plus_munus_id.value())
                        << ((term_id & (gram::rule("TOK_PLUS") | "TOK_MINUS") & expr_id))
                        << add_sub_processor);

    patterns->push_back(gram::pattern(term_id.value())
                        << (mul_div_id | factor_id));

    patterns->push_back(gram::pattern(mul_div_id.value())
                        << (factor_id & (gram::rule("TOK_DIV") | "TOK_MUL") & term_id)
                        << mul_div_processor);

    patterns->push_back(gram::pattern(factor_id.value())
                        << ("cmd" | gram::rule("TOK_ID") | "NUMBER"));

    return expr_id;
}

gram::argument math_patterns::add_sub_processor(const gram::arg_vector &args) {
    std::list<std::pair<std::string, std::function<int (int, int)>>> operations;
    operations.push_back({ "TOK_PLUS", [](int a, int b){ return a + b; } });
    operations.push_back({ "TOK_MINUS", [](int a, int b){ return a - b; } });
    return binary_int_operator(args, operations);
}

gram::argument math_patterns::mul_div_processor(const gram::arg_vector &args) {
    std::list<std::pair<std::string, std::function<int (int, int)>>> operations;
    operations.push_back({ "TOK_MUL", [](int a, int b){ return a * b; } });
    operations.push_back({ "TOK_DIV", [](int a, int b){
                               if(b == 0) {
                                   std::cout << "[error] division to null.\n";
                                   return std::numeric_limits<int>::max();
                               }
                               return a / b;
                           }});
    return binary_int_operator(args, operations);
}

gram::argument math_patterns::binary_int_operator(const gram::arg_vector &args, std::list<std::pair<std::string, std::function<int (int, int)> > > l) {
    if(args.size() > 2) {
        //std::cout << args << "\n";
        int num0, num1;
        if(extract_number(args[0], &num0)
                && args[1].contains_type<wall_e::lex::token>()
                && extract_number(args[2], &num1)
                ) {
            for(auto el : l) {
                if(el.first == args[1].value<wall_e::lex::token>().name) {
                    auto r = el.second(num0, num1);
                    //std::cout << num0 << " " << el.first << " " << num1 << " = " << r << "\n";
                    return r;
                }
            }
            std::cout << "[error] undefined operator" << args[1].value<wall_e::lex::token>() << "\n";
            return gram::argument();
        } else if(false && extract_number(args[0], &num0)
                  && args[1].contains_type<wall_e::lex::token>()
                  && args[2].contains_type<gram::arg_vector>()) {
            const auto vec = args[2].value<gram::arg_vector>();
            if(vec.size() > 2) {
                bool firstIsNumber = extract_number(vec[0], nullptr);
                if(vec[1].contains_type<wall_e::lex::token>() && (firstIsNumber || extract_number(vec[2], nullptr))) {
                    if(vec[1].value<wall_e::lex::token>().name == args[1].value<wall_e::lex::token>().name) {
                        if(firstIsNumber) {
                            //std::cout << "refactored: " << kgram_arg_vector_t { vec[2], args[1], kgram_arg_vector_t { args[0], vec[1], vec[0] } };
                            return binary_int_operator(gram::arg_vector { vec[2], args[1], gram::arg_vector { args[0], vec[1], vec[0] } }, l);
                        } else {
                            //std::cout << "refactored: " << kgram_arg_vector_t { vec[0], args[1], kgram_arg_vector_t { args[0], vec[1], vec[2] } };
                            return binary_int_operator(gram::arg_vector { vec[0], args[1], gram::arg_vector { args[0], vec[1], vec[2] } }, l);
                        }
                    }
                }
            }
        }
        return gram::pattern::__default_processor(args);
    }
    return gram::argument();
}

bool math_patterns::extract_number_str(const std::string &arg, int *number) {
    try {
        if(number)
            *number = std::stoi(arg);
        return true;
    } catch (std::invalid_argument) {
        return false;
    }
}

bool math_patterns::extract_number(const gram::argument &arg, int *number) {
    if(arg.contains_type<int>()) {
        if(number)
            *number = arg.value<int>();
        return true;
    } else if(arg.contains_type<std::string>()) {
        return extract_number_str(arg.value<std::string>(), number);
    } else if(arg.contains_type<wall_e::lex::token>()) {
        return extract_number_str(arg.value<wall_e::lex::token>().text, number);
    }
    return false;
}
}
