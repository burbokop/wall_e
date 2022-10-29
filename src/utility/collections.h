#ifndef WALL_E_COLLECTIONS_H
#define WALL_E_COLLECTIONS_H

#include <vector>
#include <optional>
#include <ostream>
#include <list>

namespace wall_e {

template<typename T, typename Alloc = std::allocator<T>>
class vec : public std::vector<T, Alloc> {
public:
    using std::vector<T, Alloc>::vector;
};

template<typename T, typename Alloc = std::allocator<T>>
class list : public std::list<T, Alloc> {
public:
    using std::list<T, Alloc>::list;
};

template<typename T1, typename T2>
class pair : public std::pair<T1, T2> {
public:
    using std::pair<T1, T2>::pair;
};

template<typename T>
class opt : public std::optional<T> {
public:
    using std::optional<T>::optional;
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
