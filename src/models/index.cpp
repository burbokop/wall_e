#include "index.h"


wall_e::index::index(
        const std::size_t &x,
        const std::size_t &level,
        const std::size_t &area,
        const std::size_t &costruction)
    : m_x(x),
      m_level(level),
      m_area(area),
      m_costruction(costruction) {}

std::size_t wall_e::index::level() const { return m_level; }
std::size_t wall_e::index::costruction() const { return m_costruction; }
std::size_t wall_e::index::area() const { return m_area; }
std::size_t wall_e::index::x() const { return m_x; }
