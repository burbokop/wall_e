#ifndef SMP2_H
#define SMP2_H

#include "../private/gram_private.h"


namespace smp2 {
    wall_e::gram::rule cc(const wall_e::gram::rule &rule);

    wall_e::gram::rule simplify(const wall_e::gram::rule &rule);
}


#endif // SMP2_H
