#ifndef _UCLE_CORE_SIMULATORS_FUNCTIONAL_FRISC_HPP_
#define _UCLE_CORE_SIMULATORS_FUNCTIONAL_FRISC_HPP_

#include <common/types.hpp>

#include <fnsim/fnsim.hpp>
#include <fnsim/address_space.hpp>
#include <fnsim/basic_devices.hpp>
#include <fnsim/registers.hpp>
#include <fnsim/simulator.hpp>

#include <util/binary.hpp>

#include <array>

namespace ucle::fnsim {

    /*struct frisc_status_reg : public flags_reg<32> {
        using flags_reg<32>::operator=;
    };*/

    struct frisc_register_file : public register_file {
        std::array<reg<32>, 8> R;
        reg<32>& SP = R[7];
        reg<32> PC;
        reg<32> SR;

        void clear() override
        {
            PC = 0; SR = 0;
            for (auto i = 0u; i < R.size(); ++i)
                R[i] = 0;
        }
    };

    class frisc_simulator : public functional_simulator_impl<frisc_register_file, address_space<mapped_device_ptr>, memory> {
        using functional_simulator_impl<frisc_register_file, address_space<mapped_device_ptr>, memory>::functional_simulator_impl;

        using cbu = util::const_bit_util<word_t>;
        using unop = util::unop<word_t>;
        using binop = util::binop<word_t>;

        protected:
            virtual address_t get_program_counter_() const override { return regs_.PC.get(); };
            virtual void set_program_counter_(address_t location) override { regs_.PC = location; }

            virtual status execute_single_() override;

        private:
            status execute_move_(word_t opcode, bool fn, const reg<32>& IR);
            status execute_alu_(word_t opcode, bool fn, const reg<32>& IR);
            status execute_mem_(word_t opcode, bool fn, const reg<32>& IR);
            status execute_ctrl_(word_t opcode, bool fn, const reg<32>& IR);

            constexpr bool eval_cond_(word_t cond) const;
    };

}

#endif  /* _UCLE_CORE_SIMULATORS_FUNCTIONAL_FRISC_HPP_ */
