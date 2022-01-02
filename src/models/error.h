#ifndef WALL_E_ERROR_H
#define WALL_E_ERROR_H

#include "text_segment.h"

namespace wall_e {

struct error {
    enum severity { err = 0, warn, info, hint };
    enum stage { lexic, gramatic, semantic, undefined_stage };
private:
    std::string m_message;
    wall_e::text_segment m_segment;
    severity m_severity = err;
    stage m_stage = undefined_stage;
    std::size_t m_kind = 0;
public:
    error(
            const std::string& message = {},
            severity severity = err,
            stage stage = undefined_stage,
            std::size_t kind = 0,
            const wall_e::text_segment& segment = {}
            );

    std::string message() const;
    wall_e::text_segment segment() const;

    severity sev() const;
    stage stg() const;
    std::size_t kind() const;
};

bool operator==(const wall_e::error &error0, const wall_e::error &error1);

} // namespace wall_e


std::ostream& operator<<(std::ostream& stream, const wall_e::error &error);
std::ostream& operator<<(std::ostream& stream, const wall_e::error::severity &severity);
std::ostream& operator<<(std::ostream& stream, const wall_e::error::stage &stage);


#endif // WALL_E_ERROR_H
