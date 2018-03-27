#ifndef _CORE_COMMON_META_HPP_
#define _CORE_COMMON_META_HPP_

#include <common/types.hpp>

#include <cstdint>
#include <type_traits>

namespace ucle::meta {

    template <unsigned N>
    using sized_uint = typename std::conditional_t<(N <= 8),  byte_t,
                       typename std::conditional_t<(N <= 16), half_t,
                       typename std::conditional_t<(N <= 32), word_t,
                       /* else */                             dword_t
                        >>>;

    template <unsigned N>
    using sized_sint = typename std::conditional_t<(N <= 8),  sbyte_t,
                       typename std::conditional_t<(N <= 16), shalf_t,
                       typename std::conditional_t<(N <= 32), sword_t,
                       /* else */                             sdword_t
                        >>>;

    template <typename T>
    using is_storage_t = typename std::enable_if_t<std::is_same_v<T, byte_t> ||
                                                   std::is_same_v<T, half_t> ||
                                                   std::is_same_v<T, word_t> ||
                                                   std::is_same_v<T, dword_t> >;

}

#endif  /* _CORE_COMMON_META_HPP_ */
