#ifndef _UCLE_CORE_FNSIM_DEVICES_RTC_HPP_
#define _UCLE_CORE_FNSIM_DEVICES_RTC_HPP_

#include <fnsim/base.hpp>
#include <fnsim/device.hpp>

#include <fnsim/processors/frisc.hpp>

namespace ucle::fnsim::frisc {

    // TODO: fix interrupt handling
    // TODO: allow different clock sources
    // TODO: notify others on countdown finish
    class counter_timer : public register_set_device<0, 32, byte_order::little_endian, address32_t> {
        using parent = register_set_device<0, 32, byte_order::little_endian, address32_t>;

        enum { CR = 0, DC = 4, LR = 4, SR = 8, IACK = 12 };

        // Registers:
        //  0: Control Register -> 0 = device disabled, 1 = device enabled
        //  4: Down Counter     -> 16-bit register, current counter value; write = LR (Link Register), start counter value
        //  8: Status Register  -> 0 = no interrupt, 1 = ; write = interrupt accepted
        // 12: Interrupt Acknowledge (write only) -> Device interrupt handling done

        public:
            void work() override
            {
                fmt::print_colored(fmt::BLUE, ".");

                DC_ -= 1;
                if (DC_ == 0) {
                    status_ = true;
                    DC_ = LR_;
                }
            }

            device_status status() override
            {
                if (status_ && can_interrupt())
                    return device_status::interrupt;
                else if (running_())
                    return device_status::pending;
                else
                    return device_status::idle;
            }

            void reset() override
            {
                CR_ = 0;
                LR_ = DC_ = 0;
                status_ = false;
            }

            bool is_worker() override       { return true; }
            bool can_interrupt() override   { return CR_[1]; }
            priority_t interrupt_priority() { return CR_[2] ? frisc_nmi : frisc_int; }

        protected:

            word_t read_word_(address_type location) const override
            {
                switch (location) {
                    case CR:
                        return CR_;
                    case DC:
                        return DC_;
                    case SR:
                        return status_;
                    case IACK:  // write-only
                    default:
                        return 0;
                }
            }

            void write_word_(address_type location, word_t value) override
            {
                switch (location) {
                    case CR:
                        CR_ = value;
                        break;
                    case DC:
                        LR_ = DC_ = value;
                        break;
                    case SR:
                        status_ = false;
                        break;
                    case IACK:  // useless (?)
                    default:
                        break;
                }
            }

        private:
            bool running_()          { return CR_[0]; }

            reg<32> CR_ { 0 };
            reg<16> LR_ { 0 };
            reg<16> DC_ { 0 };

            bool status_ { false };  // status == did count down
    };

}

#endif  /* _UCLE_CORE_FNSIM_DEVICES_RTC_HPP_ */
