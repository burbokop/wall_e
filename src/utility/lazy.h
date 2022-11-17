#ifndef WALL_E_LAZY_H
#define WALL_E_LAZY_H

#include "collections.h"

namespace wall_e {

template<typename T>
class lazy {
    const std::function<T()> m_factory;
    mutable opt<T> m_data;
public:
    lazy(const lazy<T>&) = delete;
    lazy(const std::function<T()>& factory) : m_factory(factory) {}

    template<typename C>
    lazy(const C *obj, T(C::*f)() const) : lazy([obj, f](){ return (obj->*f)(); }) {}

    const T& value() const {
        if(!m_data) {
            m_data = m_factory();
        }
        return *m_data;
    }
};

}

#endif // LAZY_H
