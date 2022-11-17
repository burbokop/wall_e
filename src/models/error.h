#ifndef WALL_E_ERROR_H
#define WALL_E_ERROR_H

#include "text_segment.h"
#include "../enums.h"

namespace wall_e {

struct error {
    wall_e_enum_member(severity,
        Err = 0,
        Warn,
        Info,
        Hint
    )
    wall_e_enum_member(stage,
        Lexic,
        Gramatic,
        Semantic,
        UndefinedStage
    )
private:
    std::string m_message;
    wall_e::text_segment m_segment;
    severity m_severity = Err;
    stage m_stage = UndefinedStage;
    std::size_t m_kind = 0;
public:
    error(
            const std::string& message = {},
            severity severity = Err,
            stage stage = UndefinedStage,
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
std::ostream& operator<<(std::ostream& stream, const error &error);

} // namespace wall_e




#endif // WALL_E_ERROR_H
