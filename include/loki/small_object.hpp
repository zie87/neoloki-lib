#ifndef NEOLOKI_SMALLOBJECT_HPP
#define NEOLOKI_SMALLOBJECT_HPP

#include <cstdint>
#include <vector>

#include "singleton.hpp"
#include "threads.hpp"

#ifndef LOKI_DEFAULT_CHUNK_SIZE
#define LOKI_DEFAULT_CHUNK_SIZE 4098
#endif

#ifndef LOKI_DEFAULT_MAX_SMALL_OBJECT_SIZE
#define LOKI_DEFAULT_MAX_SMALL_OBJECT_SIZE 64
#endif

namespace loki {

class fixed_allocator {
public:
    using size_type = std::size_t;

    explicit fixed_allocator(size_type size) noexcept;
    ~fixed_allocator();

    fixed_allocator(const fixed_allocator& other) noexcept;
    fixed_allocator& operator=(const fixed_allocator& other) noexcept {
        fixed_allocator(other).swap(*this);
        return *this;
    }

    [[nodiscard]] void* allocate();
    void deallocate(void* ptr) noexcept;

    [[nodiscard]] inline size_type block_size() const noexcept { return m_block_size; }

    void swap(fixed_allocator& other) noexcept;

private:
    struct chunk {
        void init(size_type block_size, std::uint8_t blocks);
        void reset(size_type block_size, std::uint8_t blocks) noexcept;
        void release() noexcept;

        [[nodiscard]] void* allocate(size_type block_size) noexcept;
        void deallocate(void* ptr, size_type block_size) noexcept;

        std::uint8_t* data;
        std::uint8_t first_available_block;
        std::uint8_t blocks_available;
    };
    using chunks_type = std::vector<chunk>;

    void deallocate_impl(void* ptr) noexcept;
    [[nodiscard]] chunk* vicinity_find(void* ptr) noexcept;

    size_type m_block_size    = 0U;
    std::uint8_t m_num_blocks = 0U;
    chunk* m_alloc_chunk      = nullptr;
    chunk* m_dealloc_chunk    = nullptr;

    chunks_type m_chunks{};
    mutable const fixed_allocator* m_prev = nullptr;
    mutable const fixed_allocator* m_next = nullptr;
};

inline void swap(fixed_allocator& lhs, fixed_allocator& rhs) noexcept { lhs.swap(rhs); }

class small_object_allocator {
public:
    using size_type = std::size_t;

    small_object_allocator(size_type chunk_size, size_type max_obj_size) noexcept;
    small_object_allocator(const small_object_allocator&) = delete;
    small_object_allocator& operator=(const small_object_allocator&) = delete;

    [[nodiscard]] void* allocate(size_type num_bytes);
    void deallocate(void* ptr, size_type size) noexcept;

private:
    using pool_type = std::vector<fixed_allocator>;

    const size_type m_chunk_size;
    const size_type m_max_object_size;

    pool_type m_pool{};
    fixed_allocator* m_last_alloc   = nullptr;
    fixed_allocator* m_last_dealloc = nullptr;
};

template <template <class> class ThreadingModel = LOKI_DEFAULT_THREADING,
          std::size_t ChunkSize                 = LOKI_DEFAULT_CHUNK_SIZE,
          std::size_t MaxObjectSize             = LOKI_DEFAULT_MAX_SMALL_OBJECT_SIZE>
class small_object : public ThreadingModel<small_object<ThreadingModel, ChunkSize, MaxObjectSize>> {
    using threading_model_type = ThreadingModel<small_object<ThreadingModel, ChunkSize, MaxObjectSize>>;
    struct my_allocator : public small_object_allocator {
        my_allocator() noexcept : small_object_allocator(ChunkSize, MaxObjectSize) {}
    };

    using allocator_singleton = singleton_holder<my_allocator, create_static, phoenix_singleton>;

public:
    virtual ~small_object() = default;

    static void* operator new(std::size_t size) {
        typename threading_model_type::lock lock_guard;
        return allocator_singleton::instance().allocate(size);
    }

    static void operator delete(void* ptr, std::size_t size) noexcept {
        typename threading_model_type::lock lock_guard;
        return allocator_singleton::instance().deallocate(ptr, size);
    }
};

}  // namespace loki

#endif  // NEOLOKI_SMALLOBJECT_HPP
