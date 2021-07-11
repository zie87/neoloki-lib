#ifndef LOKINEO_TYPETRAITS_HPP
#define LOKINEO_TYPETRAITS_HPP

#include "null_type.hpp"
#include "type_list.hpp"
#include "type_manip.hpp"

namespace loki {

template <typename T>
struct is_custom_unsigned_int {
    static constexpr bool result = false;
};

template <typename T>
struct is_custom_signed_int {
    static constexpr bool result = false;
};

template <typename T>
struct is_custom_float {
    static constexpr bool result = false;
};

}  // namespace loki

namespace loki {

template <typename T>
struct is_pointer {
    static constexpr bool result = false;
    using pointee_type           = null_type;
};

template <typename T>
struct is_pointer<T*> {
    static constexpr bool result = true;
    using pointee_type           = T;
};

template <typename T>
struct is_reference {
    static constexpr bool result = false;
    using referred_type          = T;
};

template <typename T>
struct is_reference<T&> {
    static constexpr bool result = true;
    using referred_type          = T;
};

template <typename T>
struct un_const {
    static constexpr bool is_const = false;
    using type                     = T;
};

template <typename T>
struct un_const<const T> {
    static constexpr bool is_const = true;
    using type                     = T;
};

template <typename T>
struct un_volatile {
    static constexpr bool is_volatile = false;
    using type                        = T;
};

template <typename T>
struct un_volatile<volatile T> {
    static constexpr bool is_volatile = true;
    using type                        = T;
};

template <typename T>
struct is_member_pointer {
    static constexpr bool result = false;
};

template <typename T, typename U>
struct is_member_pointer<T U::*> {
    static constexpr bool result = true;
};

namespace detail {
using std_unsigned_integers = type_list<unsigned char, unsigned short, unsigned int, unsigned long>;
using std_signed_integers   = type_list<signed char, signed short, signed int, signed long>;
using std_other_integers    = type_list<bool, char, wchar_t>;
using std_floats            = type_list<float, double, long double>;

template <class T>
struct add_reference {
    using reference_type = T&;
};

template <class T>
struct add_reference<T&> {
    using reference_type = T&;
};

template <>
struct add_reference<void> {
    using reference_type = null_type;
};

}  // namespace detail

template <typename T>
struct is_std_unsigned_int {
    static constexpr bool result = index_of<detail::std_unsigned_integers, T>::value >= 0;
};

template <typename T>
struct is_std_signed_int {
    static constexpr bool result = index_of<detail::std_signed_integers, T>::value >= 0;
};

template <typename T>
struct is_std_integral {
    static constexpr bool result = is_std_unsigned_int<T>::result || is_std_signed_int<T>::result ||
                                   (index_of<detail::std_other_integers, T>::value >= 0);
};

template <typename T>
struct is_std_float {
    static constexpr bool result = index_of<detail::std_floats, T>::value >= 0;
};

template <typename T>
struct is_std_arithmetic {
    static constexpr bool result = is_std_integral<T>::result || is_std_float<T>::result;
};

template <typename T>
struct is_std_fundamental {
    static constexpr bool result = is_same<T, void>::result || is_std_arithmetic<T>::result;
};

template <typename T>
struct is_unsigned_int {
    static constexpr bool result = is_std_unsigned_int<T>::result || is_custom_unsigned_int<T>::result;
};

template <typename T>
struct is_signed_int {
    static constexpr bool result = is_std_signed_int<T>::result || is_custom_signed_int<T>::result;
};

template <typename T>
struct is_integral {
    static constexpr bool result = is_std_integral<T>::result || is_unsigned_int<T>::result || is_signed_int<T>::result;
};

template <typename T>
struct is_float {
    static constexpr bool result = is_std_float<T>::result || is_custom_float<T>::result;
};

template <typename T>
struct is_arithmetic {
    static constexpr bool result = is_integral<T>::result || is_float<T>::result;
};

template <typename T>
struct is_fundamental {
    static constexpr bool result = is_std_fundamental<T>::result || is_arithmetic<T>::result;
};

template <typename T>
struct as_parameter {
    using type =
        typename select<(is_std_arithmetic<T>::result || is_pointer<T>::result || is_member_pointer<T>::result), T,
                        typename detail::add_reference<T>::reference_type>::type;
};

}  // namespace loki

namespace loki {

template <typename T>
struct type_traits {
    static constexpr auto is_pointer = loki::is_pointer<T>::result;
    using pointee_type               = typename loki::is_pointer<T>::pointee_type;

    static constexpr auto is_reference = loki::is_reference<T>::result;
    using referred_type                = typename loki::is_reference<T>::referred_type;

    static constexpr auto is_const = loki::un_const<T>::is_const;
    using non_const_type           = typename loki::un_const<T>::type;

    static constexpr auto is_volatile = loki::un_volatile<T>::is_volatile;
    using non_volatile_type           = typename loki::un_volatile<T>::type;

    using unqualified_type = typename loki::un_volatile<typename loki::un_const<T>::type>::type;

    static constexpr auto is_member_pointer = loki::is_member_pointer<T>::result;

    static constexpr auto is_std_signed_int   = loki::is_std_signed_int<T>::result;
    static constexpr auto is_std_unsigned_int = loki::is_std_unsigned_int<T>::result;
    static constexpr auto is_std_integral     = loki::is_std_integral<T>::result;
    static constexpr auto is_std_float        = loki::is_std_float<T>::result;
    static constexpr auto is_std_arithmetic   = loki::is_std_arithmetic<T>::result;
    static constexpr auto is_std_fundamental  = loki::is_std_fundamental<T>::result;

    static constexpr auto is_signed_int   = loki::is_signed_int<T>::result;
    static constexpr auto is_unsigned_int = loki::is_unsigned_int<T>::result;
    static constexpr auto is_integral     = loki::is_integral<T>::result;
    static constexpr auto is_float        = loki::is_float<T>::result;
    static constexpr auto is_arithmetic   = loki::is_arithmetic<T>::result;
    static constexpr auto is_fundamental  = loki::is_fundamental<T>::result;

    using parameter_type = typename loki::as_parameter<T>::type;
};

}  // namespace loki

#endif  // LOKINEO_TYPETRAITS_HPP
