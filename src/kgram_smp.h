#ifndef KGRAM_SMP_H
#define KGRAM_SMP_H

#include "kgram_tools.h"

kgram_rule_t kgram_smp(const kgram_rule_t &rule);
typedef std::pair<kgram_rule_t, kgram_rule_type_t::enum_t> __kgram_smp_arg_t;
__kgram_smp_arg_t __kgram_smp(const kgram_rule_t &arg, kgram_rule_type_t::enum_t last_type);


void kgram_smp_test(kgram_rule_t (*f)(const kgram_rule_t &rule));

#endif // KGRAM_SMP_H
