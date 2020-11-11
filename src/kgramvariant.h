#ifndef TV2_H
#define TV2_H


#include <vector>
#include <ostream>
#include <functional>
#include <cxxabi.h>
#include <sstream>
#include <type_traits>
#include <list>
#include <map>
#include <cassert>

template<typename T>
std::string kgram_type() {
    std::string original_typename = typeid (T).name();
    size_t original_typename_size = original_typename.size();
    int status = 0;
    char *demangle_result = abi::__cxa_demangle(original_typename.c_str(), nullptr, &original_typename_size, &status);
    std::string result = demangle_result;
    if(status == 0) {
        return result;
    } else if(status == -1) {
        printf("kgram_type: Memory allocation failiure occurred.");
    } else if(status == -2) {
        printf("kgram_type: Mangled_name is not a valid name under the C++ ABI mangling rules.");
    } else if(status == -3) {
        printf("kgram_type: One of the arguments is invalid.");
    }
    printf("kgram_type: Undefined error.");
    return "";
}

namespace kgram_stream_operator__ {
    struct no { bool b[2]; };
    template<typename T, typename Arg> no operator<< (T&, const Arg&);

    bool check (...);
    no& check (const no&);

    template <typename T, typename Arg = T>
    struct exists {
        enum { value = (sizeof(check(*(T*)(0) << *(Arg*)(0))) != sizeof(no)) };
    };
}

struct kgram_variant_handle_base_t { virtual ~kgram_variant_handle_base_t() {}; };
template<typename T>
struct kgram_variant_handle_t : kgram_variant_handle_base_t { T value; };

class kgram_variant_t;
std::ostream &operator<<(std::ostream &stream, const kgram_variant_t &arg);
class kgram_variant_t {
    kgram_variant_handle_base_t *m_data = nullptr;
    std::string m_type;

    std::function<void(kgram_variant_handle_base_t*)> m_destructor;
    std::function<kgram_variant_handle_base_t*(kgram_variant_handle_base_t*)> m_copy_constructor;
    std::function<std::string(kgram_variant_handle_base_t*)> m_to_string;

    std::function<bool(kgram_variant_handle_base_t*, kgram_variant_handle_base_t*)> m_comparator;

    bool m_single_print = false;
public:
    kgram_variant_t() {};
    template<typename T>
    kgram_variant_t(T value) { assign(value); }
    kgram_variant_t(const char *value) : kgram_variant_t(std::string(value)) {}
    kgram_variant_t(const kgram_variant_t &obj) { operator=(obj); }
    template<typename T>
    void operator=(T value) { assign(value); }
    void operator=(const kgram_variant_t &obj) {
        m_type = obj.m_type;
        m_destructor = obj.m_destructor;
        m_copy_constructor = obj.m_copy_constructor;
        m_to_string = obj.m_to_string;
        m_comparator = obj.m_comparator;
        if(obj.m_data && obj.m_copy_constructor)
            m_data = obj.m_copy_constructor(obj.m_data);
    }

    ~kgram_variant_t() { if(m_data && m_destructor) { m_destructor(m_data); } }

    template<typename T>
    T value() const {
        assert(contains_type<T>());
        if(m_data)
            return dynamic_cast<kgram_variant_handle_t<T>*>(m_data)->value;
        return T();
    }

    std::string to_string() const { if(m_data && m_to_string) return m_to_string(m_data); return std::string(); }
    bool single_print() const { return m_single_print; };

    template<typename T>
    void assign(T value) {
        auto t = kgram_type<T>();
        if(t != m_type) {

            if(m_data)
                m_destructor(m_data);

            m_data = new kgram_variant_handle_t<T>();
            m_destructor = [](kgram_variant_handle_base_t* obj){
                kgram_variant_handle_t<T>* casted_obj = dynamic_cast<kgram_variant_handle_t<T>*>(obj);
                delete casted_obj;
            };
            m_copy_constructor = [](kgram_variant_handle_base_t* obj) {
                kgram_variant_handle_t<T>* casted_obj = dynamic_cast<kgram_variant_handle_t<T>*>(obj);
                return new kgram_variant_handle_t<T>(*casted_obj);
            };

            m_comparator = [](kgram_variant_handle_base_t* obj1, kgram_variant_handle_base_t* obj2) -> bool {
                return dynamic_cast<kgram_variant_handle_t<T>*>(obj1)->value
                == dynamic_cast<kgram_variant_handle_t<T>*>(obj2)->value;
            };

            if (kgram_stream_operator__::exists<std::ostream, T>::value) {
                m_to_string = [](kgram_variant_handle_base_t* obj) {
                    kgram_variant_handle_t<T>* casted_obj = dynamic_cast<kgram_variant_handle_t<T>*>(obj);
                    std::stringstream ss;
                        ss << casted_obj->value;
                    return ss.str();
                };
            }


            m_type = t;
        }

        dynamic_cast<kgram_variant_handle_t<T>*>(m_data)->value = value;
    }
    std::string type() const { return m_type; }
    template<typename T>
    bool contains_type() const { return m_type == kgram_type<T>(); }

    friend bool operator==(const kgram_variant_t &varian0, const kgram_variant_t &varian1);
};




typedef std::vector<kgram_variant_t> kgram_variant_vector;
typedef std::list<kgram_variant_t> kgram_variant_list;
typedef std::map<std::string, kgram_variant_t> kgram_variant_map;


kgram_variant_vector kgram_variant_constrain(const kgram_variant_t &variant);


std::ostream &operator<<(std::ostream &stream, const std::vector<kgram_variant_t> &vector);
std::ostream &operator<<(std::ostream &stream, const std::list<kgram_variant_t> &vector);




bool kgram_is_number(const kgram_variant_t &variant, std::string expected_token = std::string());
bool kgram_is_number(const std::string& string);


double kgram_to_double(const kgram_variant_t &variant, bool *ok = nullptr);



#endif // TV2_H
