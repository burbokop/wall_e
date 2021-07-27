#include "text_segment.h"

#include <ostream>


std::string::size_type wall_e::text_segment::begin() const { return m_begin; }

std::string::size_type wall_e::text_segment::end() const { return m_end; }

std::string::size_type wall_e::text_segment::length() const {
    return m_end - m_begin;
}

bool wall_e::text_segment::valid_direction() const {
    return m_end >= m_begin;
}

wall_e::text_segment::text_segment() {}

wall_e::text_segment::text_segment(std::string::size_type begin, std::string::size_type end)
    : m_begin(begin), m_end(end) {}


bool operator==(const wall_e::text_segment &seg0, const wall_e::text_segment &seg1) {
    return seg0.begin() == seg1.begin() && seg0.end() == seg1.end();
}

std::ostream &operator<<(std::ostream &stream, const wall_e::text_segment &seg) {
    return stream << "{ " << seg.begin() << ", " << seg.end();
}
