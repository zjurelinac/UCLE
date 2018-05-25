#ifndef _UCLE_CORE_FNSIM_PROCESSOR_HPP_
#define _UCLE_CORE_FNSIM_PROCESSOR_HPP_

#include <fnsim/base.hpp>

#include <fnsim/device.hpp>

#include <memory>

namespace ucle::fnsim {

    template <unsigned N>
    class functional_processor_simulator {
        public:
            using address_type = meta::arch_address_t<N>;
            using device_config_type = device_config<address_type>;

            virtual ~functional_processor_simulator() = default;

            virtual status execute_single() = 0;
            virtual void reset() = 0;

            virtual void add_device(device_ptr dev_ptr, device_config_type cfg) = 0;

            virtual address_type get_program_counter() const = 0;
            virtual void set_program_counter(address_type location) = 0;

            virtual byte_t get_mem_byte(address_type location) const = 0;
            virtual half_t get_mem_half(address_type location) const = 0;
            virtual word_t get_mem_word(address_type location) const = 0;

            virtual void set_mem_byte(address_type location, byte_t value) = 0;
            virtual void set_mem_half(address_type location, half_t value) = 0;
            virtual void set_mem_word(address_type location, word_t value) = 0;

            auto get_state() { return state_; }
            void set_state(simulator_state state) { state_ = state; }

            virtual register_info get_reg_info() = 0;

            virtual bool is_mem_address_valid(address_type location) = 0;

        protected:
            void terminate_() { set_state(simulator_state::terminated); }

        private:
            simulator_state state_ = simulator_state::initialized;
    };

    template <unsigned N>
    using functional_processor_simulator_ptr = std::unique_ptr<functional_processor_simulator<N>>;

}

#endif  /* _UCLE_CORE_FNSIM_PROCESSOR_HPP_ */
