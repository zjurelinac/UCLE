#ifndef _UCLE_CORE_FNSIM_BASE_HPP_
#define _UCLE_CORE_FNSIM_BASE_HPP_

#include <common/meta.hpp>
#include <common/types.hpp>

#include <libs/fmt/format.h>

#include <map>
#include <memory>
#include <string>
#include <variant>

namespace ucle::fnsim {

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
        runtime_exception,
        filesystem_error
    };

    inline std::string to_string(status stat)
    {
        switch (stat) {
            case status::ok:                    return "ok";
            case status::invalid_state:         return "invalid state";
            case status::invalid_address_range: return "invalid address range";
            case status::invalid_identifier:    return "invalid identifier";
            case status::invalid_instruction:   return "invalid instruction";
            case status::invalid_program:       return "invalid program";
            case status::runtime_exception:     return "runtime exception";
            case status::filesystem_error:      return "filesystem error";
            default:                            return "unknown";
        }
    }

    constexpr bool is_success(status stat) { return stat == status::ok; }
    constexpr bool is_error(status stat) { return stat != status::ok; }

    // Config structures

    struct device_config {
        device_class    dev_class           = device_class::addressable_device;
        address_range   addr_range          = {0, 0};
        bool            uses_interrupts     = false;
        priority_t      interrupt_priority  = 0;
    };

    struct processor_config {
        size_t          mem_size;
        address_range   mem_addr_range      = {0, 0xFFFFFFFF};
        device_mapping  default_mapping     = device_mapping::memory;
        address_range   dev_addr_range      = {0, 0};
    };

    struct simulation_config {
        bool            count_exec_cycles   = false;
        bool            measure_exec_time   = false;
        detail_level    detailness          = detail_level::none;
    };

    // Utility structures

    struct state_info {
        simulator_state state;
        address_t       program_location;
        std::string     asm_annotation;
    };

    using reg_val = std::variant<bool, byte_t, half_t, word_t, dword_t>;
    using reg_info = std::map<std::string, reg_val>;

    inline std::string to_string(reg_val rv)
    {
        return std::visit(meta::overloaded {
            [](bool val) { return fmt::format("{}", val); },
            [](byte_t val) { return fmt::format("{}", val); },
            [](half_t val) { return fmt::format("{}", val); },
            [](word_t val) { return fmt::format("{}", val); },
            [](dword_t val) { return fmt::format("{}", val); },
        }, rv);
    }

    inline std::string to_xstring(reg_val rv)
    {
        return std::visit(meta::overloaded {
            [](bool val) { return fmt::format("{:d}", val); },
            [](byte_t val) { return fmt::format("0x{:02X}", val); },
            [](half_t val) { return fmt::format("0x{:04X}", val); },
            [](word_t val) { return fmt::format("0x{:08X}", val); },
            [](dword_t val) { return fmt::format("0x{:016X}", val); },
        }, rv);
    }

    struct execution_info {
        counter_t instruction_cnt;
        int64_t duration_ns;
    };
}

#endif  /* _UCLE_CORE_FNSIM_BASE_HPP_ */
