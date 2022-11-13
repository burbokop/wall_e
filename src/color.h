#ifndef COLOR_H
#define COLOR_H

#include <string>
#include <sstream>
#include "utility/collections.h"

namespace wall_e {
template <typename Arg, typename... Args>
inline void unpuck_to_stream(std::ostream& ss, Arg&& arg, Args&&... args) {
    ss << std::forward<Arg>(arg);
    ((ss << std::forward<Args>(args)), ...);
}

class color {
    std::uint32_t m_argb;
public:
    std::string to_ansi() const;
    std::uint32_t argb() const { return m_argb; };

    static inline std::string foreground(const wall_e::pair<uint8_t, uint8_t>& color) { return "\x1B[" + std::to_string(color.first) + "m"; }
    static inline std::string background(const wall_e::pair<uint8_t, uint8_t>& color) { return "\x1B[" + std::to_string(color.second) + "m"; }
    static inline std::string reset() { return "\033[0m"; }

    inline color(std::uint32_t argb) : m_argb(argb) {}
    inline std::string cover(const std::string &text) const {
        if(m_argb) {
            return to_ansi() + text + reset();
        } else {
            return text;
        }
    }

    bool operator==(const color& other) const {
        return m_argb == other.m_argb;
    }

    template<typename ...Args>
    inline std::string operator()(Args&&... args) const {
        static_assert (sizeof... (args) > 0, "must be at least one argument");
        std::stringstream ss;
        if(m_argb) {
            ss << to_ansi();
        }
        unpuck_to_stream(ss, args...);
        if(m_argb) {
            ss << reset();
        }
        return ss.str();
    }
};

inline const color black          = 0x000000;
inline const color red            = 0x8b0000;
inline const color green          = 0x006400;
inline const color yellow         = 0x8b8b00;
inline const color blue           = 0x00008b;
inline const color magenta        = 0x8b008b;
inline const color cyan           = 0x008b8b;
inline const color white          = 0xfffafa;
inline const color bright_black   = 0x242424;
inline const color bright_red     = 0xff0000;
inline const color bright_green   = 0x00ff00;
inline const color bright_yellow  = 0xffff00;
inline const color bright_blue    = 0x0000ff;
inline const color bright_magenta = 0xff00ff;
inline const color bright_cyan    = 0x00ffff;
inline const color bright_white   = 0xffffff;

inline std::ostream &operator << (std::ostream &stream, const wall_e::color &color) {
    return stream << color.to_ansi();
}

}


#endif // COLOR_H
