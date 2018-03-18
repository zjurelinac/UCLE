#ifndef _CORE_UTIL_BINARY_H_
#define _CORE_UTIL_BINARY_H_

#include <common/types.hpp>

namespace ucle {
    namespace util {

        // inline word_t word_from_bytes__le(byte_t B[]) { return (B[3] << 24) | (B[2] << 16) | (B[1] << 8) | B[0]; }
        // inline word_t word_from_bytes__be(byte_t B[]) { return (B[0] << 24) | (B[1] << 16) | (B[2] << 8) | B[3]; }

    }
}

#endif  // _CORE_UTIL_BINARY_H_
