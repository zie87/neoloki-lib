#ifndef LOKINEO_TYPETRAITSTESTS_HPP
#define LOKINEO_TYPETRAITSTESTS_HPP

#include <loki/type_manip.hpp>
#include <loki/type_traits.hpp>

#include "unit_test.hpp"

namespace loki {

struct type_traits_test : test {
    type_traits_test() : test("type_traits.hpp") {}

    virtual void execute(test_result& results) override {
        {
            constexpr auto r = loki::type_traits<int*>::is_pointer && !loki::type_traits<int>::is_pointer &&
                               loki::is_same<loki::type_traits<int*>::pointee_type, int>::result &&
                               loki::is_same<loki::type_traits<int>::pointee_type, loki::null_type>::result;

            test::assert("pointer trait", r, results);
        }
        {
            constexpr auto r = loki::type_traits<int&>::is_reference && !loki::type_traits<int>::is_reference &&
                               loki::is_same<loki::type_traits<int&>::referred_type, int>::result &&
                               loki::is_same<loki::type_traits<int>::referred_type, int>::result;

            test::assert("reference trait", r, results);
        }
        {
            constexpr auto r = loki::type_traits<const int>::is_const && !loki::type_traits<int>::is_const &&
                               loki::is_same<loki::type_traits<const int>::non_const_type, int>::result &&
                               loki::is_same<loki::type_traits<int>::non_const_type, int>::result;

            test::assert("const trait", r, results);
        }
        {
            constexpr auto r = loki::type_traits<volatile int>::is_volatile && !loki::type_traits<int>::is_volatile &&
                               loki::is_same<loki::type_traits<volatile int>::non_volatile_type, int>::result &&
                               loki::is_same<loki::type_traits<int>::non_volatile_type, int>::result;

            test::assert("volatile trait", r, results);
        }
        {
            constexpr auto r = loki::is_same<loki::type_traits<const volatile int>::unqualified_type, int>::result;
            test::assert("unqualified type", r, results);
        }
        {
            constexpr auto r =
                loki::type_traits<int test::*>::is_member_pointer && !loki::type_traits<int>::is_member_pointer;
            test::assert("pointer to member", r, results);
        }
        {
            constexpr auto r =
                loki::type_traits<unsigned int>::is_std_unsigned_int && !loki::type_traits<int>::is_std_unsigned_int &&
                !loki::type_traits<unsigned int>::is_std_signed_int && loki::type_traits<int>::is_std_signed_int;
            test::assert("signedness (std)", r, results);
        }
        {
            constexpr auto r = loki::type_traits<unsigned int>::is_unsigned_int &&
                               !loki::type_traits<int>::is_unsigned_int &&
                               !loki::type_traits<unsigned int>::is_signed_int && loki::type_traits<int>::is_signed_int;
            test::assert("signedness", r, results);
        }
        {
            constexpr auto r = loki::type_traits<int>::is_std_integral && !loki::type_traits<double>::is_std_integral;
            test::assert("integral (std)", r, results);
        }
        {
            constexpr auto r = loki::type_traits<int>::is_integral && !loki::type_traits<double>::is_integral;
            test::assert("integral", r, results);
        }

        {
            constexpr auto r = loki::type_traits<double>::is_std_float && !loki::type_traits<int>::is_std_float;
            test::assert("float (std)", r, results);
        }

        {
            constexpr auto r = loki::type_traits<double>::is_float && !loki::type_traits<int>::is_float;
            test::assert("float", r, results);
        }
        {
            constexpr auto r = loki::type_traits<int>::is_std_arithmetic &&
                               loki::type_traits<double>::is_std_arithmetic &&
                               !loki::type_traits<void>::is_std_arithmetic;
            test::assert("arithmetic (std)", r, results);
        }
        {
            constexpr auto r = loki::type_traits<char>::is_arithmetic && loki::type_traits<char>::is_arithmetic &&
                               loki::type_traits<double>::is_arithmetic && !loki::type_traits<void>::is_arithmetic;
            test::assert("arithmetic", r, results);
        }
        {
            constexpr auto r =
                loki::type_traits<void>::is_std_fundamental && !loki::type_traits<test>::is_std_fundamental;
            test::assert("fundamental (std)", r, results);
        }
        {
            constexpr auto r = loki::type_traits<void>::is_fundamental && !loki::type_traits<test>::is_fundamental;
            test::assert("fundamental", r, results);
        }

        {
            constexpr auto r = loki::is_same<loki::type_traits<char>::parameter_type, char>::result &&
                               loki::is_same<loki::type_traits<int>::parameter_type, int>::result &&
                               loki::is_same<loki::type_traits<double>::parameter_type, double>::result &&
                               loki::is_same<loki::type_traits<test>::parameter_type, test&>::result &&
                               loki::is_same<loki::type_traits<test*>::parameter_type, test*>::result;
            test::assert("parameter type", r, results);
        }
    }
} type_traits_test_instance;

}  // namespace loki

#endif  // LOKINEO_TYPETRAITSTESTS_HPP
