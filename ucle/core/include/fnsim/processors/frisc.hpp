#ifndef _UCLE_CORE_FNSIM_PROCESSORS_FRISC_HPP_
#define _UCLE_CORE_FNSIM_PROCESSORS_FRISC_HPP_

#include <common/types.hpp>

#include <fnsim/address_space.hpp>
#include <fnsim/base.hpp>
#include <fnsim/device.hpp>
#include <fnsim/processor.hpp>
#include <fnsim/processor_impl.hpp>
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
        using bitfield<4>::bitfield;

        constexpr frisc_arith_flags() = default;
        constexpr frisc_arith_flags(bool c, bool v, bool n, bool z)
            { set(0, n); set(1, c); set(2, v); set(3, z); }
    };

    struct frisc_register_file : public register_file {
        std::array<reg<32>, 8> R;
        reg<32>& SP = R[7];
        reg<32> PC;
        frisc_status_reg SR;
        bool IIF = 0;

        void clear() override
        {
            IIF = 0;
            PC = 0; SR = 0;
            for (auto i = 0u; i < R.size(); ++i)
                R[i] = 0;
        }
    };

    class frisc_simulator : public functional_processor_simulator_impl<byte_order::little_endian, address_t, mapped_device, address_space, memory> {
        using cbu = util::const_bin_util<word_t>;
        using unop = util::unop<word_t>;
        using binop = util::binop<word_t, frisc_arith_flags>;
        using parent = functional_processor_simulator_impl<byte_order::little_endian, address_t, mapped_device, address_space, memory>;

        public:
            using parent::functional_processor_simulator_impl;

            address_t get_program_counter() const override { return regs_.PC; };
            void set_program_counter(address_t location) override { regs_.PC = location; }
            status execute_single() override;
            register_info get_reg_info() override;

        protected:
            void clear_internals_() override { regs_.clear(); }

        private:
            status execute_move_(word_t opcode, bool fn, const reg<32>& IR);
            status execute_alu_(word_t opcode, bool fn, const reg<32>& IR);
            status execute_mem_(word_t opcode, bool fn, const reg<32>& IR);
            status execute_ctrl_(word_t opcode, bool fn, const reg<32>& IR);

            constexpr bool eval_cond_(word_t cond) const;

            frisc_register_file regs_;
    };

    functional_processor_simulator_ptr make_frisc_simulator(processor_config cfg);

}

#endif  /* _UCLE_CORE_FNSIM_PROCESSORS_FRISC_HPP_ */
