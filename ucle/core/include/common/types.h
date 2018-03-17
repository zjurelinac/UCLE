#ifndef _CORE_COMMON_BASE_H_
#define _CORE_COMMON_BASE_H_

#include <cstdint>
#include <utility>

namespace ucle {

    using byte_t = uint8_t;
    using sbyte_t = int8_t;

    using half_t = uint16_t;
    using shalf_t = int16_t;

    using word_t = uint32_t;
    using sword_t = int32_t;

    using dword_t = uint64_t;
    using sdword_t = int64_t;

    using address_t = uint32_t;

    struct address_range {
        address_t lower_bound, upper_bound;

        bool contains(address_t location) const { return lower_bound <= location && location <= upper_bound; }
    };


}


#endif  // _CORE_COMMON_BASE_H_