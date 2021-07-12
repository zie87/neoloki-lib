#ifndef NEOLOKI_SINGLETON_HPP
#define NEOLOKI_SINGLETON_HPP

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>

#include "threads.hpp"

namespace loki {

namespace detail {
class lifetime_tracker {
public:
    explicit lifetime_tracker(std::size_t longevity) : m_longevity(longevity) {}
    virtual ~lifetime_tracker() = default;

    static bool compare(const lifetime_tracker* lhs, const lifetime_tracker* rhs) noexcept {
        return lhs->m_longevity < rhs->m_longevity;
    }

private:
    std::size_t m_longevity;
};

using tracker_array_type = lifetime_tracker**;
extern tracker_array_type tracker_array_ptr;
extern std::size_t elements;

template <typename T>
struct deleter {
    static void do_delete(T* obj) { delete obj; }
};

template <typename T, typename Destroyer>
class concrete_lifetime_tracker final : public lifetime_tracker {
public:
    concrete_lifetime_tracker(T* ptr, std::size_t longevity, Destroyer destroyer) noexcept
        : lifetime_tracker(longevity), m_tracked_obj(ptr), m_destroyer(destroyer) {}
    ~concrete_lifetime_tracker() noexcept { m_destroyer(m_tracked_obj); }

private:
    T* m_tracked_obj;
    Destroyer m_destroyer;
};

void atexit_handler();

}  // namespace detail

typedef void (*atexit_handler_type)();

template <typename T, typename Destroyer>
void set_longevity(T* dyn_object, std::size_t longevity, Destroyer d = detail::deleter<T>::do_delete) {
    auto new_array = static_cast<detail::tracker_array_type>(
        std::realloc(detail::tracker_array_ptr, sizeof(*detail::tracker_array_ptr) * (detail::elements + 1)));

    if (new_array == nullptr) {
        throw std::bad_alloc();
    }

    detail::tracker_array_ptr     = new_array;
    detail::lifetime_tracker* ptr = new detail::concrete_lifetime_tracker<T, Destroyer>(dyn_object, longevity, d);

    auto pos = std::upper_bound(detail::tracker_array_ptr, detail::tracker_array_ptr + detail::elements, ptr,
                                detail::lifetime_tracker::compare);
    std::copy_backward(pos, detail::tracker_array_ptr + detail::elements,
                       detail::tracker_array_ptr + detail::elements + 1);
    *pos = ptr;
    ++detail::elements;

    std::atexit(detail::atexit_handler);
}

template <typename T>
struct create_using_new {
    static T* create() { return new T; }
    static void destroy(T* ptr) { delete ptr; }
};

template <typename T>
struct create_using_malloc {
    static T* create() noexcept {
        void* ptr = std::malloc(sizeof(T));
        if (ptr == nullptr) {
            return nullptr;
        }
        return new (ptr) T;
    }

    static void destroy(T* ptr) noexcept {
        ptr->~T();
        std::free(ptr);
    }
};

template <class T>
struct create_static {
    union max_align {
        char m_type_storage[sizeof(T)];
        std::max_align_t m_max_align;
    };

    static T* create() {
        static max_align static_storage;
        return new (&static_storage) T;
    }

    static void destroy(T* ptr) { ptr->~T(); }
};

template <typename T>
struct default_lifetime {
    static void schedule_destruction(T*, atexit_handler_type fun) { std::atexit(fun); }
    static void on_dead_reference() { throw std::logic_error("dead reference detected"); }
};

template <typename T>
class phoenix_singleton {
public:
    static void schedule_destruction(T*, atexit_handler_type fun) {
        if (!m_destroyed_once) std::atexit(fun);
    }

    static void on_dead_reference() { m_destroyed_once = true; }

private:
    static bool m_destroyed_once;
};

template <typename T>
bool phoenix_singleton<T>::m_destroyed_once = false;

namespace detail {
template <typename T>
struct adapter {
    auto operator()(T*) { return func(); }
    atexit_handler_type func;
};
}  // namespace detail

template <class T>
class singleton_with_longevity {
public:
    static void schedule_destruction(T* obj, atexit_handler_type fun) {
        detail::adapter<T> adapter = {fun};
        set_longevity(obj, get_longevity(obj), adapter);
    }

    static void on_dead_reference() { throw std::logic_error("dead reference detected"); }
};

template <class T>
struct no_destroy {
    static void schedule_destruction(T*, atexit_handler_type) {}
    static void on_dead_reference() {}
};

template <typename T, template <class> class CreationPolicy = create_using_new,
          template <class> class LifetimePolicy = default_lifetime,
          template <class> class ThreadingModel = single_threaded>
class singleton_holder {
public:
    static T& instance();

private:
    static void make_instance();
    static void destroy_singleton();

    singleton_holder();

    using instance_ptr_type = typename ThreadingModel<T*>::volatile_type;
    static instance_ptr_type m_instance;
    static bool m_destroyed;
};

template <class T, template <class> class C, template <class> class L, template <class> class M>
typename singleton_holder<T, C, L, M>::instance_ptr_type singleton_holder<T, C, L, M>::m_instance;

template <class T, template <class> class C, template <class> class L, template <class> class M>
bool singleton_holder<T, C, L, M>::m_destroyed = false;

template <class T, template <class> class CreationPolicy, template <class> class LifetimePolicy,
          template <class> class ThreadingModel>
inline T& singleton_holder<T, CreationPolicy, LifetimePolicy, ThreadingModel>::instance() {
    if (!m_instance) {
        make_instance();
    }
    return *m_instance;
}

template <class T, template <class> class CreationPolicy, template <class> class LifetimePolicy,
          template <class> class ThreadingModel>
void singleton_holder<T, CreationPolicy, LifetimePolicy, ThreadingModel>::make_instance() {
    typename ThreadingModel<T>::lock guard;

    if (!m_instance) {
        if (m_destroyed) {
            LifetimePolicy<T>::on_dead_reference();
            m_destroyed = false;
        }
        m_instance = CreationPolicy<T>::create();
        LifetimePolicy<T>::schedule_destruction(m_instance, &destroy_singleton);
    }
}

template <class T, template <class> class CreationPolicy, template <class> class L, template <class> class M>
void singleton_holder<T, CreationPolicy, L, M>::destroy_singleton() {
    assert(!m_destroyed);
    CreationPolicy<T>::destroy(m_instance);
    m_instance  = nullptr;
    m_destroyed = true;
}
}  // namespace loki

#endif  // NEOLOKI_SINGLETON_HPP
