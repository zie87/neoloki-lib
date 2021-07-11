#ifndef LOKINEO_TYPEMANIPTESTS_HPP
#define LOKINEO_TYPEMANIPTESTS_HPP

#include <loki/type_manip.hpp>

#include "unit_test.hpp"

namespace loki {

struct type_manip_test : test {
    type_manip_test() : test("type_manip.hpp") {}

    virtual void execute(test_result& results) override {
        test::assert("integer to type", (loki::integer_to_type<2>::value == 2), results);
        test::assert("type to type", (loki::is_same<loki::type_to_type<char>::type, char>::result), results);

        {
            auto r = loki::conversion<char, int>::exists && loki::conversion<char, int>::exists_both_ways &&
                     !loki::conversion<char, int>::same_type;
            r &= loki::conversion<char, char>::exists && loki::conversion<char, char>::exists_both_ways &&
                 loki::conversion<char, char>::same_type;
            r &= loki::conversion<void, void>::exists && !loki::conversion<void, char>::exists &&
                 !loki::conversion<char, void>::exists;
            r &= loki::conversion<derived2, base>::exists && loki::conversion<derived1, base>::exists &&
                 loki::conversion<base, base>::exists && !loki::conversion<base, derived2>::exists &&
                 !loki::conversion<base, derived1>::exists;
            r &= loki::conversion<derived2*, base*>::exists && loki::conversion<derived1*, base*>::exists &&
                 !loki::conversion<base*, derived2*>::exists && !loki::conversion<base*, derived1*>::exists &&
                 loki::conversion<base*, void*>::exists && !loki::conversion<void*, base*>::exists;
            test::assert("conversion", r, results);
        }

        {
            constexpr auto r = loki::is_same<loki::select<true, char, int>::type, char>::result &&
                               loki::is_same<loki::select<false, char, int>::type, int>::result;
            test::assert("select", r, results);
        }

        {
            constexpr auto r = is_super_subclass<base, derived1>::result && is_super_subclass<base, derived2>::result &&
                               is_super_subclass<base, base>::result && !is_super_subclass<derived1, base>::result &&
                               !is_super_subclass<derived2, base>::result && !is_super_subclass<void, base>::result;

            test::assert("super subclass", r, results);
        }

        {
            constexpr auto r =
                is_super_subclass_strict<base, derived1>::result && is_super_subclass_strict<base, derived2>::result &&
                !is_super_subclass_strict<base, base>::result && !is_super_subclass_strict<derived1, base>::result &&
                !is_super_subclass_strict<derived2, base>::result && !is_super_subclass_strict<void, base>::result;

            test::assert("super subclass strict", r, results);
        }
    }

private:
    struct base {
        char c;
    };
    struct derived1 : base {
        char c;
    };
    struct derived2 : derived1 {
        char c;
    };
} type_manip_test_instance;

}  // namespace loki

#endif  // LOKINEO_TYPEMANIPTESTS_HPP
