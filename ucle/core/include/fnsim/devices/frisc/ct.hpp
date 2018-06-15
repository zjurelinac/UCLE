#ifndef _UCLE_CORE_FNSIM_DEVICES_RTC_HPP_
#define _UCLE_CORE_FNSIM_DEVICES_RTC_HPP_

#include <fnsim/base.hpp>
#include <fnsim/device.hpp>

#include <fnsim/processors/frisc.hpp>

namespace ucle::fnsim::frisc {

    using namespace ucle::literals;

    class base_tick_generator {
        public:
            virtual bool should_tick() { return false; }
            virtual void reset() {}
    };

    class base_countdown_notifier {
        public:
            virtual void notify() {}
    };

    class counter_timer : public register_set_device<0, 32, byte_order::little_endian, address32_t> {
        using parent = register_set_device<0, 32, byte_order::little_endian, address32_t>;

        enum { CR = 0, DC = 4, LR = 4, SR = 8, IACK = 12 };

        // Registers:
        //  0: Control Register -> 0 = device disabled, 1 = device enabled
        //  4: Down Counter     -> 16-bit register, current counter value; write = LR (Link Register), start counter value
        //  8: Status Register  -> 0 = no interrupt, 1 = ; write = interrupt accepted
        // 12: Interrupt Acknowledge (write only) -> Device interrupt handling done

        public:
            static constexpr unsigned address_space_size = 16;

            counter_timer() {}
            counter_timer(base_tick_generator* ticker) : ticker_(ticker) {}
            counter_timer(base_tick_generator* ticker, base_countdown_notifier* notifier) : ticker_(ticker), notifier_(notifier) {}

            device_status status() override
            {
                if (can_interrupt() && status_ && int_ack_)
                    return device_status::interrupt;
                else if (running_())
                    return device_status::pending;
                else
                    return device_status::idle;
            }

            void work() override
            {
                if (ticker_ && !ticker_->should_tick()) return;

                DC_ -= 1;
                if (DC_ != 0) return;

                status_ = true;
                DC_ = LR_;

                if (notifier_)
                    notifier_->notify();
            }

            void reset() override
            {
                CR_ = 0;
                LR_ = DC_ = 0;
                status_ = false;
                int_ack_ = true;

                if (ticker_)
                    ticker_->reset();
            }

            bool is_worker() const override                { return true; }
            bool can_interrupt() const override            { return CR_[1]; }
            priority_t interrupt_priority() const override { return CR_[2] ? frisc_nmi : frisc_int; }

        protected:

            word_t read_word_(address_type location) override
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
                        int_ack_ = status_ = false;
                        break;
                    case IACK:
                        int_ack_ = true;
                        break;
                    default:
                        break;
                }
            }

        private:
            bool running_() const { return CR_[0]; }

            reg<32> CR_ { 0 };
            reg<16> LR_ { 0 };
            reg<16> DC_ { 0 };

            bool status_ { false };  // status == did count down
            bool int_ack_ { true };

            base_tick_generator* ticker_ { nullptr };
            base_countdown_notifier* notifier_ { nullptr };
    };

    class ct_chainer : public base_tick_generator, public base_countdown_notifier {
        public:
            bool should_tick() override
            {
                if (!ticked_) return false;

                ticked_ = false;
                return true;
            }
            void notify() { ticked_ = true; }
            void reset() override { ticked_ = true; }

        private:
            bool ticked_ { false };
    };

    class approximate_freq_ticker : public base_tick_generator {
        public:
            approximate_freq_ticker(frequency_t freq, frequency_t approx_proc_freq = 50_MHz)
                : freq_frac_ { static_cast<frequency_t>(static_cast<double>(approx_proc_freq) / freq + 0.5) } { fmt::print("Freq: {}\n", freq_frac_); }
            bool should_tick() override { return cnt_++ % freq_frac_ == freq_frac_ - 1; }
            void reset() override { cnt_ = 0; }
        private:
            frequency_t freq_frac_;
            counter_t cnt_ { 0 };
    };

}

#endif  /* _UCLE_CORE_FNSIM_DEVICES_RTC_HPP_ */
