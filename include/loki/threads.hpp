#ifndef NEOLOKI_THREADS_HPP
#define NEOLOKI_THREADS_HPP

#ifndef LOKI_DEFAULT_THREADING
#define LOKI_DEFAULT_THREADING ::loki::single_threaded
#endif

namespace loki {

template <class Host>
struct single_threaded {
    using volatile_type = Host;
    struct lock {};
};

}  // namespace loki

#include <mutex>

namespace loki {

template <class Host>
struct class_level_lockable {
    friend struct lock;

    using volatile_type = Host;
    struct lock {
        lock() { m_mutex.lock(); }
        explicit lock(const class_level_lockable&) { m_mutex.lock(); }
        ~lock() { m_mutex.unlock(); }

        lock(const lock&) = delete;
        lock& operator=(const lock&) = delete;
    };

private:
    static std::mutex m_mutex;
};

template <class Host>
std::mutex class_level_lockable<Host>::m_mutex{};

}  // namespace loki

#endif  // NEOLOKI_THREADS_HPP
