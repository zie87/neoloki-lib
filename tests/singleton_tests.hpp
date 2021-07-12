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

using t5 = singleton_holder<my_class<5>, create_using_new, default_lifetime, class_level_lockable>;
using t6 = singleton_holder<my_class<6>, create_using_new, phoenix_singleton, class_level_lockable>;
using t7 = singleton_holder<my_class<7>, create_using_new, singleton_with_longevity, class_level_lockable>;
using t8 = singleton_holder<my_class<8>, create_using_new, no_destroy, class_level_lockable>;

using t9  = singleton_holder<my_class<9>, create_using_malloc, default_lifetime, single_threaded>;
using t10 = singleton_holder<my_class<10>, create_using_malloc, phoenix_singleton, single_threaded>;
using t11 = singleton_holder<my_class<11>, create_using_malloc, singleton_with_longevity, single_threaded>;
using t12 = singleton_holder<my_class<12>, create_using_malloc, no_destroy, single_threaded>;

using t13 = singleton_holder<my_class<13>, create_using_malloc, default_lifetime, class_level_lockable>;
using t14 = singleton_holder<my_class<14>, create_using_malloc, phoenix_singleton, class_level_lockable>;
using t15 = singleton_holder<my_class<15>, create_using_malloc, singleton_with_longevity, class_level_lockable>;
using t16 = singleton_holder<my_class<16>, create_using_malloc, no_destroy, class_level_lockable>;

using t17 = singleton_holder<my_class<17>, create_static, default_lifetime, single_threaded>;
using t18 = singleton_holder<my_class<18>, create_static, phoenix_singleton, single_threaded>;
using t19 = singleton_holder<my_class<19>, create_static, singleton_with_longevity, single_threaded>;
using t20 = singleton_holder<my_class<20>, create_static, no_destroy, single_threaded>;

using t21 = singleton_holder<my_class<21>, create_static, default_lifetime, class_level_lockable>;
using t22 = singleton_holder<my_class<22>, create_static, phoenix_singleton, class_level_lockable>;
using t23 = singleton_holder<my_class<23>, create_static, singleton_with_longevity, class_level_lockable>;
using t24 = singleton_holder<my_class<24>, create_static, no_destroy, class_level_lockable>;

}  // namespace

struct singleton_test : test {
    singleton_test() : test("singleton.hpp") {}

    virtual void execute(test_result& results) override {
        {
            bool test_success = true;
            MAKE_TEST(t0);
            test::assertion("default config", test_success, results);
        }

        {
            bool test_success = true;
            MAKE_TEST(t1);
            MAKE_TEST(t2);
            MAKE_TEST(t3);
            MAKE_TEST(t4);
            test::assertion("new - single threaded", test_success, results);
        }
        {
            bool test_success = true;
            MAKE_TEST(t5);
            MAKE_TEST(t6);
            MAKE_TEST(t7);
            MAKE_TEST(t8);
            test::assertion("new - class level lock", test_success, results);
        }
        {
            bool test_success = true;
            MAKE_TEST(t9);
            MAKE_TEST(t10);
            MAKE_TEST(t11);
            MAKE_TEST(t12);
            test::assertion("malloc - single threaded", test_success, results);
        }
        {
            bool test_success = true;
            MAKE_TEST(t13);
            MAKE_TEST(t14);
            MAKE_TEST(t15);
            MAKE_TEST(t16);
            test::assertion("malloc - class level lock", test_success, results);
        }
        {
            bool test_success = true;
            MAKE_TEST(t17);
            MAKE_TEST(t18);
            MAKE_TEST(t19);
            MAKE_TEST(t20);
            test::assertion("static - single threaded", test_success, results);
        }
        {
            bool test_success = true;
            MAKE_TEST(t21);
            MAKE_TEST(t22);
            MAKE_TEST(t23);
            MAKE_TEST(t24);
            test::assertion("static - class level lock", test_success, results);
        }
    }

} singleton_test_instance;

}  // namespace loki

#endif  // NEOLOKI_SINGLETONTESTS_HPP
