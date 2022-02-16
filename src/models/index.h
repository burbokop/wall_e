#ifndef INDEX_H
#define INDEX_H

#include <cstddef>

namespace wall_e {

class index {
    const std::size_t m_x;
    const std::size_t m_level;
    const std::size_t m_area;
    const std::size_t m_costruction;
public:
    index(
            const std::size_t &x,
            const std::size_t &level,
            const std::size_t &area,
            const std::size_t &costruction
            );
    std::size_t x() const;
    std::size_t level() const;
    std::size_t area() const;
    std::size_t costruction() const;
};

}
#endif // INDEX_H
