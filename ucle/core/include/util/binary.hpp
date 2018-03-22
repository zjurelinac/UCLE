#ifndef _CORE_UTIL_BINARY_H_
#define _CORE_UTIL_BINARY_H_

#include <common/types.hpp>

#include <type_traits>


namespace ucle::util {

    // inline word_t word_from_bytes__le(byte_t B[]) { return (B[3] << 24) | (B[2] << 16) | (B[1] << 8) | B[0]; }
    // inline word_t word_from_bytes__be(byte_t B[]) { return (B[0] << 24) | (B[1] << 16) | (B[2] << 8) | B[3]; }

    template <typename T>
    struct const_bit_util {
        static constexpr auto bitsize() { return 8 * sizeof(T); }
        static constexpr T highest_bit() { return 1 << bitsize<T>()); }
        static constexpr auto rot_mask() { return bitsize() - 1; }
    };

    template <typename ValueType>
    struct binop {
        using value_type = ValueType;
        using res_t = std::pair<value_type, arith_flags>;
        using cbu = const_bit_util<value_type>;

        public:

            static res_t op_add(value_type op1, value_type op2, arith_flags)
            {
                auto result = op1 + op2;
                return { result, determine_add_flags_(op1, op2, result) };
            }

            static res_t op_adc(value_type op1, value_type op2, arith_flags fs)
            {
                op2 += fs.C;
                auto result = op1 + op2;
                return { result, determine_add_flags_(op1, op2, result) };
            }

            static res_t op_sub(value_type op1, value_type op2, arith_flags)
            {
                op2 = ~op2 + 1;
                auto result = op1 + op2;
                return { result, determine_add_flags_(op1, op2, result) };
            }

            static res_t op_sbc(value_type op1, value_type op2, arith_flags fs)
            {
                op2 = ~op2 + 1 - fs.C;
                auto result = op1 + op2;
                return { result, determine_add_flags_(op1, op2, result) };
            }

            static res_t op_cmp(value_type op1, value_type op2, arith_flags)
            {
                op2 = ~op2 + 1;
                return { 0, determine_add_flags_(op1, op2, op1 + op2) };
            }

            static res_t op_and(value_type op1, value_type op2, arith_flags) {
                auto result = op1 & op2;
                return { result, determine_axor_flags(result) };
            }

            static res_t op_or(value_type op1, value_type op2, arith_flags)
            {
                auto result = op1 | op2;
                return { result, determine_axor_flags(result) };
            }
            
            static res_t op_xor(value_type op1, value_type op2, arith_flags)
            {
                auto result = op1 ^ op2;
                return { result, determine_axor_flags(result) };
            }

            static res_t op_shl(value_type op1, value_type op2, arith_flags)
            {
                auto result = op1 << op2;
                arith_flags nfs = { op1 & (cbu::bitsize() + 1 - op2), 0, result & cbu::highest_bit(), result == 0 }
                return { result, nfs };
            }

            static res_t op_shr(value_type op1, value_type op2, arith_flags)
            {
                auto result = op1 >> op2;
                arith_flags nfs = { op1 & (op2 - 1), 0, result & cbu::highest_bit(), result == 0 }
                return { result, nfs };
            }

            static res_t op_asr(value_type op1, value_type op2, arith_flags)
            {
                auto result = std::make_signed(op1) >> op2;
                arith_flags nfs = { op1 & op1 & (op2 - 1), 0, result & cbu::highest_bit(), result == 0 }
                return { result, nfs };
            }

            static res_t op_rtl(value_type op1, value_type op2, arith_flags fs)
            {
                op2 &= cbu::rot_mask();
                // auto result = ;
                /*
                src2 &= 0x1F;  // (% 32)
                word wrap_mask = ((1 << src2) - 1) << (32 - src2);
                word wrap_bits = (src1 & wrap_mask) >> (32 - src2);
                dest = (src1 << src2) | wrap_bits;
                nC = wrap_bits & 1;
                */
                return {};
            }

            static res_t op_rtr(value_type op1, value_type op2, arith_flags fs)
            {
                op2 &= cbu::rot_mask();
                // auto result = ;
                /*
                src2 &= 0x1F;  // (% 32)
                word wrap_mask = (1 << src2) - 1;
                word wrap_bits = (src1 & wrap_mask);
                dest = (src1 >> src2) | (wrap_bits << (32 - src2));
                nC = wrap_bits & 1;
                */
                return {};
            }

        private:
            arith_flags determine_add_flags_(value_type op1, value_type op2, value_type result)
            {
            /*
                bool C = (((longword) src1) + src2) & (1ULL << 32);
                bool C1 = ((src1 & 0x7FFFFFFF) + (src2 & 0x7FFFFFFF)) & (1U << 31);
                bool V = C ^ C1;
            */
            }

            arith_flags determine_axor_flags(value_type result) {
                return { 0, 0, result & cbu::highest_bit(), result == 0 };
            }
        
        // Additional binary AL operations, if required -> bitset, bitclear...
    };
}

#endif  // _CORE_UTIL_BINARY_H_
