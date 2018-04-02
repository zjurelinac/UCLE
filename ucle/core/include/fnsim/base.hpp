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

    enum class byte_order { little_endian, big_endian };

    enum class device_mapping { simulator_default, memory_mapping, port_mapping, no_mapping };
    enum class device_status {};  // TODO: Select possible options

    enum class simulator_state { initialized, loaded, running, stopped, exception, terminated };

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

    constexpr bool is_success(status stat) { return stat == status::ok; }
    constexpr bool is_error(status stat) { return stat != status::ok; }

    class device {
        public:
            virtual ~device() = default;

            virtual void work() = 0;
            virtual void status() = 0;
            virtual void reset() = 0;
    };

    using device_ptr = std::shared_ptr<device>;

    struct device_config {
        bool            is_addressable = true;
        bool            uses_interrupts = false;
        device_mapping  mapping = device_mapping::simulator_default;
        address_range   addr_range = {0, 0};
        priority_t      interrupt_priority;
    };

    struct simulator_config {
        size_t          memory_size;
        address_range   memory_addr_range = {0, 0xFFFFFFFF};
        device_mapping  devices_default_mapping = device_mapping::memory_mapping;
        address_range   devices_addr_range = {0, 0};
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

    // enum class
    // struct watch_info {  };

    struct state_info {
        simulator_state state;
        address_t program_location;
    };

}

#endif  /* _UCLE_CORE_FNSIM_BASE_HPP_ */
