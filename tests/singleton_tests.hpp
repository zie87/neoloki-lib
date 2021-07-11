#ifndef NEOLOKI_SINGLETONTESTS_HPP
#define NEOLOKI_SINGLETONTESTS_HPP

#include <loki/singleton.hpp>

#include "unit_test.hpp"

#define MAKE_TEST(name)                              \
    if (test_success && name::instance().a != 99) {  \
        test_success = false;                        \
    }                                                \
    ++name::instance().a;                            \
    if (test_success && name::instance().a != 100) { \
        test_success = false;                        \
    }

namespace loki {

template <int N>
struct my_class {
    my_class() noexcept = default;

    ~my_class() {
        assert(!was_destroyed);
        was_destroyed = true;
    }

    int a              = 99;
    bool was_destroyed = false;
};

inline std::size_t get_longevity(my_class<3>*) { return 6; }
inline std::size_t get_longevity(my_class<7>*) { return 5; }
inline std::size_t get_longevity(my_class<11>*) { return 4; }
inline std::size_t get_longevity(my_class<15>*) { return 1; }
inline std::size_t get_longevity(my_class<19>*) { return 2; }
inline std::size_t get_longevity(my_class<23>*) { return 3; }

namespace {
using t0 = singleton_holder<my_class<0> >;

using t1 = singleton_holder<my_class<1>, create_using_new, default_lifetime, single_threaded>;
using t2 = singleton_holder<my_class<2>, create_using_new, phoenix_singleton, single_threaded>;
using t3 = singleton_holder<my_class<3>, create_using_new, singleton_with_longevity, single_threaded>;
using t4 = singleton_holder<my_class<4>, create_using_new, no_destroy, single_threaded>;

}  // namespace

struct singleton_test : test {
    singleton_test() : test("singleton.hpp") {}

    virtual void execute(test_result& results) override {
        {
            bool test_success = true;
            MAKE_TEST(t0);
            test::assertion("default congig", test_success, results);
        }

        {
            bool test_success = true;
            MAKE_TEST(t1);
            MAKE_TEST(t2);
            MAKE_TEST(t3);
            MAKE_TEST(t4);
            test::assertion("new - single threaded", test_success, results);
        }
    }

} singleton_test_instance;

}  // namespace loki

#endif  // NEOLOKI_SINGLETONTESTS_HPP
