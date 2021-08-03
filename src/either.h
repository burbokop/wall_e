#ifndef WALL_E_EITHER_H
#define WALL_E_EITHER_H

#include <variant>
#include <functional>

namespace wall_e {

template<typename L> class left;
template<typename R> class right;

template <typename L, typename R>
class either {
    template<typename T> friend class left;
    template<typename T> friend class right;

    std::variant<L, R> m_data;
public:
    operator bool () const { return m_data.index(); }

    L left_value() const { return std::get<0>(m_data); }
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
            return f(right());
        }
        return *this;
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
};


template<typename L>
class left {
    template<typename EL, typename ER> friend class either;
    std::optional<L> m_value;
    left() {}
public:
    left(const L& value) { m_value = value; }

    template<typename R>
    operator either<L, R>() const {
        either<L, R> result;
        result.m_data = *m_value;
        return result;
    }

    operator bool() const { return m_value.has_value(); }

    const L value() const { return *m_value; };
};

template<typename R>
class right {
    template<typename EL, typename ER> friend class either;
    std::optional<R> m_value;
    right() {}
public:
    right(const R& value) { m_value = value; }

    template<typename L>
    operator either<L, R>() const {
        either<L, R> result;
        result.m_data = m_value.value();
        return result;
    }

    operator bool() const { return m_value.has_value(); }

    const R value() const { return m_value.value(); };
};

} // namespace wall_e

#endif // WALL_E_EITHER_H
