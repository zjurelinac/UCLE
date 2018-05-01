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
        static constexpr auto address_round_mask(index_t round_bytes) { return ~(round_bytes - 1); }

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
        static constexpr auto address_rounded(T x, index_t round_bytes) { return x & address_round_mask(round_bytes); }
        static constexpr auto shift_clipped(T x) { return std::min(x, static_cast<T>(bitsize()));}

        static constexpr auto nth_byte_of(T x, index_t n) { return (x & (static_cast<T>(0xFF) << (8 * n))) >> (8 * n); }
        static constexpr auto nth_half_of(T x, index_t n) { return (x & (static_cast<T>(0xFFFF) << (16 * n))) >> (16 * n); }
        static constexpr auto nth_word_of(T x, index_t n) { return (x & (static_cast<T>(0xFFFFFFFF) << (32 * n))) >> (32 * n); }

        static constexpr auto clear_nth_byte_of(T x, index_t n) { return x & ~(static_cast<T>(0xFF) << (8 * n)); }
        static constexpr auto clear_nth_half_of(T x, index_t n) { return x & ~(static_cast<T>(0xFFFF) << (16 * n)); }
        static constexpr auto clear_nth_word_of(T x, index_t n) { return x & ~(static_cast<T>(0xFFFFFFFF) << (32 * n)); }

        static constexpr auto set_nth_byte_of(T x, index_t n, byte_t value) { return clear_nth_byte_of(x, n) | (value << (8 * n)); }
        static constexpr auto set_nth_half_of(T x, index_t n, half_t value) { return clear_nth_half_of(x, n) | (value << (16 * n)); }
        static constexpr auto set_nth_word_of(T x, index_t n, word_t value) { return clear_nth_word_of(x, n) | (value << (32 * n)); }
    };

}

#endif  /* _UCLE_CORE_UTIL_CONST_BIN_UTIL_HPP_ */
