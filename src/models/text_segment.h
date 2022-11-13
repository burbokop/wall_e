#ifndef WALL_E_TEXT_SEGMENT_H
#define WALL_E_TEXT_SEGMENT_H

#include <string>
#include <functional>

namespace wall_e {

class text_segment {
    std::string::size_type m_begin = 0;
    std::string::size_type m_end = 0;
public:
    text_segment();
    text_segment(
            std::string::size_type begin,
            std::string::size_type end
            );
    std::string::size_type begin() const;
    std::string::size_type end() const;
    std::string::size_type length() const;

    typedef std::function<bool(const text_segment&, const std::string&)> predicate;
    static predicate line_char_predicate(std::size_t line, std::size_t character);
    static predicate offset_predicate(std::size_t offset);
    static predicate eq_predicate(const text_segment& other);

    bool valid_direction() const;
};

wall_e::text_segment operator+(const wall_e::text_segment &seg0, const wall_e::text_segment &seg1);
wall_e::text_segment operator*(const wall_e::text_segment &seg0, const wall_e::text_segment &seg1);
bool operator==(const wall_e::text_segment &seg0, const wall_e::text_segment &seg1);
bool operator<(const wall_e::text_segment &seg0, const wall_e::text_segment &seg1);
std::ostream &operator<<(std::ostream& stream, const wall_e::text_segment &seg);

} // namespace km2




#endif // WALL_E_TEXT_SEGMENT_H
