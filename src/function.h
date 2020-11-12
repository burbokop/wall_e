#ifndef KM2_FUNCTION_H
#define KM2_FUNCTION_H


#include <string>
#include <vector>
#include "variant.h"
#include "lex.h"


namespace wall_e {



class function {
    static std::string produce_name(const std::string &name, const std::vector<std::string> &typenames);
    std::string m_originalName;
    std::string m_fullName;
    std::vector<std::string> m_argTypes;
public:
    function() {};
    function(const std::string &name, const std::vector<std::string> &typenames);
    function(const std::string &name, const std::vector<std::pair<wall_e::lex::token, wall_e::lex::token> > &typenames);
    std::string originalName() const;
    std::string fullName() const;
    std::vector<std::string> argTypes() const;

    static function choose_overload(const std::list<function> &overloads, const wall_e::variant_vector &args, const std::function<bool(const std::string&, const wall_e::variant&)> &matchRule);
    static std::list<function> find_overloads(const std::string &name, const std::list<function> &functions);
};

std::ostream &operator<<(std::ostream &stream, const function &function);

bool operator==(const function &function0, const function &function1);

}

#endif // KM2_FUNCTION_H
