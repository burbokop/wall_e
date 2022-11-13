#include "color.h"


std::string wall_e::color::to_ansi() const {
    const auto& r = std::to_string(std::uint8_t(m_argb >> 16));
    const auto& g = std::to_string(std::uint8_t(m_argb >> 8));
    const auto& b = std::to_string(std::uint8_t(m_argb >> 0));
    return "\033[38;2;" + r + ";" + g + ";" + b + "m";
}
