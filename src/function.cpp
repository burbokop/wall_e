#include "function.h"
namespace wall_e {


std::string function::produce_name(const std::string &name, const std::vector<std::string> &typenames) {
    std::string result = name;
    for(auto t : typenames) {
        result += "_" + t;
    }
    return result;
}

std::vector<std::string> function::argTypes() const {
    return m_argTypes;
}

function function::choose_overload(const std::list<function> &overloads, const wall_e::variant_vector &args, const std::function<bool (const std::string&, const wall_e::variant&)> &matchRule) {
    for(auto o : overloads) {
        const auto types = o.argTypes();
        if(types.size() == args.size()) {
            for(size_t i = 0; i < types.size(); ++i) {
                if(matchRule(types[i], args[i])) {
                    return o;
                }
            }
        }
    }
    return function();
}

std::list<function> function::find_overloads(const std::string &name, const std::list<function> &functions) {
    std::list<function> result;
    for(auto f : functions) {
        if(f.originalName() == name) {
            result.push_back(f);
        }
    }
    return result;
}

std::string function::fullName() const {
    return m_fullName;
}

std::string function::originalName() const {
    return m_originalName;
}

function::function(const std::string &name, const std::vector<std::string> &typenames) {
    m_originalName = name;
    m_argTypes = typenames;
    m_fullName = produce_name(name, typenames);
}

function::function(const std::string &name, const std::vector<std::pair<wall_e::lex::token, wall_e::lex::token> > &typenames) {
    std::vector<std::string> tn;
    tn.resize(typenames.size());
    for(size_t i = 0, count = typenames.size(); i < count; ++i) {
        tn[i] = typenames[i].first.name;
    }

    m_originalName = name;
    m_argTypes = tn;
    m_fullName = produce_name(name, tn);
}

std::ostream &operator<<(std::ostream &stream, const function &function) {
    stream << function.fullName();
    return stream;
}

bool operator==(const function &function0, const function &function1) {
    return function0.fullName() == function1.fullName();
}
}
