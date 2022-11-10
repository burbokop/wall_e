#ifndef VARIANT_H
#define VARIANT_H


#include <vector>
#include <ostream>
#include <functional>
#include <sstream>
#include <type_traits>
#include <list>
#include <map>
#include <cassert>
#include <type_traits>
#include <memory>
#include "../utility/typename.h"
#include "../utility/collections.h"

namespace wall_e {

namespace stream_operator {
struct no { bool b[2]; };
template<typename T, typename Arg> no operator<< (T&, const Arg&);

bool check (...);
no& check (const no&);

template <typename T, typename Arg = T>
struct exists {
    enum { value = (sizeof(check(*(T*)(0) << *(Arg*)(0))) != sizeof(no)) };
};
}


template <typename T>
class has_super_type {
private:
    typedef char yes_type[1];
    typedef char no_type[2];

    template <typename C> static yes_type& test(typename C::super_type*) ;
    template <typename C> static no_type& test(...);
public:
    enum { value = sizeof(test<T>(0)) == sizeof(yes_type) };
};

template <typename T>
class has_element_type {
private:
    typedef char yes_type[1];
    typedef char no_type[2];

    template <typename C> static yes_type& test(typename C::element_type*) ;
    template <typename C> static no_type& test(...);
public:
    enum { value = sizeof(test<T>(0)) == sizeof(yes_type) };
};

template <typename T>
class has_super_shared_type {
private:
    typedef char yes_type[1];
    typedef char no_type[2];

    template <typename C> static yes_type& test(typename C::element_type::super_type*) ;
    template <typename C> static no_type& test(...);
public:
    enum { value = sizeof(test<T>(0)) == sizeof(yes_type) };
};



template<typename T>
wall_e::list<std::string> class_lineage() {
    if constexpr (has_super_type<T>::value) {
        static const wall_e::list<std::string> lineage = []{
            auto list = class_lineage<typename T::super_type>();
            list.push_front(wall_e::type_name<T>());
            return list;
        }();
        return lineage;
    } else {
        static const wall_e::list<std::string> lineage = { wall_e::type_name<T>() };
        return lineage;
    }
}

template<typename T>
wall_e::list<std::string> shared_lineage() {
    if constexpr (has_super_shared_type<T>::value) {
        static const wall_e::list<std::string> lineage = []{
            auto list = shared_lineage<std::shared_ptr<typename T::element_type::super_type>>();
            list.push_front(wall_e::type_name<T>());
            return list;
        }();
        return lineage;
    } else {
        static const wall_e::list<std::string> lineage = { wall_e::type_name<T>() };
        return lineage;
    }
}

template<typename T>
wall_e::list<std::string> shared_elem_lineage() {
    if constexpr (has_super_shared_type<T>::value) {
        static const wall_e::list<std::string> lineage = []{
            auto list = class_lineage<typename T::element_type::super_type>();
            list.push_front(wall_e::type_name<typename T::element_type>());
            return list;
        }();
        return lineage;
    } else {
        static const wall_e::list<std::string> lineage = { wall_e::type_name<T>() };
        return lineage;
    }
}

class variant;
typedef wall_e::vec<variant> variant_vector;
typedef wall_e::list<variant> variant_list;
typedef std::map<std::string, variant> variant_map;

variant_vector constrain_variant(const variant &variant);

struct variant_handle_base_t { virtual ~variant_handle_base_t() {}; };

template<typename L> class left;
template<typename R> class right;
template<typename EL, typename ER> class either;

template<typename T>
struct variant_handle_t : variant_handle_base_t { T value; };

template<typename L, typename R>
struct variant_handle_t<either<L, R>> : variant_handle_base_t { either<L, R> value = right<R>(R()); };

std::ostream &operator<<(std::ostream &stream, const variant &arg);

class variant {
    variant_handle_base_t *m_data = nullptr;
    std::string m_type;
    wall_e::list<std::string> m_lineage;

    std::function<void(variant_handle_base_t*)> m_destructor;
    std::function<variant_handle_base_t*(variant_handle_base_t*)> m_copy_constructor;
    std::function<std::string(variant_handle_base_t*)> m_to_string;

    std::function<void*(variant_handle_base_t*)> m_addr;
    std::function<std::shared_ptr<void>(variant_handle_base_t*)> m_shared_addr;

    std::function<bool(variant_handle_base_t*, variant_handle_base_t*)> m_comparator;

    bool m_single_print = false;

public:
    template<typename T>
    variant(const T& value) { assign(value); }
    variant() {};
    variant(const char *value) : variant(std::string(value)) {}

    variant(const variant &obj) { operator=(obj); }
    template<typename T>
    void operator=(T value) { assign(value); }
    void operator=(const variant &obj) {
        m_type = obj.m_type;
        m_lineage = obj.m_lineage;
        m_destructor = obj.m_destructor;
        m_copy_constructor = obj.m_copy_constructor;
        m_to_string = obj.m_to_string;
        m_comparator = obj.m_comparator;
        m_addr = obj.m_addr;
        m_shared_addr = obj.m_shared_addr;
        if(obj.m_data && obj.m_copy_constructor)
            m_data = obj.m_copy_constructor(obj.m_data);
    }

    ~variant() { if(m_data && m_destructor) { m_destructor(m_data); } }

    inline bool is_null() const { return !m_data; }

    template<typename T>
    inline T value() const {
        if(!contains_type<T>()) {
            throw std::runtime_error("wall_e::variant::value: actual type: " + m_type + " expected: " + type_name<T>());
        }
        if(m_data)
            return dynamic_cast<variant_handle_t<T>*>(m_data)->value;
        return T();
    }

    template<typename T>
    inline bool inherited_by() const {
        const auto tt = type_name<typename std::remove_pointer<T>::type>();
        for(const auto& ll : m_lineage) {
            if(ll == tt) {
                return true;
            }
        }
        return false;
    }

    template<typename T>
    inline T cast() const {
        const auto opt = option_cast<T>();
        if(opt.has_value()) {
            return opt.value();
        } else {
            throw std::runtime_error("wall_e::variant::cast: actual lineage: " + lineage_str() + " expected type: " + type_name<T>());
        }
    }

    template<typename T>
    inline wall_e::opt<T> option() const {
        if (!contains_type<T>()) {
            return std::nullopt;
        } else if(m_data) {
            return dynamic_cast<variant_handle_t<T>*>(m_data)->value;
        }
        return T();
    }

    template<typename T>
    inline wall_e::opt<T> option_cast() const {
        static_assert (std::is_pointer<T>::value || has_element_type<T>::value, "template type must be a pointer or shared_ptr");
        if constexpr(std::is_pointer<T>::value) {
            if (!inherited_by<T>())
                return std::nullopt;

            if(m_data && m_addr)
                return reinterpret_cast<T>(m_addr(m_data));
            return nullptr;
        } else if constexpr(has_element_type<T>::value) {
            if (!inherited_by<T>())
                return std::nullopt;

            if(m_data && m_shared_addr)
                return std::reinterpret_pointer_cast<typename T::element_type>(m_shared_addr(m_data));
            return nullptr;
        }
        return T();
    }

    template<typename T>
    inline T cast_or(const T &default_value = nullptr) const {
        if(const auto opt = option_cast<T>()) {
            return *opt;
        } else {
            return default_value;
        }
    }


    template<typename T>
    inline T value_or(const T& def = T()) const {
        if(contains_type<T>() && m_data)
            return dynamic_cast<variant_handle_t<T>*>(m_data)->value;
        return def;
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

            if (stream_operator::exists<std::ostream, T>::value) {
                m_to_string = [](variant_handle_base_t* obj) {
                    variant_handle_t<T>* casted_obj = dynamic_cast<variant_handle_t<T>*>(obj);
                    std::stringstream ss;
                    ss << casted_obj->value;
                    return ss.str();
                };
            }

            if constexpr(std::is_pointer<T>::value) {
                m_addr = [](variant_handle_base_t* obj) -> void* {
                    variant_handle_t<T>* casted_obj = dynamic_cast<variant_handle_t<T>*>(obj);
                    return casted_obj->value;
                };
            } else if constexpr(has_element_type<T>::value) {
                m_shared_addr = [](variant_handle_base_t* obj) -> std::shared_ptr<void> {
                    variant_handle_t<T>* casted_obj = dynamic_cast<variant_handle_t<T>*>(obj);
                    return casted_obj->value;
                };
            }


            m_type = t;
            if constexpr (has_super_shared_type<T>::value) {
                m_lineage = shared_lineage<typename std::remove_pointer<T>::type>();
            } else {
                m_lineage = class_lineage<typename std::remove_pointer<T>::type>();
            }
        }

        dynamic_cast<variant_handle_t<T>*>(m_data)->value = value;
    }
    const std::string& type() const { return m_type; }
    wall_e::list<std::string> lineage() const { return m_lineage; }
    std::string lineage_str() const {
        std::stringstream ss;
        ss << m_lineage;
        return ss.str();
    }
    template<typename T>
    bool contains_type() const { return m_type == type_name<T>(); }

    friend bool operator==(const variant &varian0, const variant &varian1);

    inline variant_vector constrain() const { return constrain_variant(*this); }

    template<typename R, typename T>
    inline variant map(const std::function<R(const T&)>& f) const {
        if(const auto& opt  = option<T>()) {
            return f(*opt);
        }
        return *this;
    }
};

bool is_number(const variant &variant, std::string expected_token = std::string());
bool is_number(const std::string& string);


double to_double(const variant &variant, bool *ok = nullptr);

}


#endif // TV2_H
