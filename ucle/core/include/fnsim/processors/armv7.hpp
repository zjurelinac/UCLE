#ifndef _UCLE_CORE_FNSIM_PROCESSORS_ARMV7_HPP_
#define _UCLE_CORE_FNSIM_PROCESSORS_ARMV7_HPP_

#include <fnsim/base.hpp>
#include <fnsim/address_space.hpp>
#include <fnsim/device.hpp>
#include <fnsim/processor.hpp>
#include <fnsim/processor_impl.hpp>
#include <fnsim/registers.hpp>

#include <util/binary.hpp>

#include <array>

namespace ucle::fnsim::armv7 {

    struct status_reg : public flags_reg<32> {
        using flags_reg<32>::operator=;

        flag_reference N    = operator[](31);
        flag_reference Z    = operator[](30);
        flag_reference C    = operator[](29);
        flag_reference V    = operator[](28);

        flag_reference I    = operator[](7);
        flag_reference F    = operator[](6);

        byte_t mode() { return (*this)[{5, 0}]; }
    };

    using arith_flags = util::basic_arith_flags;

    struct register_file : public base_register_file {
        using reg_type = reg<32>;

        reg_type CPSR;

        auto& R(unsigned idx) 
        {
            if (idx < 13) return R[idx];
            return usrR134[idx - 13];
        }

        auto& SPSR() {
            return mSPSR_[0];
        }

        auto& PC() { return R(15); }
        auto& LR() { return R(14); }

        void clear() override
        {
            for (auto i = 0u; i < R_.size(); ++i)
                R_[i] = 0;

            for (auto i = 0u; i < R_.size(); ++i)
                mSPSR_[i] = 0;

            usrR134_[0] = usrR134_[1] = svcR134_[0] = svcR134_[1] =
            abtR134_[0] = abtR134_[1] = udfR134_[0] = udfR134_[1] =
            irqR134_[0] = irqR134_[1] = fiqR134_[0] = fiqR134_[1] = 0;
        }

        private:
            std::array<reg_type> R_[13];
            
            std::array<reg_type> usrR134_[2];
            std::array<reg_type> svcR134_[2];
            std::array<reg_type> abtR134_[2];
            std::array<reg_type> udfR134_[2];
            std::array<reg_type> irqR134_[2];
            std::array<reg_type> fiqR134_[2];

            std::array<reg_type> mSPSR_[6];
    };

    class armv7_simulator : public functional_processor_simulator_impl<32, byte_order::little_endian, mapped_device, address_space, memory, processor_config, false, frisc_max_int_prio> {
        using cbu = util::const_bin_util<word_t>;
        using unop = util::unop<word_t>;
        using binop = util::binop<word_t, arith_flags>;
        using parent = functional_processor_simulator_impl<32, byte_order::little_endian, mapped_device, address_space, memory, processor_config, false, frisc_max_int_prio>;

        public:
            armv7_simulator(processor_config_type cfg) : parent::functional_processor_simulator_impl(cfg)
            {
                // regs_.IIF = true;
                // enable_interrupt_(frisc_nmi);
            }

            // address_type get_program_counter() const override { return regs_.PC; };
            // void set_program_counter(address_type location) override { regs_.PC = location; }
            // register_info get_reg_info() override;

        protected:
            // status execute_single_() override;
            // void process_interrupt_(priority_t int_prio) override;
            // void clear_internals_() override { regs_.clear(); }

            // void push_to_stack_(word_t value)
            // {
            //     regs_.SP -= 4;
            //     write_<word_t>(regs_.SP, value);
            // }

            // word_t pop_from_stack_()
            // {
            //     auto value = read_<word_t>(regs_.SP);
            //     regs_.SP += 4;
            //     return value;
            // }

        private:
            // status execute_move_(word_t opcode, bool fn, const reg<32>& IR);
            // status execute_alu_(word_t opcode, bool fn, const reg<32>& IR);
            // status execute_mem_(word_t opcode, bool fn, const reg<32>& IR);
            // status execute_ctrl_(word_t opcode, bool fn, const reg<32>& IR);

            constexpr bool eval_cond_(word_t cond) const;

            register_file regs_;
    };

    functional_processor_simulator_ptr<32> make_armv7_simulator(processor_config cfg);
};

#endif  /* _UCLE_CORE_FNSIM_PROCESSORS_ARMV7_HPP_ */
