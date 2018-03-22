#ifndef _UCLE_CORE_SIMULATORS_FUNCTIONAL_FRISC_HPP_
#define _UCLE_CORE_SIMULATORS_FUNCTIONAL_FRISC_HPP_

#include <array>

#include <common/types.hpp>
#include <fnsim/fnsim.hpp>
#include <fnsim/address_space.hpp>
#include <fnsim/basic_devices.hpp>
#include <fnsim/registers.hpp>
#include <fnsim/simulator.hpp>

namespace ucle::fnsim {

    struct frisc_register_file : register_file {
        std::array<reg<32>, 8> R;
        reg<32>& SP = R[7];

        reg<32> PC;
        reg<32> SR;  // TODO: A flags register!

        void clear() override
        {
            PC = 0;
            SR = 0;

            for (auto i = 0u; i < R.size(); ++i)
                R[i] = 0;
        }
    };

    class frisc_simulator : public functional_simulator_impl<frisc_register_file, address_space<mapped_device_ptr>, memory> {
        using functional_simulator_impl<frisc_register_file, address_space<mapped_device_ptr>, memory>::functional_simulator_impl;

        protected:
            virtual address_t get_program_counter_() const override { return regs_.PC.get(); };
            virtual void set_program_counter_(address_t location) override { regs_.PC = location; }

            virtual status_t execute_single_() override;

    };

}

#endif  /* _UCLE_CORE_SIMULATORS_FUNCTIONAL_FRISC_HPP_ */
