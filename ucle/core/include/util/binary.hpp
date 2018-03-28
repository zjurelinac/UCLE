#ifndef _UCLE_CORE_UTIL_BINARY_HPP_
#define _UCLE_CORE_UTIL_BINARY_HPP_

#include <common/types.hpp>

#include <util/const_bit_util.hpp>

#include <type_traits>

namespace ucle::util {

    template <typename ValueType>
    struct unop {
        using value_type = ValueType;
        using cbu = const_bit_util<value_type>;

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
            auto ext_num = cbu::bitsize() - from;
            return (op & cbu::nth_bit(from)) ? (op | cbu::set_topn_mask(ext_num)) : (op & cbu::clear_topn_mask(ext_num));
        }
    };

    template <typename ValueType, typename ArithFlags = arith_flags>
    struct binop {
        using value_type = ValueType;
        using flag_type = ArithFlags;
        using res_t = std::pair<value_type, arith_flags>;
        using cbu = const_bit_util<value_type>;

        static constexpr res_t op_add(value_type op1, value_type op2, flag_type)
        {
            auto result = op1 + op2;
            return { result, determine_add_flags_(op1, op2, result) };
        }

        static constexpr res_t op_adc(value_type op1, value_type op2, flag_type fs)
        {
            auto result = op1 + op2 + fs.C;
            return { result, determine_addcarry_flags_(op1, op2, result, fs) };
        }

        static constexpr res_t op_sub(value_type op1, value_type op2, flag_type)
        {
            op2 = ~op2 + 1;
            auto result = op1 + op2;
            return { result, determine_add_flags_(op1, op2, result) };
        }

        static constexpr res_t op_sbc(value_type op1, value_type op2, flag_type fs)
        {
            op2 = ~op2 + 1;
            auto result = op1 + op2 - fs.C;
            return { result, determine_addcarry_flags_(op1, op2, result, fs) };
        }

        static constexpr res_t op_cmp(value_type op1, value_type op2, flag_type)
        {
            op2 = ~op2 + 1;
            return { 0, determine_add_flags_(op1, op2, op1 + op2) };
        }

        static constexpr res_t op_and(value_type op1, value_type op2, flag_type) {
            auto result = op1 & op2;
            return { result, determine_axor_flags(result) };
        }

        static constexpr res_t op_or(value_type op1, value_type op2, flag_type)
        {
            auto result = op1 | op2;
            return { result, determine_axor_flags(result) };
        }

        static constexpr res_t op_xor(value_type op1, value_type op2, flag_type)
        {
            auto result = op1 ^ op2;
            return { result, determine_axor_flags(result) };
        }

        static constexpr res_t op_shl(value_type op1, value_type op2, flag_type)
        {
            auto result = op1 << op2;
            flag_type nfs = { op1 & (cbu::bitsize() + 1 - op2), 0, result & cbu::highest_bit(), result == 0 };
            return { result, nfs };
        }

        static constexpr res_t op_shr(value_type op1, value_type op2, flag_type)
        {
            auto result = op1 >> op2;
            flag_type nfs = { op1 & (op2 - 1), 0, result & cbu::highest_bit(), result == 0 };
            return { result, nfs };
        }

        static constexpr res_t op_asr(value_type op1, value_type op2, flag_type)
        {
            auto result = std::make_signed(op1) >> op2;
            flag_type nfs = { op1 & op1 & (op2 - 1), 0, result & cbu::highest_bit(), result == 0 };
            return { result, nfs };
        }

        static constexpr res_t op_rtl(value_type op1, value_type op2, flag_type fs)
        {
            op2 &= cbu::rot_mask();
            auto shift = cbu::bitsize() - op2;
            auto result = (op1 << op2) | (op1 >> shift);
            flag_type nfs = { op1 & (1 << shift), 0, result & cbu::highest_bit(), result == 0 };
            return { result, nfs };
        }

        static constexpr res_t op_rtr(value_type op1, value_type op2, arith_flags fs)
        {
            op2 &= cbu::rot_mask();
            auto result = (op1 << (cbu::bitsize() - op2)) | (op1 >> op2);
            flag_type nfs = { op1 & (1 << op2 - 1), 0, result & cbu::highest_bit(), result == 0 };
            return { result, nfs };
        }

    private:
        static constexpr flag_type determine_add_flags_(value_type op1, value_type op2, value_type result)
        {   // TODO: Optimize if possible!
            auto hi_bit = cbu::highest_bit();
            auto oth_bits = cbu::all_but_highest_bit();
            bool C = ((op1 >> 1) + (op2 >> 1) + ((op1 & 1) + (op2 & 1))) & hi_bit;
            bool C1 = ((op1 & oth_bits) + (op2 & oth_bits)) & hi_bit;
            return { C, C ^ C1, result & hi_bit, result == 0 };
        }

        static constexpr flag_type determine_addcarry_flags_(value_type op1, value_type op2, value_type result, flag_type old)
        {   // TODO: Optimize if possible!
            auto hi_bit = cbu::highest_bit();
            auto oth_bits = cbu::all_but_highest_bit();
            bool C = ((op1 >> 1) + (op2 >> 1) + ((op1 & 1) + (op2 & 1))) & hi_bit;
            bool C1 = ((op1 & oth_bits) + (op2 & oth_bits)) & hi_bit;
            return { C, C ^ C1, result & hi_bit, result == 0 };
        }

        static constexpr flag_type determine_axor_flags(value_type result) {
            return { 0, 0, result & cbu::highest_bit(), result == 0 };
        }

        // Additional binary AL operations, if required -> bitset, bitclear...
    };
}

#endif  // _UCLE_CORE_UTIL_BINARY_HPP_
