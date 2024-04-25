#pragma once
#include <type_traits>

namespace SvCore::Utility
{
    template <typename T, typename First, typename... Remainder>
    constexpr bool IsOneOf = std::is_same_v<T, First> || IsOneOf<T, Remainder...>;

    template <typename T, typename Other>
    constexpr bool IsOneOf<T, Other> = std::is_same_v<T, Other>;

    template <typename T, typename... Remainder>
    constexpr bool HasDuplicates = IsOneOf<T, Remainder...> || HasDuplicates<Remainder...>;

    template <typename T>
    constexpr bool HasDuplicates<T> = false;

    template <typename T, typename... Remainder>
    constexpr bool IsAllConst = std::is_const_v<T> && IsAllConst<Remainder...>;

    template <typename T>
    constexpr bool IsAllConst<T> = std::is_const_v<T>;

    template <typename T, typename First, typename... Remainder>
    constexpr size_t IndexOf = std::is_same_v<T, First> ? 0 : IndexOf<T, Remainder...> + 1;

    template <typename T, typename First>
    constexpr size_t IndexOf<T, First> = std::is_same_v<T, First> ? 0 : 1;

    template <typename ...Args>
    constexpr bool IsEmpty = true;

    template <typename ...Args>
    constexpr bool IsEmpty<Args...> = sizeof...(Args) == 0;
}
