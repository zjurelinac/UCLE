#ifndef _UCLE_CORE_COMMON_TYPES_HPP_
#define _UCLE_CORE_COMMON_TYPES_HPP_

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

    using address8_t = uint8_t;
    using address16_t = uint16_t;
    using address32_t = uint32_t;
    using address64_t = uint64_t;

    using size_t = uint32_t;
    using index_t = int32_t;

    using priority_t = uint32_t;
    using counter_t = uint64_t;
    using frequency_t = uint64_t;

    namespace literals {
        constexpr frequency_t operator "" _Hz(unsigned long long f) { return f; }
        constexpr frequency_t operator "" _kHz(unsigned long long f) { return 1000*f; }
        constexpr frequency_t operator "" _MHz(unsigned long long f) { return 1000000*f; }
        constexpr frequency_t operator "" _GHz(unsigned long long f) { return 1000000000*f; }
    }

    template <typename AddressType = address32_t>
    struct address_range {
        using address_type = AddressType;

        address_type low_addr, high_addr;

        constexpr bool contains(address_type location) const { return low_addr <= location && location <= high_addr; }
        constexpr bool contains(const address_range<AddressType>& rhs) const { return low_addr <= rhs.low_addr && rhs.high_addr <= high_addr; }
    };

    template <typename AT>
    constexpr bool operator==(const address_range<AT>& lhs, const address_range<AT>& rhs)
        { return lhs.low_addr == rhs.low_addr && lhs.high_addr == rhs.high_addr; }

    template <typename AT>
    constexpr bool operator!=(const address_range<AT>& lhs, const address_range<AT>& rhs)
        { return lhs.low_addr != rhs.low_addr || lhs.high_addr != rhs.high_addr; }

    template <typename AT>
    constexpr bool operator<(const address_range<AT>& lhs, const address_range<AT>& rhs)
        { return (lhs.low_addr == rhs.low_addr) ? (lhs.high_addr < rhs.high_addr) : (lhs.low_addr < rhs.low_addr); }

    template <typename AT>
    constexpr bool operator>(const address_range<AT>& lhs, const address_range<AT>& rhs)
        { return (lhs.low_addr == rhs.low_addr) ? (lhs.high_addr > rhs.high_addr) : (lhs.low_addr > rhs.low_addr); }

    template <typename AT>
    constexpr bool operator<=(const address_range<AT>& lhs, const address_range<AT>& rhs)
        { return lhs < rhs || lhs == rhs; }

    template <typename AT>
    constexpr bool operator>=(const address_range<AT>& lhs, const address_range<AT>& rhs)
        { return lhs > rhs || lhs == rhs; }

    struct bitrange {
        index_t low = 0;
        index_t high = 0;

        constexpr bitrange(index_t hi, index_t lo) {
            if (lo > hi) std::swap(lo, hi);
            low = lo; high = hi;
        }
        constexpr auto mask() const { return (1 << (high - low + 1)) - 1; }
        constexpr auto shift() const { return low; }
        constexpr auto fullmask() const { return mask() << low; }
    };

    class base_exception : public std::exception {
        public:
            base_exception(std::string desc) : desc_{desc} {}
            virtual const char* what() const noexcept override { return desc_.c_str(); }
        private:
            std::string desc_;
    };

    class fatal_error : public base_exception { using base_exception::base_exception; };
}


#endif  /* _UCLE_CORE_COMMON_TYPES_HPP_ */
