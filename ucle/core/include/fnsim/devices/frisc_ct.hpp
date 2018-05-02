#ifndef _UCLE_CORE_FNSIM_DEVICES_RTC_HPP_
#define _UCLE_CORE_FNSIM_DEVICES_RTC_HPP_

#include <fnsim/base.hpp>
#include <fnsim/device.hpp>

namespace ucle::fnsim::frisc {

    // TODO: fix interrupt handling
    class counter_timer : public register_set_device<4, 32, byte_order::little_endian, address32_t> {
        using parent = register_set_device<4, 32, byte_order::little_endian, address32_t>;

        enum { CR = 0, LR = 4, SR = 8, IACK = 12 };

        // Registers:
        //  0: Control Register -> 0 = device disabled, 1 = device enabled
        //  4: L
        //  8: Status Register -> 0 = , 1 =
        // 12: Interrupt Acknowledge (write only)

        public:
            void work() override
            {
                fmt::print_colored(fmt::BLUE, ".");
            }

            device_status status() override
            {
                if (is_running_())
                    return device_status::pending;
                else
                    return device_status::idle;
            }

            bool is_worker() override { return true; }

        protected:

            void write_word_(address_type location, word_t value)
            {
                parent::write<word_t>(location, value);

                switch (location) {
                    case CR:
                        break;
                    case LR:
                        break;
                    case SR:
                        break;
                    case IACK:
                        break;
                    default:
                        break;
                }
            }

        private:

            bool is_running_() { return reg_(CR)[0]; }
    };

}

#endif  /* _UCLE_CORE_FNSIM_DEVICES_RTC_HPP_ */
