#ifndef WALL_E_BOX_H
#define WALL_E_BOX_H

#include "collections.h"

namespace wall_e {

template<typename T>
class box {
    T* m_value = nullptr;
    box(T* value) : m_value(value) {}
public:
    box(const box& value) = delete;
    void operator=(const box& value) = delete;
    void operator=(box&& value) = delete;

    box(box&& value) : m_value(value.m_value) {}
    ~box() { if(m_value) delete m_value; }

    T* get() const { return m_value; };
    T* operator->() const { return m_value; };

    template<typename ...A>
    static box make(A&& ...args) { return box(new T(args...)); }

    static box from(const T& value) { return box(new T(value)); }
};

template<typename T> using opt_box = box<const opt<T>>;

template<typename T>
class box_list {
    wall_e::list<T*> m_data;
public:
    box_list() {}

    class factory {
        friend box_list;
        std::function<wall_e::list<T*>()> m_f;
        factory(const std::function<wall_e::list<T*>()>& f) : m_f(f) {}
    public:
        factory() {}
        template<typename ...TT>
        static factory make() {
            return factory([]{ return wall_e::list<T*> { new TT() ... }; });
        }
    };

    template<typename ...TT>
    box_list(const factory& f) : m_data(f.m_f ? f.m_f() : list<T*> {}) {}

    box_list(const box_list& value) = delete;
    void operator=(const box_list& value) = delete;
    void operator=(box_list&& value) = delete;

    std::size_t size() const { return m_data.size(); }

    box_list(box_list&& value) : m_data(value.m_data) {}
    ~box_list() { for(T* d : m_data) delete d; }

    typename std::list<T*>::const_iterator begin() const { return m_data.begin(); }
    typename std::list<T*>::const_iterator end() const { return m_data.end(); }

    template<typename ...A>
    T* make(A&& ...args) {
        m_data.push_back(new T(args...));
        return m_data.back();
    }

    friend inline std::ostream &operator<<(std::ostream &stream, const box_list &list) {
        return stream << list.m_data;
    }
};

}

#endif // WALL_E_BOX_H
