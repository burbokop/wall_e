#ifndef WALL_E_COLLECTIONS_H
#define WALL_E_COLLECTIONS_H

#include <vector>
#include <optional>
#include <ostream>
#include <list>
#include <functional>
#include <map>
#include <stack>

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

    template<typename R>
    inline opt<R> flatMap(const std::function<opt<R>(const T&)>& f) const {
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
};

template<typename R, typename T, typename RAlloc, typename Alloc, template <typename, typename> typename C>
inline C<R, RAlloc>& map_collection(C<R, RAlloc>& output, const C<T, Alloc>& input, const std::function<R(const T&)>& f) {
    for(const auto& i : input) { output.push_back(f(i)); } return output;
}

//template<typename R, typename T, template <typename> typename C, typename Alloc = std::allocator<T>>
//inline C<const R&>&& map_collection(C<R>&& output, const C<T>& input, const std::function<const R&(const T&)>& f) {
//    for(const auto& i : input) { output.push_back(f(i)); } return output;
//}

template<typename R, typename T, typename RAlloc, typename Alloc, template <typename, typename> typename C>
inline C<R, RAlloc>& filter_map_collection(C<R, RAlloc>& output, const C<T, Alloc>& input, const std::function<R(const T&)>& f) {
    for(const auto& i : input) {
        if(auto&& r = f(i)) {
            output.push_back(r);
        }
    } return output;
}

//template<typename R, typename T, typename Alloc, template <typename, typename> typename C>
//inline C<const R&, Alloc>& filter_map_collection(C<R, Alloc>& output, const C<T, Alloc>& input, const std::function<const R&(const T&)>& f) {
//    for(const auto& i : input) {
//        if(auto&& r = f(i)) {
//            output.push_back(r);
//        }
//    } return output;
//}

template<typename T, typename Alloc = std::allocator<T>>
class vec : public std::vector<T, Alloc> {
public:
    using std::vector<T, Alloc>::vector;

    template<typename R, typename RAlloc = std::allocator<R>>
    inline vec<R, RAlloc> map(const std::function<R(const T&)>& f) const {
        vec<R, RAlloc> result; result.reserve(this->size()); return map_collection(result, *this, f);
    }

    // compilation error
    //template<typename R>
    //vec<const R&, Alloc> map(const std::function<const R&(const T&)> f) const {
    //    vec<R, Alloc> result; result.reserve(this->size()); return map_collection(result, *this, f);
    //}

    template<typename R, typename RAlloc = std::allocator<R>>
    inline vec<R, RAlloc> filter_map(const std::function<opt<R>(const T&)>& f) const {
        vec<R, RAlloc> result; result.reserve(this->size()); return filter_map_collection(result, *this, f);
    }
    //template<typename R>
    //vec<const R&, Alloc> filter_map(const std::function<const R&(const T&)> f) const {
    //    vec<R, Alloc> result; result.reserve(this->size()); return filter_map_collection(result, *this, f);
    //}

    void append(const std::vector<T, Alloc>& tail) {
        this->insert(this->end(), tail.begin(), tail.end());
    }

    wall_e::opt<T> front_opt() const {
        if(this->empty()) {
            return std::nullopt;
        } else {
            return this->front();
        }
    }

    inline bool contains(const T& v) const {
        return std::find(this->begin(), this->end(), v) != this->end();
    }
};


template<typename T, typename Alloc = std::allocator<T>>
class list : public std::list<T, Alloc> {
public:
    using std::list<T, Alloc>::list;

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

template<typename T>
inline list<T> &&operator+(list<T> && l0, const list<T> &l1) {
    l0.insert(l0.end(), l1.begin(), l1.end());
    return std::move(l0);
}

template<typename C, typename R, typename ...Args>
std::function<R(C, Args...)> to_static(R(C::*f)(Args...)) {
    return [f](C c, Args... args){ return (c->*f)(args...); };
}

typedef wall_e::pair<std::string, std::string> str_pair;
typedef wall_e::vec<std::string> str_vec;
typedef wall_e::list<std::string> str_list;

}


#endif // COLLECTIONS_H
