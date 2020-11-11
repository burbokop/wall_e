#include "kgram_smp.h"

#include <iostream>

kgram_rule_t kgram_smp(const kgram_rule_t &rule) {
    auto c = rule;
    while (true) {
        auto nc = __kgram_smp(c, kgram_rule_type_t::Undefined);
        if(c == nc.first)
            break;
        c = nc.first;
    }
    return c;
}


__kgram_smp_arg_t __kgram_smp(const kgram_rule_t &arg, kgram_rule_type_t::enum_t last_type) {
    kgram_rule_t result;

    if(arg.type() == kgram_rule_type_t::Text) {
        return __kgram_smp_arg_t(arg, arg.type());
    } else if(arg.type() == kgram_rule_type_t::Conjunction) {
        auto c = arg.children();
        for(auto cc : c) {
            if(cc.type() == kgram_rule_type_t::Conjunction) {

            }
        }

    } else if(arg.type() == kgram_rule_type_t::Disjunction) {
        if(last_type == kgram_rule_type_t::Conjunction) {
            return __kgram_smp_arg_t(arg, arg.type());
        } else {

        }
    }
    return __kgram_smp_arg_t();
}


void kgram_smp_test(kgram_rule_t (*f)(const kgram_rule_t &)) {
    auto test = [&f](const kgram_rule_t &r){
        std::cout << "kgram_smp_test: " << r << " => " << f(r) << '\n';
    };

    test((kgram_rule_t("0") | "1") & "2" & (kgram_rule_t("3") | "4"));
    test(kgram_rule_t("0") & "1" & "2" & ((kgram_rule_t("3") & "4") | "5"));
    test(kgram_rule_t("0") & (kgram_rule_t("1") | "2") & "3" & ((kgram_rule_t("4") & "5") | "6"));
    test(kgram_rule_t("0") | kgram_rule_t("1") & "2" & "3" | "4");
}

