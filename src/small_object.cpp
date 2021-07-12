#include "loki/small_object.hpp"

#include <algorithm>
#include <cassert>
#include <limits>

namespace loki {

void fixed_allocator::chunk::init(size_type block_size, std::uint8_t blocks) {
    assert(block_size > 0);
    assert(blocks > 0);

    data = new uint8_t[block_size * blocks];
    reset(block_size, blocks);
}

void fixed_allocator::chunk::reset(size_type block_size, std::uint8_t blocks) noexcept {
    assert(block_size > 0);
    assert(blocks > 0);

    first_available_block = 0;
    blocks_available      = blocks;

    auto* ptr = data;
    for (std::uint8_t i = 0; i != blocks; ptr += block_size) {
        *ptr = ++i;
    }
}

void fixed_allocator::chunk::release() noexcept { delete[] data; }

void* fixed_allocator::chunk::allocate(size_type block_size) noexcept {
    if (blocks_available == 0) {
        return nullptr;
    }

    std::uint8_t* ptr     = data + (first_available_block * block_size);
    first_available_block = *ptr;
    --blocks_available;

    return ptr;
}

void fixed_allocator::chunk::deallocate(void* ptr, size_type block_size) noexcept {
    assert(ptr >= data);

    auto* to_release = static_cast<std::uint8_t*>(ptr);
    // alignment check
    assert(((to_release - data) % block_size) == 0);

    *to_release           = first_available_block;
    first_available_block = static_cast<std::uint8_t>((to_release - data) / block_size);

    assert(first_available_block == (to_release - data) / block_size);
    ++blocks_available;
}

fixed_allocator::fixed_allocator(size_type size) noexcept
    : m_block_size(size), m_alloc_chunk(nullptr), m_dealloc_chunk(nullptr) {
    assert(m_block_size > 0);

    m_prev = this;
    m_next = this;

    size_type num_blocks           = LOKI_DEFAULT_CHUNK_SIZE / m_block_size;
    constexpr size_type max_blocks = std::numeric_limits<std::uint8_t>::max();
    num_blocks                     = std::min(num_blocks, max_blocks);
    if (num_blocks == 0) {
        num_blocks = 8 * m_block_size;
    }

    m_num_blocks = static_cast<std::uint8_t>(num_blocks);
}

fixed_allocator::~fixed_allocator() {
    if (m_prev != this) {
        m_prev->m_next = m_next;
        m_next->m_prev = m_prev;
        return;
    }

    assert(m_prev == m_next);

    for (auto& chunk : m_chunks) {
        assert(chunk.blocks_available == m_num_blocks);
        chunk.release();
    }
}

fixed_allocator::fixed_allocator(const fixed_allocator& other) noexcept
    : m_block_size(other.m_block_size), m_num_blocks(other.m_num_blocks), m_chunks(other.m_chunks) {
    m_prev = &other;
    m_next = other.m_next;

    other.m_next->m_prev = this;
    other.m_next         = this;

    m_alloc_chunk = other.m_alloc_chunk ? &m_chunks.front() + (other.m_alloc_chunk - &other.m_chunks.front()) : nullptr;
    m_dealloc_chunk =
        other.m_dealloc_chunk ? &m_chunks.front() + (other.m_dealloc_chunk - &other.m_chunks.front()) : nullptr;
}

void fixed_allocator::swap(fixed_allocator& other) noexcept {
    using std::swap;
    swap(m_block_size, other.m_block_size);
    swap(m_num_blocks, other.m_num_blocks);
    swap(m_chunks, other.m_chunks);
    swap(m_alloc_chunk, other.m_alloc_chunk);
    swap(m_dealloc_chunk, other.m_dealloc_chunk);
}

void* fixed_allocator::allocate() {
    if (m_alloc_chunk == nullptr || m_alloc_chunk->blocks_available == 0) {
        for (auto it = m_chunks.begin();; ++it) {
            if (it == m_chunks.end()) {
                m_chunks.reserve(m_chunks.size() + 1);
                chunk new_chunk;
                new_chunk.init(m_block_size, m_num_blocks);
                m_chunks.emplace_back(new_chunk);
                m_alloc_chunk   = &m_chunks.back();
                m_dealloc_chunk = &m_chunks.front();
                break;
            }

            if (it->blocks_available > 0) {
                m_alloc_chunk = &*it;
                break;
            }
        }
    }

    assert(m_alloc_chunk != nullptr);
    assert(m_alloc_chunk->blocks_available > 0);

    return m_alloc_chunk->allocate(m_block_size);
}

void fixed_allocator::deallocate(void* ptr) noexcept {
    assert(!m_chunks.empty());
    assert(&m_chunks.front() <= m_dealloc_chunk);
    assert(&m_chunks.back() >= m_dealloc_chunk);

    m_dealloc_chunk = vicinity_find(ptr);
    assert(m_dealloc_chunk != nullptr);
    deallocate_impl(ptr);
}

auto fixed_allocator::vicinity_find(void* ptr) noexcept -> fixed_allocator::chunk* {
    assert(!m_chunks.empty());
    assert(m_dealloc_chunk != nullptr);

    const size_type chunk_len = m_num_blocks * m_block_size;
    chunk* low                = m_dealloc_chunk;
    chunk* high               = m_dealloc_chunk + 1;

    auto low_bound  = &m_chunks.front();
    auto high_bound = &m_chunks.back() + 1;

    if (high == high_bound) {
        high = nullptr;
    }

    while (true) {
        if (low) {
            if (ptr >= low->data && ptr <= (low->data + chunk_len)) {
                return low;
            }

            if (low == low_bound) {
                low = nullptr;
            } else {
                --low;
            }
        }

        if (high) {
            if (ptr >= high->data && ptr <= (high->data + chunk_len)) {
                return high;
            }
            if (++high == high_bound) {
                high = nullptr;
            }
        }
    }

    assert(false);
    return nullptr;
}

void fixed_allocator::deallocate_impl(void* ptr) noexcept {
    const size_type chunk_len = m_num_blocks * m_block_size;
    assert(m_dealloc_chunk->data <= ptr);
    assert(m_dealloc_chunk->data + chunk_len > ptr);

    m_dealloc_chunk->deallocate(ptr, m_block_size);

    if (m_dealloc_chunk->blocks_available != m_num_blocks) {
        return;
    }

    auto& last_chunk = m_chunks.back();

    if (&last_chunk == m_dealloc_chunk) {
        if (m_chunks.size() > 1 && (m_dealloc_chunk[-1].blocks_available == m_num_blocks)) {
            last_chunk.release();
            m_chunks.pop_back();
            m_alloc_chunk = m_dealloc_chunk = &m_chunks.front();
        }
        return;
    }

    if (last_chunk.blocks_available == m_num_blocks) {
        last_chunk.release();
        m_chunks.pop_back();
        m_alloc_chunk = m_dealloc_chunk;
        return;
    }

    using std::swap;
    swap(*m_dealloc_chunk, last_chunk);
    m_alloc_chunk = &m_chunks.back();
}

small_object_allocator::small_object_allocator(size_type chunk_size, size_type max_obj_size) noexcept
    : m_chunk_size(chunk_size), m_max_object_size(max_obj_size) {}

void* small_object_allocator::allocate(size_type num_bytes) {
    if (num_bytes > m_max_object_size) {
        return operator new(num_bytes);
    }

    if (m_last_alloc && m_last_alloc->block_size() == num_bytes) {
        return m_last_alloc->allocate();
    }

    auto comp = [](const fixed_allocator& x, size_type num_bytes) { return x.block_size() < num_bytes; };

    auto it = std::lower_bound(m_pool.begin(), m_pool.end(), num_bytes, comp);
    if ((it == m_pool.end()) || (it->block_size() != num_bytes)) {
        it             = m_pool.insert(it, fixed_allocator(num_bytes));
        m_last_dealloc = &*m_pool.begin();
    }

    m_last_alloc = &*it;
    return m_last_alloc->allocate();
}

void small_object_allocator::deallocate(void* ptr, size_type num_bytes) noexcept {
    if (num_bytes > m_max_object_size) {
        return operator delete(ptr);
    }

    if (m_last_dealloc && m_last_dealloc->block_size() == num_bytes) {
        m_last_dealloc->deallocate(ptr);
        return;
    }

    auto comp = [](const fixed_allocator& x, size_type num_bytes) { return x.block_size() < num_bytes; };

    auto it = std::lower_bound(m_pool.begin(), m_pool.end(), num_bytes, comp);
    assert(it != m_pool.end());
    assert(it->block_size() == num_bytes);

    m_last_dealloc = &*it;
    m_last_dealloc->deallocate(ptr);
}

}  // namespace loki
