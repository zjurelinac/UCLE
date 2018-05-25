#ifndef _UCLE_CORE_FNSIM_PROCESSORS_FRISC_HPP_
#define _UCLE_CORE_FNSIM_PROCESSORS_FRISC_HPP_

#include <fnsim/base.hpp>
#include <fnsim/address_space.hpp>
#include <fnsim/device.hpp>
#include <fnsim/processor.hpp>
#include <fnsim/processor_impl.hpp>
#include <fnsim/registers.hpp>

#include <util/binary.hpp>

#include <array>

namespace ucle::fnsim::frisc {

    struct status_reg : public flags_reg<32> {
        using flags_reg<32>::operator=;

        flag_reference GIE  = operator[](4);
        flag_reference Z    = operator[](3);
        flag_reference V    = operator[](2);
        flag_reference C    = operator[](1);
        flag_reference N    = operator[](0);
    };

    // struct arith_flags : public bitfield<4> {
    //     using bitfield<4>::bitfield;

    //     constexpr arith_flags() = default;
    //     constexpr arith_flags(bool c, bool v, bool n, bool z)
    //         { set(0, n); set(1, c); set(2, v); set(3, z); }
    // };

    using arith_flags = util::basic_arith_flags;

    struct register_file : public base_register_file {
        std::array<reg<32>, 8> R;
        reg<32>& SP = R[7];
        reg<32> PC;
        status_reg SR;
        bool IIF = 0;

        void clear() override
        {
            IIF = 0;
            PC = 0; SR = 0;
            for (auto i = 0u; i < R.size(); ++i)
                R[i] = 0;
        }
    };

    enum interrupt_priorities {
        frisc_int = 1,
        frisc_nmi = 2,
        frisc_max_int_prio = 2
    };

    class frisc_simulator : public functional_processor_simulator_impl<32, byte_order::little_endian, mapped_device, address_space, memory, processor_config, false, frisc_max_int_prio> {
        using cbu = util::const_bin_util<word_t>;
        using unop = util::unop<word_t>;
        using binop = util::binop<word_t, arith_flags>;
        using parent = functional_processor_simulator_impl<32, byte_order::little_endian, mapped_device, address_space, memory, processor_config, false, frisc_max_int_prio>;

        public:
            frisc_simulator(processor_config_type cfg) : parent::functional_processor_simulator_impl(cfg)
            {
                regs_.IIF = true;
                enable_interrupt_(frisc_nmi);
            }

            address_type get_program_counter() const override { return regs_.PC; };
            void set_program_counter(address_type location) override { regs_.PC = location; }
            register_info get_reg_info() override;

        protected:
            status execute_single_() override;
            void process_interrupt_(priority_t int_prio) override;
            void clear_internals_() override { regs_.clear(); }

            void push_to_stack_(word_t value)
            {
                regs_.SP -= 4;
                write_<word_t>(regs_.SP, value);
            }

            word_t pop_from_stack_()
            {
                auto value = read_<word_t>(regs_.SP);
                regs_.SP += 4;
                return value;
            }

        private:
            status execute_move_(word_t opcode, bool fn, const reg<32>& IR);
            status execute_alu_(word_t opcode, bool fn, const reg<32>& IR);
            status execute_mem_(word_t opcode, bool fn, const reg<32>& IR);
            status execute_ctrl_(word_t opcode, bool fn, const reg<32>& IR);

            constexpr bool eval_cond_(word_t cond) const;

            register_file regs_;
    };

    functional_processor_simulator_ptr<32> make_frisc_simulator(processor_config cfg);

}

#endif  /* _UCLE_CORE_FNSIM_PROCESSORS_FRISC_HPP_ */
