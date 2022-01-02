#include "stack.h"

#include <stdint.h>
#include <sys/resource.h>

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>



bool wall_e::stack::coming_overflow(std::size_t offset) {
    return alloca(100000) == nullptr;
}
