#include "gram_smp.h"


wall_e::gram::rule wall_e::smp::cc(const wall_e::gram::rule &rule) {
    if (!rule.isNull() && rule.type() == wall_e::gram::rule_type::Conjunction) {
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

wall_e::gram::rule wall_e::smp::dd(const wall_e::gram::rule &rule) {
    if (!rule.isNull() && rule.type() == wall_e::gram::rule_type::Disjunction) {
        size_t i = 0;
        for(const auto& child : rule.children()) {
            if(child.type() == wall_e::gram::rule_type::Disjunction) {
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

wall_e::gram::rule wall_e::smp::cd(const wall_e::gram::rule &rule) {
    return rule;
}

wall_e::gram::rule wall_e::smp::dc(const wall_e::gram::rule &rule) {
    return rule;
}

wall_e::gram::rule wall_e::smp::simplify(const wall_e::gram::rule &rule) {
    auto funcs = { cc, dd };

    auto current = rule;
    while (true) {
        size_t c = 0;
        for (const auto& f : funcs) {
            wall_e::gram::rule next = f(current);
            if(current == next) {
                ++c;
            }
            current = next;
        }
        if(c == funcs.size()) {
            return current;
        }
    }
}
