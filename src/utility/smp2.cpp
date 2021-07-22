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

wall_e::gram::rule smp2::dd(const wall_e::gram::rule &rule) {
    if (rule.type() == wall_e::gram::rule_type::Disjunction) {
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

wall_e::gram::rule smp2::cd(const wall_e::gram::rule &rule) {
    return rule;
}

wall_e::gram::rule smp2::dc(const wall_e::gram::rule &rule) {
    if (rule.type() == wall_e::gram::rule_type::Disjunction) {
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
