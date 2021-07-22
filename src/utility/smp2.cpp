#include "smp2.h"


wall_e::gram::rule smp2::cc(const wall_e::gram::rule &rule) {
    if (rule.type() == wall_e::gram::rule_type::Conjunction) {
        size_t i = 0;
        for(const auto& child : rule.children()) {
            if(child.type() == wall_e::gram::rule_type::Conjunction) {
                auto result = rule.replaced(i, child.children());
                if (result.has_value()) {
                    return result.value();
                }
            }
            ++i;
        }
    }
    return rule;
}

wall_e::gram::rule smp2::simplify(const wall_e::gram::rule &rule) {
    auto current = rule;
    while (true) {
        auto next = cc(current);
        if(current == next) {
            return next;
        }
        current = next;
    }
}
