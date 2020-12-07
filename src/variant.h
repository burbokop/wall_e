#ifndef VARIANT_H
#define VARIANT_H


#include <vector>
#include <ostream>
#include <functional>
#include <cxxabi.h>
#include <sstream>
#include <type_traits>
#include <list>
#include <map>
#include <cassert>

namespace wall_e {

template<typename T>
std::string type_name() {
    std::string original_typename = typeid (T).name();
    size_t original_typename_size = original_typename.size();
    int status = 0;
    char *demangle_result = abi::__cxa_demangle(original_typename.c_str(), nullptr, &original_typename_size, &status);
    std::string result = demangle_result;
    if(status == 0) {
        return result;
    } else if(status == -1) {
        printf("wall_e::type: Memory allocation failiure occurred.");
    } else if(status == -2) {
        printf("wall_e::type: Mangled_name is not a valid name under the C++ ABI mangling rules.");
    } else if(status == -3) {
        printf("wall_e::type: One of the arguments is invalid.");
    }
    printf("wall_e::type: Undefined error.");
    return "";
}

namespace stream_operator__ {
    struct no { bool b[2]; };
    template<typename T, typename Arg> no operator<< (T&, const Arg&);

    bool check (...);
    no& check (const no&);

    template <typename T, typename Arg = T>
    struct exists {
        enum { value = (sizeof(check(*(T*)(0) << *(Arg*)(0))) != sizeof(no)) };
    };
}

class variant;
typedef std::vector<variant> variant_vector;
typedef std::list<variant> variant_list;
typedef std::map<std::string, variant> variant_map;

variant_vector constrain_variant(const variant &variant);


struct variant_handle_base_t { virtual ~variant_handle_base_t() {}; };
template<typename T>
struct variant_handle_t : variant_handle_base_t { T value; };

std::ostream &operator<<(std::ostream &stream, const variant &arg);
class variant {
    variant_handle_base_t *m_data = nullptr;
    std::string m_type;

    std::function<void(variant_handle_base_t*)> m_destructor;
    std::function<variant_handle_base_t*(variant_handle_base_t*)> m_copy_constructor;
    std::function<std::string(variant_handle_base_t*)> m_to_string;

    std::function<bool(variant_handle_base_t*, variant_handle_base_t*)> m_comparator;

    bool m_single_print = false;
public:
    variant() {};
    template<typename T>
    variant(T value) { assign(value); }
    variant(const char *value) : variant(std::string(value)) {}
    variant(const variant &obj) { operator=(obj); }
    template<typename T>
    void operator=(T value) { assign(value); }
    void operator=(const variant &obj) {
        m_type = obj.m_type;
        m_destructor = obj.m_destructor;
        m_copy_constructor = obj.m_copy_constructor;
        m_to_string = obj.m_to_string;
        m_comparator = obj.m_comparator;
        if(obj.m_data && obj.m_copy_constructor)
            m_data = obj.m_copy_constructor(obj.m_data);
    }

    ~variant() { if(m_data && m_destructor) { m_destructor(m_data); } }

    template<typename T>
    T value() const {
        assert(contains_type<T>());
        if(m_data)
            return dynamic_cast<variant_handle_t<T>*>(m_data)->value;
        return T();
    }

    std::string to_string() const { if(m_data && m_to_string) return m_to_string(m_data); return std::string(); }
    bool single_print() const { return m_single_print; };

    template<typename T>
    void assign(T value) {
        auto t = type_name<T>();
        if(t != m_type) {

            if(m_data)
                m_destructor(m_data);

            m_data = new variant_handle_t<T>();
            m_destructor = [](variant_handle_base_t* obj){
                variant_handle_t<T>* casted_obj = dynamic_cast<variant_handle_t<T>*>(obj);
                delete casted_obj;
            };
            m_copy_constructor = [](variant_handle_base_t* obj) {
                variant_handle_t<T>* casted_obj = dynamic_cast<variant_handle_t<T>*>(obj);
                return new variant_handle_t<T>(*casted_obj);
            };

            m_comparator = [](variant_handle_base_t* obj1, variant_handle_base_t* obj2) -> bool {
                return dynamic_cast<variant_handle_t<T>*>(obj1)->value
                == dynamic_cast<variant_handle_t<T>*>(obj2)->value;
            };

            if (stream_operator__::exists<std::ostream, T>::value) {
                m_to_string = [](variant_handle_base_t* obj) {
                    variant_handle_t<T>* casted_obj = dynamic_cast<variant_handle_t<T>*>(obj);
                    std::stringstream ss;
                        ss << casted_obj->value;
                    return ss.str();
                };
            }


            m_type = t;
        }

        dynamic_cast<variant_handle_t<T>*>(m_data)->value = value;
    }
    std::string type() const { return m_type; }
    template<typename T>
    bool contains_type() const { return m_type == type_name<T>(); }

    friend bool operator==(const variant &varian0, const variant &varian1);

    inline variant_vector constrain() const { return constrain_variant(*this); }
};








std::ostream &operator<<(std::ostream &stream, const std::vector<variant> &vector);
std::ostream &operator<<(std::ostream &stream, const std::list<variant> &vector);




bool is_number(const variant &variant, std::string expected_token = std::string());
bool is_number(const std::string& string);


double to_double(const variant &variant, bool *ok = nullptr);

}


#endif // TV2_H
