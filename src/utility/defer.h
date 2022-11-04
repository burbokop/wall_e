#ifndef WALL_E_DEFER_H
#define WALL_E_DEFER_H

#include <functional>

namespace wall_e {

class defer {
    std::function<void()> m_d;
public:
    defer(const std::function<void()>& d) : m_d(d) {}
    defer(std::nullptr_t) = delete;
    defer(const defer&) = delete;
    ~defer() { if(m_d) m_d(); }
};

}

#endif // WALL_E_DEFER_H
