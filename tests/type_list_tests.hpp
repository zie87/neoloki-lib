#ifndef LOKINEO_TYPELISTTESTS_HPP
#define LOKINEO_TYPELISTTESTS_HPP

#include <loki/type_list.hpp>
#include <loki/type_manip.hpp>

#include "unit_test.hpp"

namespace loki {

struct type_list_test : test {
    type_list_test() : test("type_list.hpp") {}

    virtual void execute(test_result& results) override {
        using empty_list                = loki::type_list<>;
        using char_list                 = loki::type_list<char>;
        using char_int_double_list      = loki::type_list<char, int, double>;
        using char_int_double_char_list = loki::type_list<char, int, double, char>;

        using base_derived1_derived2_list          = loki::type_list<base, derived1, derived2>;
        using derived2_derived1_base_list          = loki::type_list<derived2, derived1, base>;
        using base_derived1_base_derived2_list     = loki::type_list<base, derived1, base, derived2>;
        using derived1_base_derived1_derived2_list = loki::type_list<derived1, base, derived1, derived2>;

        {
            constexpr auto r = (loki::length<empty_list>::value == 0) && (loki::length<char_list>::value == 1) &&
                               (loki::length<char_int_double_list>::value == 3);
            test::assert("length", r, results);
        }
        {
            constexpr auto r = loki::is_same<loki::type_at<char_list, 0>::type, char>::result &&
                               loki::is_same<loki::type_at<char_int_double_char_list, 2>::type, double>::result;
            test::assert("type at", r, results);
        }
        {
            constexpr auto r =
                loki::is_same<loki::type_at_non_strict<empty_list, 0>::type, loki::null_type>::result &&
                loki::is_same<loki::type_at_non_strict<char_list, 0>::type, char>::result &&
                loki::is_same<loki::type_at_non_strict<char_int_double_list, 2>::type, double>::result &&
                loki::is_same<loki::type_at_non_strict<char_int_double_list, 3>::type, loki::null_type>::result &&
                loki::is_same<loki::type_at_non_strict<char_list, 1, long>::type, long>::result;
            test::assert("type at non strict", r, results);
        }
        {
            constexpr auto r = (loki::index_of<empty_list, char>::value == -1) &&
                               (loki::index_of<char_list, char>::value == 0) &&
                               (loki::index_of<char_int_double_list, double>::value == 2) &&
                               (loki::index_of<char_int_double_list, long>::value == -1);
            test::assert("index of", r, results);
        }
        {
            constexpr auto r = loki::is_same<loki::append<empty_list, empty_list>::type, empty_list>::result &&
                               loki::is_same<loki::append<empty_list, char>::type, loki::type_list<char>>::result &&
                               loki::is_same<loki::append<char_list, empty_list>::type, char_list>::result &&
                               loki::is_same<loki::append<empty_list, char_list>::type, char_list>::result &&
                               loki::is_same<loki::append<char_list, int>::type, loki::type_list<char, int>>::result &&
                               loki::is_same<loki::append<char_list, char_int_double_list>::type,
                                             loki::type_list<char, char, int, double>>::result &&
                               loki::is_same<loki::append<char_int_double_list, char_int_double_list>::type,
                                             loki::type_list<char, int, double, char, int, double>>::result;
            test::assert("append", r, results);
        }
        {
            constexpr auto r =
                loki::is_same<loki::erase<empty_list, char>::type, empty_list>::result &&
                loki::is_same<loki::erase<char_list, char>::type, empty_list>::result &&
                loki::is_same<loki::erase<char_list, long>::type, char_list>::result &&
                loki::is_same<loki::erase<char_int_double_list, char>::type, loki::type_list<int, double>>::result &&
                loki::is_same<loki::erase<char_int_double_list, int>::type, loki::type_list<char, double>>::result &&
                loki::is_same<loki::erase<char_int_double_list, double>::type, loki::type_list<char, int>>::result;
            test::assert("erase", r, results);
        }
        {
            constexpr auto r =
                loki::is_same<loki::erase_all<empty_list, char>::type, empty_list>::result &&
                loki::is_same<loki::erase_all<char_list, long>::type, char_list>::result &&
                loki::is_same<loki::erase_all<char_int_double_list, int>::type, type_list<char, double>>::result &&
                loki::is_same<loki::erase_all<char_int_double_list, double>::type, type_list<char, int>>::result &&
                loki::is_same<loki::erase_all<char_int_double_char_list, char>::type, type_list<int, double>>::result &&
                loki::is_same<loki::erase_all<char_int_double_char_list, int>::type,
                              loki::type_list<char, double, char>>::result &&
                loki::is_same<loki::erase_all<char_int_double_char_list, double>::type,
                              loki::type_list<char, int, char>>::result;
            test::assert("erase all", r, results);
        }
        {
            constexpr auto r =
                loki::is_same<loki::no_duplicates<empty_list>::type, empty_list>::result &&
                loki::is_same<loki::no_duplicates<char_list>::type, char_list>::result &&
                loki::is_same<loki::no_duplicates<char_int_double_list>::type, char_int_double_list>::result &&
                loki::is_same<loki::no_duplicates<char_int_double_char_list>::type, char_int_double_list>::result;
            test::assert("no duplicates", r, results);
        }
        {
            constexpr auto r =
                loki::is_same<loki::replace<empty_list, char, long>::type, empty_list>::result &&
                loki::is_same<loki::replace<char_list, char, long>::type, loki::type_list<long>>::result &&
                loki::is_same<loki::replace<char_list, int, long>::type, char_list>::result &&
                loki::is_same<loki::replace<char_int_double_list, char, long>::type,
                              loki::type_list<long, int, double>>::result &&
                loki::is_same<loki::replace<char_int_double_list, long, char[16]>::type,
                              char_int_double_list>::result &&
                loki::is_same<loki::replace<char_int_double_char_list, char, long>::type,
                              loki::type_list<long, int, double, char>>::result;

            test::assert("replace", r, results);
        }
        {
            constexpr auto r = loki::is_same<loki::replace_all<empty_list, char, long>::type, empty_list>::result &&
                               loki::is_same<loki::replace_all<char_list, char, long>::type, type_list<long>>::result &&
                               loki::is_same<loki::replace_all<char_list, int, long>::type, char_list>::result &&
                               loki::is_same<loki::replace_all<char_int_double_list, char, long>::type,
                                             loki::type_list<long, int, double>>::result &&
                               loki::is_same<loki::replace_all<char_int_double_list, long, char[16]>::type,
                                             char_int_double_list>::result &&
                               loki::is_same<loki::replace_all<char_int_double_char_list, char, long>::type,
                                             loki::type_list<long, int, double, long>>::result;
            test::assert("replace all", r, results);
        }
        {
            constexpr auto r =
                loki::is_same<loki::reverse<empty_list>::type, empty_list>::result &&
                loki::is_same<loki::reverse<char_list>::type, char_list>::result &&
                loki::is_same<loki::reverse<char_int_double_list>::type, loki::type_list<double, int, char>>::result &&
                loki::is_same<loki::reverse<loki::type_list<>>::type, loki::type_list<>>::result;
            test::assert("reverse", r, results);
        }
        {
            constexpr auto r =
                loki::is_same<loki::most_derived<empty_list, base>::type, base>::result &&
                loki::is_same<loki::most_derived<base_derived1_derived2_list, base>::type, derived2>::result &&
                loki::is_same<loki::most_derived<base_derived1_derived2_list, derived1>::type, derived2>::result &&
                loki::is_same<loki::most_derived<base_derived1_derived2_list, derived2>::type, derived2>::result &&
                loki::is_same<loki::most_derived<derived2_derived1_base_list, base>::type, derived2>::result &&
                loki::is_same<loki::most_derived<derived2_derived1_base_list, derived1>::type, derived2>::result &&
                loki::is_same<loki::most_derived<derived2_derived1_base_list, derived2>::type, derived2>::result &&
                loki::is_same<loki::most_derived<empty_list, base>::type, base>::result;
            test::assert("most derived", r, results);
        }
        {
            constexpr auto r =
                loki::is_same<loki::derived_to_front<empty_list>::type, empty_list>::result &&
                loki::is_same<loki::derived_to_front<char_list>::type, char_list>::result &&
                loki::is_same<loki::derived_to_front<char_int_double_list>::type, char_int_double_list>::result &&
                loki::is_same<loki::derived_to_front<char_int_double_char_list>::type,
                              char_int_double_char_list>::result &&
                loki::is_same<loki::derived_to_front<base_derived1_derived2_list>::type,
                              derived2_derived1_base_list>::result &&
                loki::is_same<loki::derived_to_front<derived2_derived1_base_list>::type,
                              derived2_derived1_base_list>::result &&
                loki::is_same<loki::derived_to_front<base_derived1_base_derived2_list>::type,
                              loki::type_list<derived2, derived1, base, base>>::result &&
                loki::is_same<loki::derived_to_front<derived1_base_derived1_derived2_list>::type,
                              loki::type_list<derived2, derived1, derived1, base>>::result;
            test::assert("derived to front", r, results);
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
} type_list_test_instance;

}  // namespace loki

#endif  // LOKINEO_TYPELISTTESTS_HPP
