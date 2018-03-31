#ifndef _UCLE_CORE_FNSIM_BASE_HPP_
#define _UCLE_CORE_FNSIM_BASE_HPP_

#include <common/types.hpp>

#include <memory>

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

}

#endif  /* _UCLE_CORE_FNSIM_BASE_HPP_ */
