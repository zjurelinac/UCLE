#ifndef _CORE_COMMON_TYPES_HPP_
#define _CORE_COMMON_TYPES_HPP_

#include <bitset>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
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
    using size_t = uint32_t;
    using index_t = uint32_t;

    using identifier_t = uint32_t;
    using priority_t = uint32_t;

    enum class status {
        ok,
        invalid_state,
        invalid_address_range,
        invalid_identifier,
        invalid_instruction,
        runtime_exception,
        filesystem_error
    };

    constexpr bool is_success(status stat) { return stat == status::ok; }
    constexpr bool is_error(status stat) { return stat != status::ok; }

    enum class byte_order { LE, BE };

    struct address_range {
        address_t low_addr, high_addr;

        bool operator==(const address_range& other) const
            { return low_addr == other.low_addr && high_addr == other.high_addr; }
        bool operator!=(const address_range& other) const
            { return low_addr != other.low_addr || high_addr != other.high_addr; }
        bool operator<(const address_range& other) const
            { return (low_addr == other.low_addr) ? (high_addr < other.high_addr) : (low_addr < other.low_addr); }
        bool operator>(const address_range& other) const
            { return (low_addr == other.low_addr) ? (high_addr > other.high_addr) : (low_addr > other.low_addr); }

        bool contains(address_t location) const { return low_addr <= location && location <= high_addr; }
        bool contains(const address_range& other) const { return low_addr <= other.low_addr && other.high_addr <= high_addr; }
    };

    struct arith_flags { bool C, V, N, Z; };  // TODO: Redesign (& rename)

    struct bitrange {
        index_t low, high;

        constexpr bitrange(index_t hi, index_t lo) : low(lo), high(hi) {
            if (lo > hi) {
                low = hi;
                high = lo;
            }
        }
        constexpr auto mask() const { return (1 << (high - low + 1)) - 1; }
        constexpr auto shift() const { return low; }
        constexpr auto fullmask() const { return mask() << low; }
    };
}


#endif  /* _CORE_COMMON_TYPES_HPP_ */