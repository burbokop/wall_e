#ifndef WALL_E_COLLECTIONS_H
#define WALL_E_COLLECTIONS_H

#include <vector>
#include <optional>
#include <ostream>
#include <list>
#include <functional>
#include <map>
#include <stack>
#include <numeric>

#ifdef QT_CORE_LIB
    #include <QDebug>
#endif

namespace wall_e {

template<typename T>
class opt : public std::optional<T> {
public:
    using std::optional<T>::optional;

    template<typename R>
    inline opt<R> map(const std::function<R(const T&)>& f) const {
        if(this->has_value()) {
            return f(this->value());
        } else {
            return std::nullopt;
        }
    }

    template<typename R, typename Arg>
    constexpr inline typename std::enable_if<std::is_class<T>::value, opt<R>>::type map_member(Arg &&f) const {
        if(this->has_value()) {
            return this->value().*f;
        } else {
            return std::nullopt;
        }
    }

    template<typename R, typename Arg>
    constexpr inline typename std::enable_if<std::is_class<T>::value, opt<R>>::type map_member_func(Arg &&f) const {
        if(this->has_value()) {
            return (this->value().*f)();
        } else {
            return std::nullopt;
        }
    }

    template<typename R>
    inline opt<R> flat_map(const std::function<opt<R>(const T&)>& f) const {
        if(this->has_value()) {
            return f(this->value());
        } else {
            return std::nullopt;
        }
    }

    inline bool exists(const std::function<bool(const T&)>& f) const {
        if(this->has_value()) {
            return f(this->value());
        } else {
            return false;
        }
    }

    const opt<T>& filter(const std::function<bool(const T&)>& f) const {
        if(this->has_value() && f(this->value())) {
            return *this;
        } else {
            static const opt<T> nullopt = std::nullopt;
            return nullopt;
        }
    }
};

template<typename R, typename T> using member_ptr = typename std::conditional<std::is_class<T>::value, R(T::*), R*>::type;
template<typename R, typename T> using member_func_ptr = typename std::conditional<std::is_class<T>::value, R(T::*)() const, R(*)()>::type;

template<typename R, typename T, typename RAlloc, typename Alloc, template <typename, typename> typename C>
inline C<R, RAlloc>& map_collection(C<R, RAlloc>& output, const C<T, Alloc>& input, const std::function<R(const T&)>& f) {
    for(const auto& i : input) { output.push_back(f(i)); } return output;
}

template<typename R, typename T, typename RAlloc, typename Alloc, template <typename, typename> typename C>
inline C<R, RAlloc>& map_member_collection(C<R, RAlloc>& output, const C<T, Alloc>& input, member_ptr<R, T> f) {
    for(const auto& i : input) { output.push_back(i.*f); } return output;
}

template<typename R, typename T, typename RAlloc, typename Alloc, template <typename, typename> typename C>
inline C<R, RAlloc>& map_member_func_collection(C<R, RAlloc>& output, const C<T, Alloc>& input, member_func_ptr<R, T> f) {
    for(const auto& i : input) { output.push_back((i.*f)()); } return output;
}

//template<typename R, typename T, template <typename> typename C, typename Alloc = std::allocator<T>>
//inline C<const R&>&& map_collection(C<R>&& output, const C<T>& input, const std::function<const R&(const T&)>& f) {
//    for(const auto& i : input) { output.push_back(f(i)); } return output;
//}

template<typename T, typename Alloc, template <typename, typename> typename C>
inline C<T, Alloc>& filter_collection(C<T, Alloc>& output, const C<T, Alloc>& input, const std::function<bool(const T&)>& f) {
    for(const auto& i : input) {
        if(f(i)) {
            output.push_back(i);
        }
    } return output;
}

template<typename R, typename T, typename RAlloc, typename Alloc, template <typename, typename> typename C>
inline C<R, RAlloc>& filter_map_collection(C<R, RAlloc>& output, const C<T, Alloc>& input, const std::function<opt<R>(const T&)>& f) {
    for(const auto& i : input) {
        if(auto&& r = f(i)) {
            output.push_back(*r);
        }
    } return output;
}


template<typename T, typename Alloc, template <typename, typename> typename C>
inline std::pair<C<T, Alloc>, C<T, Alloc>>& divide_collection(std::pair<C<T, Alloc>, C<T, Alloc>>& output, const C<T, Alloc>& input, const std::function<bool(const T&)>& to_first) {
    for(const auto& i : input) {
        if(to_first(i)) {
            output.first.push_back(i);
        } else {
            output.second.push_back(i);
        }
    }
    return output;
}


//template<typename R, typename T, typename Alloc, template <typename, typename> typename C>
//inline C<const R&, Alloc>& filter_map_collection(C<R, Alloc>& output, const C<T, Alloc>& input, const std::function<const R&(const T&)>& f) {
//    for(const auto& i : input) {
//        if(auto&& r = f(i)) {
//            output.push_back(r);
//        }
//    } return output;
//}

enum strip_excess_format {
    StripNothing,
    StripExact,
    StripAndReplaceByDots
};

inline std::string align_left(const std::string& str, std::size_t size, strip_excess_format strip_excess = StripNothing) {
    if(str.size() < size) {
        return str + std::string(size - str.size(), ' ');
    } else if(str.size() == size) {
        return str;
    } else if(strip_excess == StripExact) {
        return str.substr(0, size);
    } else if(strip_excess == StripAndReplaceByDots) {
        auto substr = str.substr(0, size);
        const auto dots_num = std::min(size, std::size_t(3));
        return substr.replace(substr.size() - dots_num, dots_num, std::string(dots_num, '.'));
    } else {
        return str;
    }
}

inline std::string align_right(const std::string& str, std::size_t size, strip_excess_format strip_excess = StripNothing) {
    if(str.size() < size) {
        return std::string(size - str.size(), ' ') + str;
    } else if(str.size() == size) {
        return str;
    } else if(strip_excess == StripExact) {
        return str.substr(str.size() - size, size);
    } else if(strip_excess == StripAndReplaceByDots) {
        auto substr = str.substr(str.size() - size, size);
        const auto dots_num = std::min(size, std::size_t(3));
        return substr.replace(0, dots_num, std::string(dots_num, '.'));
    } else {
        return str;
    }
}

template<typename MutIt>
void align_left(MutIt begin, MutIt end, std::size_t min = 0, strip_excess_format strip_excess = StripNothing) {
    std::size_t max_size = min;
    for(auto it = begin; it != end; ++it) {
        max_size = std::max(max_size, it->size());
    }

    for(; begin != end; ++begin) {
        *begin = align_left(*begin, max_size, strip_excess);
    }
}

template<typename MutIt>
void align_right(MutIt begin, MutIt end, std::size_t min = 0, strip_excess_format strip_excess = StripNothing) {
    std::size_t max_size = min;
    for(auto it = begin; it != end; ++it) {
        max_size = std::max(max_size, it->size());
    }

    for(; begin != end; ++begin) {
        *begin = align_right(*begin, max_size, strip_excess);
    }
}

template<typename T, typename Alloc, template <typename, typename> typename C>
inline std::string join(const C<T, Alloc>& collection, const std::string& delim) {
    static_assert(std::is_same<T, std::string>::value, "T must be std::string");
    std::size_t i = 0;
    std::string res;
    for(const auto& s : collection) {
        res += s;
        if(i++ < collection.size() - 1) res += delim;
    }
    return res;
}

template<typename T, typename Alloc = std::allocator<T>>
class vec : public std::vector<T, Alloc> {
public:
    using std::vector<T, Alloc>::vector;

    vec(std::initializer_list<T> list, const Alloc& a = Alloc()) : std::vector<T, Alloc>(list, a) {}

    template<typename R, typename RAlloc = std::allocator<R>>
    constexpr inline vec<R, RAlloc> map(const std::function<R(const T&)>& f) const {
        vec<R, RAlloc> result; result.reserve(this->size()); return map_collection(result, *this, f);
    }


    template<typename R, typename RAlloc = std::allocator<R>, typename Arg>
    constexpr inline typename std::enable_if<std::is_class<T>::value, vec<R, RAlloc>>::type map_member(Arg f) const {
        vec<R, RAlloc> result; result.reserve(this->size()); return map_member_collection(result, *this, f);
    }

    template<typename R, typename RAlloc = std::allocator<R>, typename Arg>
    constexpr inline typename std::enable_if<std::is_class<T>::value, vec<R, RAlloc>>::type map_member_func(Arg f) const {
        vec<R, RAlloc> result; result.reserve(this->size()); return map_member_func_collection(result, *this, f);
    }



    // compilation error
    //template<typename R>
    //vec<const R&, Alloc> map(const std::function<const R&(const T&)> f) const {
    //    vec<R, Alloc> result; result.reserve(this->size()); return map_collection(result, *this, f);
    //}

    constexpr inline vec<T, Alloc> filter(const std::function<bool(const T&)>& f) const {
        vec<T, Alloc> result; result.reserve(this->size()); return filter_collection(result, *this, f);
    }

    template<typename R, typename RAlloc = std::allocator<R>>
    constexpr inline vec<R, RAlloc> filter_map(const std::function<opt<R>(const T&)>& f) const {
        vec<R, RAlloc> result; result.reserve(this->size()); return filter_map_collection(result, *this, f);
    }

    //template<typename R>
    //vec<const R&, Alloc> filter_map(const std::function<const R&(const T&)> f) const {
    //    vec<R, Alloc> result; result.reserve(this->size()); return filter_map_collection(result, *this, f);
    //}

    constexpr inline void append(const std::vector<T, Alloc>& tail) {
        this->insert(this->end(), tail.begin(), tail.end());
    }

    constexpr inline wall_e::opt<T> front_opt() const {
        if(this->empty()) {
            return std::nullopt;
        } else {
            return this->front();
        }
    }

    constexpr inline bool contains(const T& v) const {
        return std::find(this->begin(), this->end(), v) != this->end();
    }

    template<typename BinaryOperation>
    constexpr inline T reduce(T init, BinaryOperation op) {
        return std::accumulate(this->begin(), this->end(), init, op);
    }

    constexpr inline T max() {
        static_assert(std::is_integral<T>::value, "T must be integral");
        return reduce(std::numeric_limits<T>::min(), std::max);
    }
    constexpr inline T min() {
        static_assert(std::is_integral<T>::value, "T must be integral");
        return reduce(std::numeric_limits<T>::max(), std::min);
    }

    constexpr inline T sum() {
        static_assert(std::is_integral<T>::value, "T must be integral");
        return reduce(0, [](const auto& a, const auto& b){ return a + b; });
    }

    constexpr inline vec<T> aligned_left(std::size_t min = 0, strip_excess_format strip_excess = StripNothing) const {
        static_assert(std::is_same<T, std::string>::value, "T must be std::string");
        vec<T> copy = *this;
        wall_e::align_left(copy.begin(), copy.end(), min, strip_excess);
        return copy;
    }

    constexpr inline vec<T> aligned_right(std::size_t min = 0, strip_excess_format strip_excess = StripNothing) const {
        static_assert(std::is_same<T, std::string>::value, "T must be std::string");
        vec<T> copy = *this;
        wall_e::align_right(copy.begin(), copy.end(), min, strip_excess);
        return copy;
    }

    inline std::string join(const std::string& delim) const { return wall_e::join(*this, delim); }

    typedef std::pair<vec<T, Alloc>, vec<T, Alloc>> divide_result;

    inline divide_result divide(const std::function<bool(const T&)>& to_first) const {
        divide_result result;
        result.first.reserve(this->size());
        result.second.reserve(this->size());
        return divide_collection(result, *this, to_first);
    }

};


template<typename T, typename Alloc = std::allocator<T>>
class list : public std::list<T, Alloc> {
public:
    using std::list<T, Alloc>::list;

    list(std::initializer_list<T> l, const Alloc& a = Alloc()) : std::list<T, Alloc>(l, a) {}

    template<typename R, typename RAlloc = std::allocator<R>>
    inline list<R, RAlloc> map(const std::function<R(const T&)> f) const {
        list<R, RAlloc> result; return map_collection(result, *this, f);
    }
    //template<typename R>
    //list<const R&, Alloc> map(const std::function<const R&(const T&)> f) const {
    //    list<R, Alloc> result; return map_collection(result, *this, f);
    //}

    template<typename R, typename RAlloc = std::allocator<R>>
    inline list<R, RAlloc> filter_map(const std::function<opt<R>(const T&)> f) const {
        list<R, RAlloc> result; return filter_map_collection(result, *this, f);
    }
    //template<typename R>
    //list<const R&, Alloc> filter_map(const std::function<const R&(const T&)> f) const {
    //    list<R, Alloc> result; return filter_map_collection(result, *this, f);
    //}

    wall_e::opt<T> front_opt() const {
        if(this->empty()) {
            return std::nullopt;
        } else {
            return this->front();
        }
    }

    inline static list<T, Alloc> with_op(const list<T, Alloc>& l0, const list<T, Alloc>& l1) {
        return l0.with(l1);
    }

    inline list<T, Alloc> with(const list<T, Alloc>& other) const {
        auto copy = *this;
        copy.insert(copy.end(), other.begin(), other.end());
        return copy;
    }

    inline list<T, Alloc> with(const T& other) const {
        auto copy = *this;
        copy.push_back(other);
        return copy;
    }

    template<typename BinaryOperation>
    constexpr inline T reduce(T init, BinaryOperation op) {
        return std::accumulate(this->begin(), this->end(), init, op);
    }

    inline std::string join(const std::string& delim) const { return wall_e::join(*this, delim); }

    inline bool contains(const std::function<bool(const T& value)>& predicate) const {
        return std::find_if(this->begin(), this->end(), predicate) != this->end();
    }
    inline bool contains(const T& value) const {
        return std::find(this->begin(), this->end(), value) != this->end();
    }
};

template<typename T1, typename T2>
class pair : public std::pair<T1, T2> {
public:
    using std::pair<T1, T2>::pair;
};


template<
        typename Key,
        typename Tp,
        typename Compare = std::less<Key>,
        typename Alloc = std::allocator<std::pair<const Key, Tp>>
        >
class map : public std::map<Key, Tp, Compare, Alloc> {
public:
    using std::map<Key, Tp, Compare, Alloc>::map;

    inline opt<Tp> find_opt(const Key& k) const {
        const auto& it = this->find(k);
        if(it != this->end()) {
            return it->second;
        } else {
            return std::nullopt;
        }
    }

    inline wall_e::list<Key> keys() const {
        wall_e::list<Key> res;
        for(const auto& k : *this) {
            res.push_back(k.first);
        }
        return res;
    }

    inline const Tp& get_or(const Key& k, const Tp& def) const {
        const auto& it = this->find(k);
        if(it != this->end()) {
            return it->second;
        } else {
            return def;
        }
    }
};

template<typename T, typename Sequence = std::deque<T>>
class stack : public std::stack<T, Sequence> {
public:
    using std::stack<T, Sequence>::stack;

    inline friend std::ostream &operator<<(std::ostream &stream, const stack<T> &s) {
        if(s.empty()) return stream << "[]";
        int i = 0;
        stream << "[ ";
        for(const auto& v : s.c) {
            stream << v;
            if(i != s.size() - 1) {
                stream << ", ";
            }
            ++i;
        }
        stream << " ]";
        return stream;
    }
};

template<typename T, std::size_t S = 256>
struct buf {
    std::size_t m_size;
    T m_data[S];
public:
    using value_type                      = T;
    static constexpr std::size_t capacity = S;
    using iterator                        = T*;
    using const_iterator                  = const T*;

    buf() : m_size(0) {}

    std::size_t size() const { return m_size; }
    bool empty() const { return m_size == 0; }
    void push_back(const T& val) { insert_back(val); }

    const T& operator[](std::size_t i) const { return m_data[i]; }
    T& operator[](std::size_t i) { return m_data[i]; }

    const_iterator begin() const { return m_data; }
    const_iterator end() const { return ((T*)m_data) + m_size; }
    iterator begin() { return m_data; }
    iterator end() { return ((T*)m_data) + m_size; }

    iterator insert_back(const T& val) {
        m_data[m_size++] = val;
        return ((T*)m_data) + m_size - 1;
    }

    iterator find(const T& what) {
        auto it = begin();
        while(it != end()) {
            if(*it == what) return it;
            ++it;
        }
        return end();
    }
    template<typename F>
    iterator find(F(T::*f), const F& what) {
        auto it = begin();
        while(it != end()) {
            if((it->*f) == what) return it;
            ++it;
        }
        return end();
    }
    template<typename F>
    iterator find(F(T::*f), bool(*cmp)(const F, const F), const F& what) {
        auto it = begin();
        while(it != end()) {
            if(cmp((it->*f), what)) return it;
            ++it;
        }
        return end();
    }
};

template<
        typename Key,
        typename Tp,
        typename Compare = std::less<Key>,
        typename Alloc = std::allocator<std::pair<const Key, Tp>>
        >
inline std::ostream &operator<<(std::ostream &stream, const wall_e::map<Key, Tp, Compare, Alloc> &m) {
    if(m.empty()) return stream << "{}";
    int i = 0;
    stream << "{ ";
    for(const auto& v : m) {
        stream << v.first << " -> " << v.second;
        if(i != m.size() - 1) {
            stream << ", ";
        }
        ++i;
    }
    stream << " }";
    return stream;
}


template<typename T>
inline std::ostream &operator<<(std::ostream &stream, const list<T> &list) {
    if(list.empty()) return stream << "[]";
    int i = 0;
    stream << "[ ";
    for(const auto& l : list) {
        stream << l;
        if(i != list.size() - 1) {
            stream << ", ";
        }
        ++i;
    }
    stream << " ]";
    return stream;
}

template<typename T>
inline std::ostream &operator<<(std::ostream &stream, const vec<T> &vec) {
    if(vec.empty()) return stream << "[]";
    int i = 0;
    stream << "[ ";
    for(const auto& v : vec) {
        stream << v;
        if(i != vec.size() - 1) {
            stream << ", ";
        }
        ++i;
    }
    stream << " ]";
    return stream;
}

template<typename T>
inline std::ostream &operator<<(std::ostream &stream, const buf<T> &b) {
    if(b.empty()) return stream << "[]";
    int i = 0;
    stream << "[ ";
    for(const auto& v : b) {
        stream << v;
        if(i != b.size() - 1) {
            stream << ", ";
        }
        ++i;
    }
    stream << " ]";
    return stream;
}


template<typename A, typename B>
inline std::ostream &operator<<(std::ostream &stream, const pair<A, B> &pair) {
    return stream << "{ " << pair.first << ", " << pair.second << " }";
}

template<typename T>
inline std::ostream &operator<<(std::ostream &stream, const opt<T> &opt) {
    stream << "{ ";
    if(opt.has_value()) {
        stream << opt.value();
    } else {
        stream << "std::nullopt";
    }
    return stream << " }";
}

#ifdef QT_CORE_LIB
template<typename T>
inline QDebug& operator<<(QDebug& dbg, const opt<T>& opt) {
    dbg << "{ ";
    if(opt.has_value()) {
        dbg << opt.value();
    } else {
        dbg << "std::nullopt";
    }
    return dbg << " }";
}
#endif

template<typename T>
inline list<T> &&operator+(list<T> && l0, const list<T> &l1) {
    l0.insert(l0.end(), l1.begin(), l1.end());
    return std::move(l0);
}

/*
 * AMBIGOUS
template<typename T>
inline list<T> &&operator+(list<T> && l0, const T &v) {
    l0.push_back(v);
    return std::move(l0);
}

template<typename T>
inline list<T> operator+(list<T> l0, const list<T> &l1) {
    l0.insert(l0.end(), l1.begin(), l1.end());
    return l0;
}
*/

template<typename T>
inline list<T>&& operator+(list<T> &&l0, const T &v) {
    l0.push_back(v);
    return std::move(l0);
}


template<typename C, typename R, typename ...Args>
std::function<R(C, Args...)> to_static(R(C::*f)(Args...)) {
    return [f](C c, Args... args){ return (c->*f)(args...); };
}

class stream_writable_func {
    std::function<std::ostream&(std::ostream&)> m_f;
public:
    inline stream_writable_func(const std::function<std::ostream&(std::ostream&)>& f = nullptr) : m_f(f) {}
    inline std::ostream& operator()(std::ostream& s) const { return m_f(s); }
    friend inline std::ostream &operator<<(std::ostream &stream, const stream_writable_func &f) { return f(stream); }
    static stream_writable_func identity() { return stream_writable_func([](std::ostream& s) -> std::ostream& { return s; }); }
};

typedef wall_e::pair<std::string, std::string> str_pair;
typedef wall_e::vec<std::string> str_vec;
typedef wall_e::list<std::string> str_list;

}


#endif // COLLECTIONS_H
