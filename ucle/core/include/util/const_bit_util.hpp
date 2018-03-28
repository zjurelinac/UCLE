#ifndef _UCLE_CORE_UTIL_CONST_BIT_UTIL_HPP_
#define _UCLE_CORE_UTIL_CONST_BIT_UTIL_HPP_

#include <common/types.hpp>

#include <type_traits>

namespace ucle::util {

    template <typename T>
    struct const_bit_util {
        static constexpr auto bitsize() { return 8 * sizeof(T); }

        static constexpr auto highest_bit() { return static_cast<T>(1) << bitsize() - 1; }
        static constexpr auto nth_bit(size_t n) { return static_cast<T>(1) << n; }
        static constexpr auto all_bits() { return ~static_cast<T>(0); }
        static constexpr auto all_but_highest_bit() { return ~highest_bit(); }

        static constexpr auto rot_mask() { return bitsize() - 1; }
        static constexpr auto clear_topn_mask(size_t n) { return (static_cast<T>(1) << n) - 1; }
        static constexpr auto set_topn_mask(size_t n) { return ~clear_topn_mask(n); }

        static constexpr auto address_round_mask() { return ~(sizeof(T) - 1); }
    };

}

#endif  /* _UCLE_CORE_UTIL_CONST_BIT_UTIL_HPP_ */
