#ifndef WALL_E_MACRO_H
#define WALL_E_MACRO_H

#if _MSC_VER && !__INTEL_COMPILER
#define wall_e_this_function __FUNCSIG__
#else
#define wall_e_this_function __PRETTY_FUNCTION__
#endif

#define wall_e_lambda(BODY) [=](const auto& _){ BODY }

#endif // WALL_E_MACRO_H
