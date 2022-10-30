#ifndef WALL_E_COLLECTIONS_H
#define WALL_E_COLLECTIONS_H

#include <vector>
#include <optional>
#include <ostream>
#include <list>
#include <functional>

namespace wall_e {

template<typename T>
class opt : public std::optional<T> {
public:
    using std::optional<T>::optional;
};

template<typename R, typename T, typename Alloc, template <typename, typename> typename C>
inline C<R, Alloc>& map_collection(C<R, Alloc>& output, const C<T, Alloc>& input, const std::function<R(const T&)>& f) {
    for(const auto& i : input) { output.push_back(f(i)); } return output;
}

//template<typename R, typename T, template <typename> typename C, typename Alloc = std::allocator<T>>
//inline C<const R&>&& map_collection(C<R>&& output, const C<T>& input, const std::function<const R&(const T&)>& f) {
//    for(const auto& i : input) { output.push_back(f(i)); } return output;
//}

template<typename R, typename T, typename Alloc, template <typename, typename> typename C>
inline C<R, Alloc>& filter_map_collection(C<R, Alloc>& output, const C<T, Alloc>& input, const std::function<R(const T&)>& f) {
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

    template<typename R>
    inline vec<R, Alloc> map(const std::function<R(const T&)> f) const {
        vec<R, Alloc> result; result.reserve(this->size()); return map_collection(result, *this, f);
    }

    // compilation error
    //template<typename R>
    //vec<const R&, Alloc> map(const std::function<const R&(const T&)> f) const {
    //    vec<R, Alloc> result; result.reserve(this->size()); return map_collection(result, *this, f);
    //}

    template<typename R>
    inline vec<R, Alloc> filter_map(const std::function<opt<R>(const T&)> f) const {
        vec<R, Alloc> result; result.reserve(this->size()); return filter_map_collection(result, *this, f);
    }
    //template<typename R>
    //vec<const R&, Alloc> filter_map(const std::function<const R&(const T&)> f) const {
    //    vec<R, Alloc> result; result.reserve(this->size()); return filter_map_collection(result, *this, f);
    //}
};


template<typename T, typename Alloc = std::allocator<T>>
class list : public std::list<T, Alloc> {
public:
    using std::list<T, Alloc>::list;

    template<typename R>
    inline list<R, Alloc> map(const std::function<R(const T&)> f) const {
        list<R, Alloc> result; return map_collection(result, *this, f);
    }
    //template<typename R>
    //list<const R&, Alloc> map(const std::function<const R&(const T&)> f) const {
    //    list<R, Alloc> result; return map_collection(result, *this, f);
    //}

    template<typename R>
    inline list<R, Alloc> filter_map(const std::function<opt<R>(const T&)> f) const {
        list<R, Alloc> result; return filter_map_collection(result, *this, f);
    }
    //template<typename R>
    //list<const R&, Alloc> filter_map(const std::function<const R&(const T&)> f) const {
    //    list<R, Alloc> result; return filter_map_collection(result, *this, f);
    //}
};

template<typename T1, typename T2>
class pair : public std::pair<T1, T2> {
public:
    using std::pair<T1, T2>::pair;
};

template<typename T>
inline std::ostream &operator<<(std::ostream &stream, const list<T> &list) {
    int i = 0;
    stream << "{ ";
    for(const auto& l : list) {
        stream << l;
        if(i != list.size() - 1) {
            stream << ", ";
        }
        ++i;
    }
    stream << " }";
    return stream;
}

template<typename T>
inline std::ostream &operator<<(std::ostream &stream, const vec<T> &vec) {
    int i = 0;
    stream << "{ ";
    for(const auto& v : vec) {
        stream << v;
        if(i != vec.size() - 1) {
            stream << ", ";
        }
        ++i;
    }
    stream << " }";
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

}


#endif // COLLECTIONS_H
