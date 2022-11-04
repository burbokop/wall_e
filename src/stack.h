#ifndef WALL_E_THREAD_STACK_H
#define WALL_E_THREAD_STACK_H

#include <cstddef>

namespace wall_e {
namespace thread_stack {

bool coming_overflow(std::size_t offset);

}
}

#endif // WALL_E_THREAD_STACK_H
