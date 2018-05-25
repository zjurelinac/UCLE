#ifndef _UCLE_CORE_UTIL_BINARY_HPP_
#define _UCLE_CORE_UTIL_BINARY_HPP_

#include <common/types.hpp>
#include <common/structures.hpp>

#include <util/const_bin_util.hpp>

#include <type_traits>

namespace ucle::util {

    struct basic_arith_flags { bool C, V, N, Z; };

    template <typename ValueType>
    struct unop {
        using value_type = ValueType;
        using cbu = const_bin_util<value_type>;

        static constexpr value_type invert(value_type op)
        {
            return ~op;
        }

        static constexpr value_type negate(value_type op)
        {
            return ~op + 1;
        }

        static constexpr value_type sign_extend(value_type op, size_t from)
        {
            auto n = cbu::bitsize() - from;
            return cbu::nth_bit_of(op, from - 1) ? cbu::set_top_n_of(op, n) : cbu::clear_top_n_of(op, n);
        }
    };

    template <typename ValueType, typename ArithFlags = basic_arith_flags>
    struct binop {
        using value_type = ValueType;
        using flag_type = ArithFlags;
        using res_t = std::pair<value_type, flag_type>;
        using cbu = const_bin_util<value_type>;

        static constexpr res_t op_add(value_type op1, value_type op2)
        {
            auto result = op1 + op2;
            return { result, determine_add_flags_(op1, op2, result) };
        }

        static constexpr res_t op_adc(value_type op1, value_type op2, bool carry)
        {
            auto result = op1 + op2 + carry;
            return { result, determine_addcarry_flags_(op1, op2, result, carry) };
        }

        static constexpr res_t op_sub(value_type op1, value_type op2)
        {
            op2 = ~op2 + 1;
            auto result = op1 + op2;
            return { result, determine_add_flags_(op1, op2, result) };
        }

        static constexpr res_t op_sbc(value_type op1, value_type op2, bool carry)
        {
            op2 = ~op2 + 1;
            auto result = op1 + op2 - carry;
            return { result, determine_addcarry_flags_(op1, op2, result, carry) };
        }

        static constexpr res_t op_cmp(value_type op1, value_type op2)
        {
            op2 = ~op2 + 1;
            return { 0, determine_add_flags_(op1, op2, op1 + op2) };
        }

        static constexpr res_t op_and(value_type op1, value_type op2) {
            auto result = op1 & op2;
            return { result, determine_axor_flags(result) };
        }

        static constexpr res_t op_or(value_type op1, value_type op2)
        {
            auto result = op1 | op2;
            return { result, determine_axor_flags(result) };
        }

        static constexpr res_t op_xor(value_type op1, value_type op2)
        {
            auto result = op1 ^ op2;
            return { result, determine_axor_flags(result) };
        }

        static constexpr res_t op_shl(value_type op1, value_type op2)
        {
            auto result = op2 >= cbu::bitsize() ? 0 : op1 << op2;
            return { result, { cbu::nth_bit_of(op1, cbu::bitsize() - op2), 0, cbu::high_bit_of(result), result == 0 } };
        }

        static res_t op_shr(value_type op1, value_type op2)
        {
            auto result = op2 >= cbu::bitsize() ? 0 : op1 >> op2;
            return { result, { cbu::nth_bit_of(op1, op2 - 1), 0, cbu::high_bit_of(result), result == 0 } };
        }

        static constexpr res_t op_asr(value_type op1, value_type op2)
        {
            op2 = cbu::shift_clipped(op2);
            using signed_value_type = std::make_signed_t<value_type>;
            value_type result = signed_value_type(op1) >> op2;
            return { result, { cbu::nth_bit_of(op1, op2 - 1), 0, cbu::high_bit_of(result), result == 0 } };
        }

        static constexpr res_t op_rtl(value_type op1, value_type op2)
        {
            op2 = cbu::rot_masked(op2);
            auto shift = cbu::bitsize() - op2;
            auto result = (op1 << op2) | (op1 >> shift);
            return { result, { cbu::nth_bit_of(op1, shift), 0, cbu::high_bit_of(result), result == 0 } };
        }

        static constexpr res_t op_rtr(value_type op1, value_type op2)
        {
            op2 = cbu::rot_masked(op2);
            auto result = (op1 << (cbu::bitsize() - op2)) | (op1 >> op2);
            return { result, { cbu::nth_bit_of(op1, op2 - 1), 0, cbu::high_bit_of(result), result == 0 } };
        }

        private:
            static constexpr flag_type determine_add_flags_(value_type op1, value_type op2, value_type result)
            {   // TODO: Optimize if possible!
                bool C0 = (op1 & 1) + (op2 & 1) > 1;
                bool Cn = cbu::high_bit_of((op1 >> 1) + (op2 >> 1) + C0);
                bool Cn_1 = cbu::high_bit_of(cbu::all_but_high_bit_of(op1) + cbu::all_but_high_bit_of(op2));
                return {Cn, Cn ^ Cn_1, cbu::high_bit_of(result), result == 0};
            }

            static constexpr flag_type determine_addcarry_flags_(value_type op1, value_type op2, value_type result, bool carry)
            {   // TODO: Optimize if possible!
                bool C0 = (op1 & 1) + (op2 & 1) + carry > 1;
                bool Cn = cbu::high_bit_of((op1 >> 1) + (op2 >> 1) + C0);
                bool Cn_1 = cbu::high_bit_of(cbu::all_but_high_bit_of(op1) + cbu::all_but_high_bit_of(op2) + carry);
                return {Cn, Cn ^ Cn_1, cbu::high_bit_of(result), result == 0};
            }

            static constexpr flag_type determine_axor_flags(value_type result) {
                return {0, 0, cbu::high_bit_of(result), result == 0};
            }

        // Additional binary AL operations, if required -> bitset, bitclear...
    };
}

#endif  // _UCLE_CORE_UTIL_BINARY_HPP_
