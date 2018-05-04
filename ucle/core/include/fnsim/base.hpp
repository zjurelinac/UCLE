#ifndef _UCLE_CORE_FNSIM_BASE_HPP_
#define _UCLE_CORE_FNSIM_BASE_HPP_

#include <common/meta.hpp>
#include <common/structures.hpp>
#include <common/types.hpp>

#include <fnsim/exceptions.hpp>

#include <libs/fmt/format.h>
#include <libs/nlohmann/json.hpp>

#include <map>
#include <memory>
#include <string>
#include <variant>

namespace ucle::fnsim {
    using json = nlohmann::json;

    // Enumerations

    enum class byte_order { little_endian, big_endian };

    enum class device_class { memory, addressable_device, non_addressable_device };

    enum class device_mapping { none, memory, port };

    // enum class device_status {};  // TODO: Select possible options

    enum class detail_level { none, medium, maximum };

    enum class simulator_state { initialized, loaded, running, stopped, exception, terminated };

    inline std::string to_string(simulator_state state)
    {
        switch (state) {
            case simulator_state::initialized:  return "initialized";
            case simulator_state::loaded:       return "loaded";
            case simulator_state::running:      return "running";
            case simulator_state::stopped:      return "stopped";
            case simulator_state::exception:    return "exception";
            case simulator_state::terminated:   return "terminated";
            default:                            return "unknown";
        }
    }

    enum class status {
        ok,
        invalid_state,
        invalid_address_range,
        invalid_identifier,
        invalid_instruction,
        invalid_program,
        invalid_argument,
        invalid_memory_location,
        runtime_exception,
        filesystem_error,
        generic_error
    };

    inline std::string to_string(status stat)
    {
        switch (stat) {
            case status::ok:                        return "ok";
            case status::invalid_state:             return "invalid state";
            case status::invalid_address_range:     return "invalid address range";
            case status::invalid_identifier:        return "invalid identifier";
            case status::invalid_instruction:       return "invalid instruction";
            case status::invalid_program:           return "invalid program";
            case status::invalid_argument:          return "invalid argument";
            case status::invalid_memory_location:   return "invalid memory location";
            case status::runtime_exception:         return "runtime exception";
            case status::filesystem_error:          return "filesystem error";
            case status::generic_error:             return "generic error";
            default:                                return "unknown";
        }
    }

    constexpr bool is_success(status stat) { return stat == status::ok; }
    constexpr bool is_error(status stat) { return stat != status::ok; }

    enum class device_status { idle, pending, interrupt };

    inline std::string to_string(device_status stat)
    {
        switch (stat) {
            case device_status::idle:           return "idle";
            case device_status::pending:        return "pending";
            case device_status::interrupt:      return "interrupt";
            default:                            return "unknown";
        }
    }

    // Config structures

    template <typename AddressType>
    struct device_config {
        using address_type = AddressType;

        address_type        start_address       = 0;
        size_t              addr_space_size     = 0;
        device_class        dev_class           = device_class::addressable_device;
    };

    struct processor_config {
        size_t              mem_size;
        address_range<>     mem_addr_range          = {0, 0xFFFFFFFF};
        address_range<>     dev_addr_range          = {0, 0};
        device_mapping      default_mapping         = device_mapping::memory;
    };

    // Utility structures

    template <typename AddressType>
    struct state_info {
        using address_type = AddressType;

        simulator_state state;
        address_type    program_location;
        std::string     asm_annotation;
    };

    struct reg_val {
        using value_type = std::variant<bool, byte_t, half_t, word_t, dword_t>;

        reg_val() noexcept {};
        reg_val(const reg_val& other) noexcept : value(other.value) {};
        reg_val(reg_val&& other) noexcept : value(std::move(other.value)) {};

        reg_val& operator=(const reg_val&) noexcept = default;
        reg_val& operator=(reg_val&&) noexcept      = default;

        ~reg_val()                                  = default;

        template <typename T, typename = meta::is_storage_equiv_t<T>>
        reg_val(T&& t) : value(t) {}

        template <class T, typename = meta::is_storage_equiv_t<T>>
        reg_val& operator=(T&& t) noexcept { value = t; return *this; }

        value_type value;
    };

    inline std::string to_string(reg_val rv)
    {
        return std::visit(meta::overloaded {
            [](bool val)    { return fmt::format("{}", val); },
            [](byte_t val)  { return fmt::format("{}", val); },
            [](half_t val)  { return fmt::format("{}", val); },
            [](word_t val)  { return fmt::format("{}", val); },
            [](dword_t val) { return fmt::format("{}", val); }
        }, rv.value);
    }

    inline std::string to_xstring(reg_val rv)
    {
        return std::visit(meta::overloaded {
            [](bool val)    { return fmt::format("{:d}", val); },
            [](byte_t val)  { return fmt::format("0x{:02X}", val); },
            [](half_t val)  { return fmt::format("0x{:04X}", val); },
            [](word_t val)  { return fmt::format("0x{:08X}", val); },
            [](dword_t val) { return fmt::format("0x{:016X}", val); }
        }, rv.value);
    }

    inline int64_t to_int(reg_val rv)
    {
        return std::visit(meta::overloaded {
            [](bool val)    { return static_cast<int64_t>(val); },
            [](byte_t val)  { return static_cast<int64_t>(val); },
            [](half_t val)  { return static_cast<int64_t>(val); },
            [](word_t val)  { return static_cast<int64_t>(val); },
            [](dword_t val) { return static_cast<int64_t>(val); }
        }, rv.value);
    }

    inline uint64_t to_uint(reg_val rv)
    {
        return std::visit(meta::overloaded {
            [](bool val)    { return static_cast<uint64_t>(val); },
            [](byte_t val)  { return static_cast<uint64_t>(val); },
            [](half_t val)  { return static_cast<uint64_t>(val); },
            [](word_t val)  { return static_cast<uint64_t>(val); },
            [](dword_t val) { return static_cast<uint64_t>(val); }
        }, rv.value);
    }

    inline size_t size(reg_val rv)
    {
        return std::visit(meta::overloaded {
            [](bool) { return 1; },
            [](byte_t) { return 8; },
            [](half_t) { return 16; },
            [](word_t) { return 32; },
            [](dword_t) { return 64; }
        }, rv.value);
    }

    inline void to_json(json& j, const reg_val& rv)
    {
        std::visit(meta::overloaded {
            [&j](bool val)    { j = val; },
            [&j](byte_t val)  { j = val; },
            [&j](half_t val)  { j = val; },
            [&j](word_t val)  { j = val; },
            [&j](dword_t val) { j = val; }
        }, rv.value);
    }

    using register_info = std::map<std::string, reg_val>;

    struct execution_info {
        counter_t instruction_cnt;
        int64_t duration_ns;
    };
}

#endif  /* _UCLE_CORE_FNSIM_BASE_HPP_ */
