#ifndef _UCLE_CORE_FNSIM_FNSIM_HPP_
#define _UCLE_CORE_FNSIM_FNSIM_HPP_

#include <common/types.hpp>

#include <fnsim/base.hpp>

#include <memory>

namespace ucle::fnsim {

    class functional_simulator {
        public:
            virtual ~functional_simulator() = default;

            virtual address_t get_program_counter() const = 0;
            virtual void set_program_counter(address_t location) = 0;

            virtual status execute_single() = 0;
            virtual void reset() = 0;

            virtual byte_t get_byte(address_t location) const = 0;
            virtual void set_byte(address_t location, byte_t value) = 0;

            virtual reg_info get_reg_info() = 0;

            virtual identifier_t add_device(device_ptr dev_ptr, device_config cfg) = 0;
            virtual void remove_device(identifier_t dev_id) = 0;

            simulator_state get_state() { return state_; }
            void set_state(simulator_state state) { state_ = state; }

        protected:
            void terminate_() { set_state(simulator_state::terminated); }

        private:
            simulator_state state_ = simulator_state::initialized;
    };

    using functional_simulator_ptr = std::unique_ptr<functional_simulator>;

}

#endif  /* _UCLE_CORE_FNSIM_FNSIM_HPP_ */
