#ifndef WALL_E_EITHER_H
#define WALL_E_EITHER_H

#include <variant>
#include <functional>
#include <ostream>
#include "../utility/collections.h"

namespace wall_e {

template<typename L> class left;
template<typename R> class right;

template <typename L, typename R>
class either {
    template<typename T> friend class left;
    template<typename T> friend class right;

    std::variant<L, R> m_data;

    either(const std::variant<L, R>& var) : m_data(var) {}
public:
    typedef L left_type;
    typedef R right_type;

    static either<L, R> from_std_variant(const std::variant<L, R>& var) { return var; }

    operator bool () const { return m_data.index(); }
    bool defined() const { return m_data.index(); }

    /**
     * @brief left_value getter
     * @return left value
     * @note `unsafe`, better call ::left()
     */
    L left_value() const { return std::get<0>(m_data); }
    /**
     * @brief right_value getter
     * @return right value
     * @note `unsafe`, better call ::right()
     */
    R right_value() const { return std::get<1>(m_data); }

    inline auto left() const { return operator bool() ? wall_e::left<L>() : wall_e::left(left_value()); }
    inline auto right() const { return operator bool() ? wall_e::right(right_value()) : wall_e::right<R>(); }


    template<typename Result>
    Result fold(const std::function<Result(const L&)>& on_left, const std::function<Result(const R&)>& on_right) const {
        if(*this) {
            return on_right(right());
        } else {
            return on_left(left());
        }
    }

    void fold(const std::function<void(const L&)>& on_left, const std::function<void(const R&)>& on_right) const {
        if(*this) {
            on_right(right());
        } else {
            on_left(left());
        }
    }

    template<typename NR>
    either<L, NR> map(const std::function<NR(const R&)>& f) {
        if(*this) {
            return either<L, NR>::from_std_variant(std::variant<L, NR>(std::in_place_type<NR>, f(right_value())));
        }
        return either<L, NR>::from_std_variant(std::variant<L, NR>(std::in_place_type<L>, left_value()));
    }

    template<typename NR>
    either<L, NR> flat_map(const std::function<either<L, NR>(const R&)>& f) {
        if(*this) {
            return f(right());
        }
        return *this;
    }

    template<typename NL>
    either<NL, R> map_left(const std::function<NL(const L&)>& f) {
        if(!*this) {
            return f(left());
        }
        return *this;
    }

    template<typename NL>
    either<NL, R> flat_map_left(const std::function<either<NL, R>(const L&)>& f) {
        if(!*this) {
            return f(left());
        }
        return *this;
    }

    wall_e::opt<R> option() {
        if(*this) {
            return right_value();
        } else {
            return std::nullopt;
        }
    }

    R get_or(const R& default_value = {}) {
        if(*this) {
            return right_value();
        } else {
            return default_value;
        }
    }

    bool operator ==(const either<L, R>& other) const { return this->m_data == other.m_data; }
    bool operator <(const either<L, R>& other) const { return this->m_data < other.m_data; }
};


template<typename L>
class left {
    template<typename EL, typename ER> friend class either;
    wall_e::opt<L> m_value;
    left() {}
public:
    typedef L left_type;

    left(const L& value) { m_value = value; }

    template<typename R>
    operator either<L, R>() const { return std::variant<L, R>(std::in_place_type<L>, *m_value); }
    operator bool() const { return m_value.has_value(); }
    const L value() const { return *m_value; };

    bool operator ==(const left<L>& other) const { return this->m_value == other.m_value; }
};

template<typename R>
class right {
    template<typename EL, typename ER> friend class either;
    wall_e::opt<R> m_value;
    right() {}
public:
    typedef R right_type;

    right(const R& value) { m_value = value; }

    template<typename L>
    operator either<L, R>() const { return std::variant<L, R>(std::in_place_type<R>, *m_value); }
    operator bool() const { return m_value.has_value(); }
    const R value() const { return m_value.value(); };

    bool operator ==(const right<R>& other) const { return this->m_value == other.m_value; }
};

template<template<typename> typename C, typename L, typename R>
either<C<L>, C<R>> partition(const C<either<L, R>>& collection) {
    C<L> left;
    C<R> right;
    for(const auto& item : collection) {
        if(item) {
            right.push_back(item.right_value());
        } else {
            left.push_back(item.left_value());
        }
    }
    if(left.size() > 0) {
        return wall_e::left(left);
    } else {
        return wall_e::right(right);
    }
}

template<template<typename, typename> typename C, typename L, typename R>
either<C<L, std::allocator<L>>, C<R, std::allocator<R>>> partition(const C<either<L, R>, std::allocator<either<L, R>>>& collection) {
    C<L, std::allocator<L>> left;
    C<R, std::allocator<R>> right;
    for(const auto& item : collection) {
        if(item) {
            right.push_back(item.right_value());
        } else {
            left.push_back(item.left_value());
        }
    }
    if(left.size() > 0) {
        return wall_e::left(left);
    } else {
        return wall_e::right(right);
    }
}



template<typename L, typename R>
std::ostream &operator<<(std::ostream &stream, const wall_e::either<L, R>& e) {
    if(e) {
        return stream << "right(" << e.right_value() << ")";
    } else {
        return stream << "left(" << e.left_value() << ")";
    }
}
} // namespace wall_e


#endif // WALL_E_EITHER_H
