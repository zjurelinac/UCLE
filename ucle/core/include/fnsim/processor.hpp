#ifndef _UCLE_CORE_FNSIM_PROCESSOR_HPP_
#define _UCLE_CORE_FNSIM_PROCESSOR_HPP_

#include <fnsim/base.hpp>

#include <fnsim/device.hpp>

#include <memory>

namespace ucle::fnsim {

    class functional_processor_simulator {
        public:
            virtual ~functional_processor_simulator() = default;

            virtual status execute_single() = 0;
            virtual void reset() = 0;

            virtual void add_device(device_ptr dev_ptr, device_config cfg) = 0;

            virtual address_t get_program_counter() const = 0;
            virtual void set_program_counter(address_t location) = 0;

            virtual byte_t get_mem_byte(address_t location) const = 0;
            virtual void set_mem_byte(address_t location, byte_t value) = 0;

            auto get_state() { return state_; }
            void set_state(simulator_state state) { state_ = state; }

            virtual register_info get_reg_info() = 0;

            virtual bool is_mem_address_valid(address_t location) = 0;

        protected:
            void terminate_() { set_state(simulator_state::terminated); }

        private:
            simulator_state state_ = simulator_state::initialized;
    };

    using functional_processor_simulator_ptr = std::unique_ptr<functional_processor_simulator>;

}

#endif  /* _UCLE_CORE_FNSIM_PROCESSOR_HPP_ */
