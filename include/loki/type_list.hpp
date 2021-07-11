#ifndef LOKINEO_TYPELIST_HPP
#define LOKINEO_TYPELIST_HPP

#include <cstdint>

#include "null_type.hpp"
#include "type_manip.hpp"

namespace loki {

template <typename... TYPES>
struct type_list {};

//////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////

template <typename... TYPES>
struct size {};

template <typename... TYPES>
struct size<type_list<TYPES...>> {
    static constexpr std::size_t value = sizeof...(TYPES);
};

template <typename... TYPES>
using length = size<TYPES...>;

//////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////
template <class TList, std::size_t Idx>
struct type_at;

template <typename Head, typename... Tail>
struct type_at<type_list<Head, Tail...>, 0> {
    using type = Head;
};

template <typename Head, typename... Tail, std::size_t Idx>
struct type_at<type_list<Head, Tail...>, Idx> : type_at<type_list<Tail...>, Idx - 1> {};

//////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////
template <class TList, std::size_t Idx, typename DefaultType = null_type>
struct type_at_non_strict {
    using type = DefaultType;
};

template <typename Head, typename... Tail, typename DefaultType>
struct type_at_non_strict<type_list<Head, Tail...>, 0, DefaultType> {
    using type = Head;
};

template <typename Head, typename... Tail, std::size_t Idx, typename DefaultType>
struct type_at_non_strict<type_list<Head, Tail...>, Idx, DefaultType>
    : type_at_non_strict<type_list<Tail...>, Idx - 1, DefaultType> {};

//////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////
template <class TList, typename T>
struct index_of;

template <typename T>
struct index_of<type_list<>, T> {
    static constexpr int value = -1;
};

template <typename T, typename... Tail>
struct index_of<type_list<T, Tail...>, T> {
    static constexpr int value = 0;
};

template <typename Head, typename... Tail, typename T>
struct index_of<type_list<Head, Tail...>, T> {
private:
    static constexpr int temp = index_of<type_list<Tail...>, T>::value;

public:
    static constexpr int value = (temp == -1) ? -1 : 1 + temp;
};

//////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////
template <class TList, typename T>
struct append;

template <>
struct append<type_list<>, type_list<>> {
    using type = type_list<>;
};

template <typename T>
struct append<type_list<>, T> {
    using type = type_list<T>;
};

template <typename T>
struct append<type_list<>, type_list<T>> {
    using type = type_list<T>;
};

template <typename T>
struct append<type_list<T>, type_list<>> {
    using type = typename append<type_list<>, T>::type;
};

template <typename T, typename U>
struct append<type_list<T>, U> {
    using type = type_list<T, U>;
};

template <typename T, typename U>
struct append<type_list<T>, type_list<U>> {
    using type = type_list<T, U>;
};

template <typename... T, typename U>
struct append<type_list<T...>, U> {
    using type = type_list<T..., U>;
};

template <typename T, typename... U>
struct append<type_list<T>, type_list<U...>> {
    using type = type_list<T, U...>;
};

template <typename... T, typename... U>
struct append<type_list<T...>, type_list<U...>> {
    using type = type_list<T..., U...>;
};

//////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////
template <class TList, typename T>
struct erase;

template <typename T>
struct erase<type_list<>, T> {
    using type = type_list<>;
};

template <typename T>
struct erase<type_list<T>, T> {
    using type = type_list<>;
};

template <typename T, typename U>
struct erase<type_list<T>, U> {
    using type = type_list<T>;
};

template <typename T, typename... Tail>
struct erase<type_list<T, Tail...>, T> {
    using type = type_list<Tail...>;
};

template <typename Head, class... Tail, typename T>
struct erase<type_list<Head, Tail...>, T> {
    using type = typename append<type_list<Head>, typename erase<type_list<Tail...>, T>::type>::type;
};

//////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////

template <class TList, typename T>
struct erase_all;

template <typename T>
struct erase_all<type_list<>, T> {
    using type = type_list<>;
};

template <typename T>
struct erase_all<type_list<T>, T> {
    using type = type_list<>;
};

template <typename T, typename U>
struct erase_all<type_list<T>, U> {
    using type = type_list<T>;
};
template <typename T, typename... Tail>
struct erase_all<type_list<T, Tail...>, T> {
    using type = typename erase_all<type_list<Tail...>, T>::type;
};

template <typename Head, class... Tail, typename T>
struct erase_all<type_list<Head, Tail...>, T> {
    using type = typename append<type_list<Head>, typename erase_all<type_list<Tail...>, T>::type>::type;
};
//////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////
template <class TList>
struct no_duplicates;

template <>
struct no_duplicates<type_list<>> {
    using type = type_list<>;
};

template <typename T>
struct no_duplicates<type_list<T>> {
    using type = type_list<T>;
};

template <typename Head, typename... Tail>
struct no_duplicates<type_list<Head, Tail...>> {
private:
    using l1 = typename no_duplicates<type_list<Tail...>>::type;
    using l2 = typename erase<l1, Head>::type;

public:
    using type = typename append<type_list<Head>, l2>::type;
};
//////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////
template <class TList, typename T, typename U>
struct replace;

template <typename T, typename U>
struct replace<type_list<>, T, U> {
    using type = type_list<>;
};

template <typename T, typename U>
struct replace<type_list<T>, T, U> {
    using type = type_list<U>;
};

template <typename T, typename U, typename V>
struct replace<type_list<T>, U, V> {
    using type = type_list<T>;
};

template <typename Head, typename... Tail, typename T>
struct replace<type_list<Head, Tail...>, Head, T> {
    using type = type_list<T, Tail...>;
};

template <typename Head, typename... Tail, typename T, typename U>
struct replace<type_list<Head, Tail...>, T, U> {
    using type = typename append<type_list<Head>, typename replace<type_list<Tail...>, T, U>::type>::type;
};
//////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////

template <class TList, typename T, typename U>
struct replace_all;

template <typename T, typename U>
struct replace_all<type_list<>, T, U> {
    using type = type_list<>;
};

template <typename T, typename U>
struct replace_all<type_list<T>, T, U> {
    using type = type_list<U>;
};

template <typename T, typename U, typename V>
struct replace_all<type_list<T>, U, V> {
    using type = type_list<T>;
};

template <typename Head, typename... Tail, typename T>
struct replace_all<type_list<Head, Tail...>, Head, T> {
    using type = typename append<type_list<T>, typename replace_all<type_list<Tail...>, Head, T>::type>::type;
};

template <typename Head, typename... Tail, typename T, typename U>
struct replace_all<type_list<Head, Tail...>, T, U> {
    using type = typename append<type_list<Head>, typename replace_all<type_list<Tail...>, T, U>::type>::type;
};
//////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////

template <class TList>
struct reverse;

template <>
struct reverse<type_list<>> {
    using type = type_list<>;
};

template <typename T>
struct reverse<type_list<T>> {
    using type = type_list<T>;
};

template <typename T, typename U>
struct reverse<type_list<T, U>> {
    using type = type_list<U, T>;
};

template <typename Head, typename... Tail>
struct reverse<type_list<Head, Tail...>> {
public:
    using type = typename append<typename reverse<type_list<Tail...>>::type, Head>::type;
};
//////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////
template <class TList, class T>
struct most_derived;

template <class T>
struct most_derived<type_list<>, T> {
    using type = T;
};

template <typename Head, typename... Tail, typename T>
struct most_derived<type_list<Head, Tail...>, T> {
private:
    using candidate = typename most_derived<type_list<Tail...>, T>::type;

public:
    using type = typename select<is_super_subclass<candidate, Head>::result, Head, candidate>::type;
};

//////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////
template <class TList>
struct derived_to_front;

template <>
struct derived_to_front<type_list<>> {
    using type = type_list<>;
};

template <typename T>
struct derived_to_front<type_list<T>> {
    using type = type_list<T>;
};

template <typename Head, typename... Tail>
struct derived_to_front<type_list<Head, Tail...>> {
private:
    using the_most_derived = typename most_derived<type_list<Tail...>, Head>::type;
    using temp             = typename replace<type_list<Tail...>, the_most_derived, Head>::type;
    using new_tail         = typename derived_to_front<temp>::type;

public:
    using type = typename append<type_list<the_most_derived>, new_tail>::type;
};

}  // namespace loki

#endif  // LOKINEO_TYPELIST_HPP
