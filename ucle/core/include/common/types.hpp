#ifndef _CORE_COMMON_BASE_H_
#define _CORE_COMMON_BASE_H_

#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <exception>
#include <utility>
#include <variant>

namespace ucle {

    /*** Generic types ***/

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

    /*** Run-utility types ***/

    template <typename Result, typename Error>
    using outcome = std::variant<Result, Error>;

    template <typename Result, typename Error>
    bool is_successful(outcome<Result, Error> out) { return std::holds_alternative<Result>(out); }

    template <typename Result, typename Error>
    Result get_result(outcome<Result, Error> out) { return std::get<Result>(out); }

    template <typename Result, typename Error>
    Error get_error(outcome<Result, Error> out) { return std::get<Error>(out); }

    enum class success_t { SUCCESS };

    /*struct memory_access_error : public std::exception {
        memory_access_error(address_t location, const char* msg) { sprintf(msg_, "Memory access error [@%08X]: %s", location, msg); }
        virtual const char* what() const noexcept override { return msg_; }
        private: char msg_[64];
    };*/

}


#endif  // _CORE_COMMON_BASE_H_