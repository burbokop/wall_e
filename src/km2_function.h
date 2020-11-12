#ifndef KM2_FUNCTION_H
#define KM2_FUNCTION_H


#include <string>
#include <vector>
#include "variant.h"
#include "lex.h"





class km2_function {
    static std::string produce_name(const std::string &name, const std::vector<std::string> &typenames);
    std::string m_originalName;
    std::string m_fullName;
    std::vector<std::string> m_argTypes;
public:
    km2_function() {};
    km2_function(const std::string &name, const std::vector<std::string> &typenames);
    km2_function(const std::string &name, const std::vector<std::pair<wall_e::lex::Token, wall_e::lex::Token> > &typenames);
    std::string originalName() const;
    std::string fullName() const;
    std::vector<std::string> argTypes() const;

    static km2_function choose_overload(const std::list<km2_function> &overloads, const wall_e::variant_vector &args, const std::function<bool(const std::string&, const wall_e::variant&)> &matchRule);
    static std::list<km2_function> find_overloads(const std::string &name, const std::list<km2_function> &functions);
};

std::ostream &operator<<(std::ostream &stream, const km2_function &function);

bool operator==(const km2_function &function0, const km2_function &function1);



#endif // KM2_FUNCTION_H
