#ifndef KM2_TEXT_SEGMENT_H
#define KM2_TEXT_SEGMENT_H

#include <string>

namespace wall_e {

class text_segment {
    std::string::size_type m_begin;
    std::string::size_type m_end;
public:
    text_segment();
    text_segment(
            std::string::size_type begin,
            std::string::size_type end
            );
    std::string::size_type begin() const;
    std::string::size_type end() const;

    std::string::size_type length() const;

    bool valid_direction() const;
};

bool operator==(const wall_e::text_segment &seg0, const wall_e::text_segment &seg1);
std::ostream &operator<<(std::ostream& stream, const wall_e::text_segment &seg);

} // namespace km2




#endif // KM2_TEXT_SEGMENT_H
