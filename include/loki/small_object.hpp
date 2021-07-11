#ifndef NEOLOKI_SMALLOBJECT_HPP
#define NEOLOKI_SMALLOBJECT_HPP

#include <cstdint>

#ifndef LOKI_DEFAULT_CHUNK_SIZE
#define LOKI_DEFAULT_CHUNK_SIZE 4098
#endif

#ifndef LOKI_DEFAULT_MAX_SMALL_OBJECT_SIZE
#define LOKI_DEFAULT_MAX_SMALL_OBJECT_SIZE 64
#endif

namespace loki {

class fixed_allocator {
private:
    struct chunk {};
};

template <std::size_t ChunkSize = LOKI_DEFAULT_CHUNK_SIZE, std::size_t MaxObjectSize = LOKI_DEFAULT_MAX_SMALL_OBJECT_SIZE>
class small_object {
    public:
        virtual ~small_object() = default;
};

}  // namespace loki

#endif  // NEOLOKI_SMALLOBJECT_HPP
