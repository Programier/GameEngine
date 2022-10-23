#pragma once

#include <type_traits>

namespace Engine
{
    template<typename T, typename _ = void>
    struct is_container : std::false_type {};

    template<typename... Ts>
    struct is_container_helper {};

    template<typename T>
    struct is_container<T,
                        std::conditional_t<false,
                                           is_container_helper<decltype(std::declval<T>().size()), decltype(std::declval<T>().begin()),
                                                               decltype(std::declval<T>().end())>,
                                           void>> : public std::true_type {};
}// namespace Engine
