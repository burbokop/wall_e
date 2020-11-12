#ifndef KGRAM_SMP_H
#define KGRAM_SMP_H

#include "kgram_tools.h"

namespace wall_e {
namespace gram {


rule smp(const rule &rule);
typedef std::pair<rule, rule_type_t::enum_t> __smp_arg_t;
__smp_arg_t __smp(const rule &arg, rule_type_t::enum_t last_type);


void smp_test(rule (*f)(const rule &rule));

}
}

#endif // KGRAM_SMP_H
