#ifndef NEOLOKI_THREADS_HPP
#define NEOLOKI_THREADS_HPP

namespace loki {

template <class Host>
struct single_threaded {
    using volatile_type = Host;
    struct lock {};
};
}  // namespace loki

#endif  // NEOLOKI_THREADS_HPP
