#include "error.h"

#include <ostream>


std::string wall_e::error::message() const { return m_message; }
wall_e::text_segment wall_e::error::segment() const { return m_segment; }
wall_e::error::severity wall_e::error::sev() const { return m_severity; }
wall_e::error::stage wall_e::error::stg() const { return m_stage; }
std::size_t wall_e::error::kind() const { return m_kind; }

wall_e::error::error(
        const std::string& message,
        severity severity,
        stage stage,
        std::size_t kind,
        const wall_e::text_segment& segment
        )
    : m_message(message),
      m_severity(severity),
      m_stage(stage),
      m_kind(kind),
      m_segment(segment) {}

bool wall_e::operator==(const wall_e::error &error0, const wall_e::error &error1) {
    return error0.sev() == error1.sev()
            && error0.segment() == error1.segment()
            && error0.stg() == error1.stg()
            && error0.kind() == error1.kind();
}

std::ostream &wall_e::operator<<(std::ostream &stream, const wall_e::error &error) {
    return stream << "{ message: " << error.message()
                  << ", segment: " << error.segment()
                  << ", sev: " << error.sev()
                  << ", stg: " << error.stg()
                  << ", kind: " << error.kind()
                  << " }";
}

