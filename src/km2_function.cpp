#include "km2_function.h"


std::string km2_function::produce_name(const std::string &name, const std::vector<std::string> &typenames) {
    std::string result = name;
    for(auto t : typenames) {
        result += "_" + t;
    }
    return result;
}

std::vector<std::string> km2_function::argTypes() const {
    return m_argTypes;
}

km2_function km2_function::choose_overload(const std::list<km2_function> &overloads, const kgram_variant_vector &args, const std::function<bool (const std::string&, const kgram_variant_t&)> &matchRule) {
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
    return km2_function();
}

std::list<km2_function> km2_function::find_overloads(const std::string &name, const std::list<km2_function> &functions) {
    std::list<km2_function> result;
    for(auto f : functions) {
        if(f.originalName() == name) {
            result.push_back(f);
        }
    }
    return result;
}

std::string km2_function::fullName() const {
    return m_fullName;
}

std::string km2_function::originalName() const {
    return m_originalName;
}

km2_function::km2_function(const std::string &name, const std::vector<std::string> &typenames) {
    m_originalName = name;
    m_argTypes = typenames;
    m_fullName = produce_name(name, typenames);
}

km2_function::km2_function(const std::string &name, const std::vector<std::pair<klex_token_t, klex_token_t> > &typenames) {
    std::vector<std::string> tn;
    tn.resize(typenames.size());
    for(size_t i = 0, count = typenames.size(); i < count; ++i) {
        tn[i] = typenames[i].first.name;
    }

    m_originalName = name;
    m_argTypes = tn;
    m_fullName = produce_name(name, tn);
}

std::ostream &operator<<(std::ostream &stream, const km2_function &function) {
    stream << function.fullName();
    return stream;
}

bool operator==(const km2_function &function0, const km2_function &function1) {
    return function0.fullName() == function1.fullName();
}
