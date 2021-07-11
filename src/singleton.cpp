#include "loki/singleton.hpp"

#include <cassert>
#include <cstdlib>

namespace loki {
detail::tracker_array_type detail::tracker_array_ptr = nullptr;
std::size_t detail::elements                         = 0U;

void detail::atexit_handler() {
    assert((detail::elements > 0) && (detail::tracker_array_ptr != nullptr));
    // Pick the element at the top of the stack
    auto* top = tracker_array_ptr[detail::elements - 1];
    // Remove that object off the stack
    // Don't check errors - realloc with less memory
    //     can't fail
    tracker_array_ptr = static_cast<detail::tracker_array_type>(
        std::realloc(tracker_array_ptr, sizeof(*tracker_array_ptr) * --detail::elements));
    // Destroy the element
    delete top;
}

}  // namespace loki
