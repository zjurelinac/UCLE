#ifndef _CORE_UTIL_BINARY_H_
#define _CORE_UTIL_BINARY_H_

#include <common/types.hpp>

namespace ucle::util {

    // inline word_t word_from_bytes__le(byte_t B[]) { return (B[3] << 24) | (B[2] << 16) | (B[1] << 8) | B[0]; }
    // inline word_t word_from_bytes__be(byte_t B[]) { return (B[0] << 24) | (B[1] << 16) | (B[2] << 8) | B[3]; }

    template <typename ValueType>
    struct binop {
        using value_type = ValueType;
        using res_t = std::pair<value_type, arith_flags>;

        static res_t op_add(value_type op1, value_type op2, arith_flags fs) {}
        static res_t op_adc(value_type op1, value_type op2, arith_flags fs) {}
        static res_t op_sub(value_type op1, value_type op2, arith_flags fs) {}
        static res_t op_sbc(value_type op1, value_type op2, arith_flags fs) {}

        static res_t op_and(value_type op1, value_type op2, arith_flags fs) {}
        static res_t op_or(value_type op1, value_type op2, arith_flags fs) {}
        static res_t op_xor(value_type op1, value_type op2, arith_flags fs) {}

        static res_t op_shl(value_type op1, value_type op2, arith_flags fs) {}
        static res_t op_shr(value_type op1, value_type op2, arith_flags fs) {}
        static res_t op_asr(value_type op1, value_type op2, arith_flags fs) {}

        static res_t op_rtl(value_type op1, value_type op2, arith_flags fs) {}
        static res_t op_rtr(value_type op1, value_type op2, arith_flags fs) {}

        // ...
    };
}

#endif  // _CORE_UTIL_BINARY_H_
