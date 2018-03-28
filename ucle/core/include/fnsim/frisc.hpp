#ifndef _UCLE_CORE_FNSIM_FRISC_HPP_
#define _UCLE_CORE_FNSIM_FRISC_HPP_

#include <common/types.hpp>

#include <fnsim/fnsim.hpp>
#include <fnsim/address_space.hpp>
#include <fnsim/basic_devices.hpp>
#include <fnsim/fnsim_impl.hpp>
#include <fnsim/registers.hpp>

#include <util/binary.hpp>

#include <array>

namespace ucle::fnsim {

    struct frisc_status_reg : public flags_reg<32> {
        using flags_reg<32>::operator=;

        flag_reference GIE  = operator[](4);
        flag_reference Z    = operator[](3);
        flag_reference V    = operator[](2);
        flag_reference C    = operator[](1);
        flag_reference N    = operator[](0);
    };

    struct frisc_arith_flags : public bitfield<4> {
        using parent = bitfield<4>;
        using parent::bitfield;

        parent::reference Z = operator[](3);
        parent::reference V = operator[](2);
        parent::reference C = operator[](1);
        parent::reference N = operator[](0);

        constexpr frisc_arith_flags(bool c, bool v, bool n, bool z)
            { C = c; V = v; N = n; Z = z; }
    };

    struct frisc_register_file : public register_file {
        std::array<reg<32>, 8> R;
        reg<32>& SP = R[7];
        reg<32> PC;
        frisc_status_reg SR;

        void clear() override
        {
            PC = 0; SR = 0;
            for (auto i = 0u; i < R.size(); ++i)
                R[i] = 0;
        }
    };

    class frisc_simulator : public functional_simulator_impl<byte_order::LE, address_space<mapped_device_ptr<byte_order::LE>>, memory> {
        using cbu = util::const_bit_util<word_t>;
        using unop = util::unop<word_t>;
        using binop = util::binop<word_t, frisc_arith_flags>;
        using parent = functional_simulator_impl<byte_order::LE, address_space<mapped_device_ptr>, memory>;

        public:
            using parent::functional_simulator_impl;

        protected:
            virtual address_t get_program_counter_() const override { return regs_.PC; };
            virtual void set_program_counter_(address_t location) override { regs_.PC = location; }

            virtual void clear_internals_() override { regs_.clear(); }

            virtual status execute_single_() override;

        private:
            status execute_move_(word_t opcode, bool fn, const reg<32>& IR);
            status execute_alu_(word_t opcode, bool fn, const reg<32>& IR);
            status execute_mem_(word_t opcode, bool fn, const reg<32>& IR);
            status execute_ctrl_(word_t opcode, bool fn, const reg<32>& IR);

            constexpr bool eval_cond_(word_t cond) const;

            frisc_register_file            regs_;          /* Internal register file keeping the state of all registers and flags */
    };

}

#endif  /* _UCLE_CORE_FNSIM_FRISC_HPP_ */
