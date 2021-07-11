#ifndef LOKINEO_TYPEMANIP_HPP
#define LOKINEO_TYPEMANIP_HPP

namespace loki {

template <typename T, T Val>
struct value_to_type {
    static constexpr T value = Val;
};

template <int Val>
using integer_to_type = value_to_type<int, Val>;

template <typename T>
struct type_to_type {
    using type = T;
};

template <typename T, typename U>
struct is_same {
    static constexpr bool result = false;
};

template <typename T>
struct is_same<T, T> {
    static constexpr bool result = true;
};

}  // namespace loki

namespace loki {
template <bool flag, typename T, typename U>
struct select {
    using type = T;
};

template <typename T, typename U>
struct select<false, T, U> {
    using type = U;
};
}  // namespace loki

namespace loki {

namespace detail {
template <typename T, typename U>
struct conversion_helper {
    using small_type = char;
    struct big_type {
        char dummy[2];
    };

    static big_type test(...);
    static small_type test(U);
    static T make_type();
};
}  // namespace detail

template <typename T, typename U>
struct conversion {
    using helper                 = detail::conversion_helper<T, U>;
    static constexpr bool exists = (sizeof(typename helper::small_type) == sizeof(helper::test(helper::make_type())));
    static constexpr bool exists_both_ways = exists && conversion<U, T>::exists;
    static constexpr bool same_type        = false;
};

template <typename T>
struct conversion<T, T> {
    static constexpr bool exists           = true;
    static constexpr bool exists_both_ways = true;
    static constexpr bool same_type        = true;
};

template <typename T>
struct conversion<void, T> {
    static constexpr bool exists           = false;
    static constexpr bool exists_both_ways = false;
    static constexpr bool same_type        = false;
};

template <typename T>
struct conversion<T, void> {
    static constexpr bool exists           = false;
    static constexpr bool exists_both_ways = false;
    static constexpr bool same_type        = false;
};

template <>
struct conversion<void, void> {
    static constexpr bool exists           = true;
    static constexpr bool exists_both_ways = true;
    static constexpr bool same_type        = true;
};

template <class T, class U>
struct is_super_subclass {
    static constexpr bool result = (::loki::conversion<const volatile U*, const volatile T*>::exists &&
                                    !::loki::conversion<const volatile T*, const volatile void*>::same_type);
};

template <class T, class U>
struct is_super_subclass_strict {
    static constexpr bool result =
        is_super_subclass<T, U>::result && !is_same<const volatile T*, const volatile U*>::result;
};

}  // namespace loki

#endif  // LOKINEO_TYPEMANIP_HPP
