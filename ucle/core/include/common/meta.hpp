#ifndef _UCLE_CORE_COMMON_META_HPP_
#define _UCLE_CORE_COMMON_META_HPP_

#include <common/types.hpp>

#include <cstdint>
#include <type_traits>

namespace ucle::meta {

    template <unsigned N>
    using sized_uint_t = typename std::conditional_t<(N <= 8),  byte_t,
                         typename std::conditional_t<(N <= 16), half_t,
                         typename std::conditional_t<(N <= 32), word_t,
                         /* else */                             dword_t
                         >>>;

    template <unsigned N>
    using sized_sint_t = typename std::conditional_t<(N <= 8),  sbyte_t,
                         typename std::conditional_t<(N <= 16), shalf_t,
                         typename std::conditional_t<(N <= 32), sword_t,
                         /* else */                             sdword_t
                         >>>;

    template <unsigned N>
    using arch_address_t = typename std::conditional_t<(N <= 8),  address8_t,
                           typename std::conditional_t<(N <= 16), address16_t,
                           typename std::conditional_t<(N <= 32), address32_t,
                           /* else */                             address64_t
                           >>>;

    template <typename T>
    using is_storage_t = typename std::enable_if_t<std::is_same_v<T, byte_t> ||
                                                   std::is_same_v<T, half_t> ||
                                                   std::is_same_v<T, word_t> ||
                                                   std::is_same_v<T, dword_t> >;

    template <typename T>
    using is_storage_equiv_t = typename std::enable_if_t<std::is_convertible_v<T, byte_t> ||
                                                         std::is_convertible_v<T, half_t> ||
                                                         std::is_convertible_v<T, word_t> ||
                                                         std::is_convertible_v<T, dword_t> >;

    template <typename T>
    using is_integer_t = typename std::enable_if_t<std::is_same_v<T, int> ||
                                                   std::is_same_v<T, unsigned> ||
                                                   std::is_same_v<T, long> ||
                                                   std::is_same_v<T, unsigned long> ||
                                                   std::is_same_v<T, long long> ||
                                                   std::is_same_v<T, unsigned long long> >;

    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

}

#endif  /* _UCLE_CORE_COMMON_META_HPP_ */
