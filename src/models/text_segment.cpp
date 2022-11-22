#include "text_segment.h"

#include <ostream>


std::string::size_type wall_e::text_segment::begin() const { return m_begin; }

std::string::size_type wall_e::text_segment::end() const { return m_end; }

std::string::size_type wall_e::text_segment::length() const {
    return m_end - m_begin;
}

bool wall_e::text_segment::valid_direction() const {
    return m_begin < m_end;
}

wall_e::list<wall_e::text_segment> wall_e::text_segment::split_by_mask(const list<text_segment> &mask) const {
    list<text_segment> result;
    auto it = mask.begin();
    for(; it != mask.end(); ++it) {
        const auto item = *this * *it;
        if(item.valid_direction()) {
            result.push_back(item);
        }
        if(it->m_begin >= m_end) break;
    }
    return result;
}

wall_e::list<wall_e::text_segment> wall_e::text_segment::lines(const std::string& uri, const std::string &text, bool including_endl) {
    wall_e::list<wall_e::text_segment> result;
    std::size_t current_line_start = 0;
    std::size_t i = 0;
    for(const auto& c : text) {
        if(c == '\n') {
            result.push_back(text_segment(uri, current_line_start, i + (including_endl ? 1 : 0)));
            current_line_start = i + 1;
        }
        ++i;
    }
    return result;
}

wall_e::text_segment::text_segment() {}

wall_e::text_segment::text_segment(const std::string &uri, std::string::size_type begin, std::string::size_type end)
    : m_uri(uri),
      m_begin(begin),
      m_end(end) {}


wall_e::text_segment wall_e::operator+(const wall_e::text_segment &seg0, const wall_e::text_segment &seg1) {
    if(seg0.uri() == seg1.uri()) {
        return wall_e::text_segment(
            seg0.uri(),
            std::min(seg0.begin(), seg1.begin()),
            std::max(seg0.end(), seg1.end())
            );
    } else {
        return wall_e::text_segment();
    }
}

wall_e::text_segment wall_e::operator*(const wall_e::text_segment &seg0, const wall_e::text_segment &seg1) {
    if(seg0.uri() == seg1.uri()) {
        return wall_e::text_segment(
            seg0.uri(),
            std::max(seg0.begin(), seg1.begin()),
            std::min(seg0.end(), seg1.end())
            );
    } else {
        return wall_e::text_segment();
    }
}


bool wall_e::operator<(const wall_e::text_segment &seg0, const wall_e::text_segment &seg1) {
    return seg0.begin() < seg1.begin() && seg0.end() < seg1.end();
}

bool wall_e::operator==(const wall_e::text_segment &seg0, const wall_e::text_segment &seg1) {
    return seg0.begin() == seg1.begin() && seg0.end() == seg1.end();
}

std::ostream &wall_e::operator<<(std::ostream &stream, const wall_e::text_segment &seg) {
    return stream << "{ " << seg.begin() << ", " << seg.end() << " }";
}
/*
wall_e::text_segment::predicate wall_e::text_segment::line_char_predicate(std::size_t line, std::size_t character) {
    return [=](const text_segment& segment) {
        std::size_t l = 0;
        for(std::size_t i = 0; i < text.size(); ++i) {
            if(text[i] == '\n') {
                if(l++ == line) {
                    return offset_predicate(i + character)(segment, text);
                }
            }
        }
        return false;
    };
}
*/
wall_e::text_segment::predicate wall_e::text_segment::offset_predicate(std::size_t offset) {
    return [=](const text_segment& segment){
        if(segment.m_begin <= offset && offset < segment.m_end) {
            return true;
        }
        return false;
    };
}

wall_e::text_segment::predicate wall_e::text_segment::eq_predicate(const text_segment &other) {
    return [=](const text_segment& segment){ return segment == other; };
}
