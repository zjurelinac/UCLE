#ifndef _UCLE_CORE_UTIL_CONST_BIN_UTIL_HPP_
#define _UCLE_CORE_UTIL_CONST_BIN_UTIL_HPP_

#include <common/types.hpp>

#include <algorithm>
#include <type_traits>

namespace ucle::util {

    template <typename T>
    struct const_bin_util {
        // Operations without an operand

        static constexpr auto bitsize() { return 8 * sizeof(T); }

        static constexpr auto high_bit() { return static_cast<T>(1) << (bitsize() - 1); }
        static constexpr auto nth_bit(index_t n) { return static_cast<T>(1) << n; }
        static constexpr auto all_bits() { return ~static_cast<T>(0); }
        static constexpr auto all_but_high_bit() { return ~high_bit(); }
        static constexpr auto low_n_bits(index_t n) { return (static_cast<T>(1) << n) - 1; }

        static constexpr auto rot_mask() { return bitsize() - 1; }
        static constexpr auto address_round_mask() { return ~(sizeof(T) - 1); }

        // Operations with an operand

        static constexpr bool high_bit_of(T x) { return x >> (bitsize() - 1); }
        static constexpr bool nth_bit_of(T x, index_t n) { return (x >> n) & 1; }
        static constexpr auto all_but_high_bit_of(T x) { return x & all_but_high_bit(); }
        static constexpr auto low_n_bits_of(T x, index_t n) { return x & low_n_bits(n); }
        static constexpr auto clear_top_n_of(T x, index_t n) { return low_n_bits_of(x, bitsize() - n); }
        static constexpr auto set_top_n_of(T x, index_t n) { return x | ~low_n_bits(bitsize() - n); }

        static constexpr bool all_bits_set(T x) { return x == all_bits(); }
        static constexpr bool all_but_high_bit_set(T x) { return x == ~high_bit(); }

        static constexpr auto rot_masked(T x) { return x & rot_mask(); }
        static constexpr auto address_rounded(T x) { return x & address_round_mask(); }
        static constexpr auto shift_clipped(T x) { return std::min(x, static_cast<T>(bitsize()));}

        static constexpr auto nth_byte_of(T x, index_t n) { return (x & (static_cast<T>(0xFF) << (8 * n))) >> (8 * n); }
    };

}

#endif  /* _UCLE_CORE_UTIL_CONST_BIN_UTIL_HPP_ */
