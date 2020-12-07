#ifndef COLOR_H
#define COLOR_H

#include <string>
#include <sstream>

namespace wall_e {
namespace color {
static inline std::string f(const std::pair<uint8_t, uint8_t>& color) { return "\x1B[" + std::to_string(color.first) + "m"; }
static inline std::string b(const std::pair<uint8_t, uint8_t>& color) { return "\x1B[" + std::to_string(color.second) + "m"; }
inline std::string reset() { return "\033[0m"; }

class color_t {
    std::pair<uint8_t, uint8_t> data;
public:
    color_t(uint8_t foreground, uint8_t background) { data = { foreground, background }; }
    inline std::string f() const { return color::f(data); }
    inline std::string b() const { return color::b(data); }
    inline std::string cover(const std::string &text) const { return f() + text + reset(); }
    inline std::string operator()(const std::string &text) const { return f() + text + reset(); }
    template<typename T>
    inline std::string operator()(const T &value) const {
        std::stringstream ss;
        ss << f() << value << reset();
        return ss.str();
    }
};

inline const color_t Black         = { 30,  40 };
inline const color_t Red           = { 31,  41 };
inline const color_t Green         = { 32,  42 };
inline const color_t Yellow        = { 33,  43 };
inline const color_t Blue          = { 34,  44 };
inline const color_t Magenta       = { 35,  45 };
inline const color_t Cyan          = { 36,  46 };
inline const color_t White         = { 37,  47 };
inline const color_t BrightBlack   = { 90, 100 };
inline const color_t BrightRed     = { 91, 101 };
inline const color_t BrightGreen   = { 92, 102 };
inline const color_t BrightYellow  = { 93, 103 };
inline const color_t BrightBlue    = { 94, 104 };
inline const color_t BrightMagenta = { 95, 105 };
inline const color_t BrightCyan    = { 96, 106 };
inline const color_t BrightWhite   = { 97, 107 };
}
}

inline std::ostream &operator << (std::ostream &stream, const wall_e::color::color_t &color) {
    stream << color.f();
    return stream;
}

#endif // COLOR_H
