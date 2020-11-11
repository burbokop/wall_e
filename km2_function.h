#ifndef KM2_FUNCTION_H
#define KM2_FUNCTION_H


#include <string>
#include <vector>
#include "kgramvariant.h"
#include "klex.h"





class km2_function {
    static std::string produce_name(const std::string &name, const std::vector<std::string> &typenames);
    std::string m_originalName;
    std::string m_fullName;
    std::vector<std::string> m_argTypes;
public:
    km2_function() {};
    km2_function(const std::string &name, const std::vector<std::string> &typenames);
    km2_function(const std::string &name, const std::vector<std::pair<klex_token_t, klex_token_t> > &typenames);
    std::string originalName() const;
    std::string fullName() const;
    std::vector<std::string> argTypes() const;

    static km2_function choose_overload(const std::list<km2_function> &overloads, const kgram_variant_vector &args, const std::function<bool(const std::string&, const kgram_variant_t&)> &matchRule);
    static std::list<km2_function> find_overloads(const std::string &name, const std::list<km2_function> &functions);
};

std::ostream &operator<<(std::ostream &stream, const km2_function &function);

bool operator==(const km2_function &function0, const km2_function &function1);



#endif // KM2_FUNCTION_H
