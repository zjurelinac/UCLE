#ifndef _UCLE_CORE_FNSIM_DEVICES_RTC_HPP_
#define _UCLE_CORE_FNSIM_DEVICES_RTC_HPP_

#include <fnsim/base.hpp>
#include <fnsim/device.hpp>

#include <fnsim/processors/frisc.hpp>

#include <cmath>
#include <chrono>
#include <functional>

namespace ucle::fnsim::frisc {

    using namespace ucle::literals;

    class counter_timer : public register_set_device<0, 32, byte_order::little_endian, address32_t> {
        using parent = register_set_device<0, 32, byte_order::little_endian, address32_t>;

        using ticker_fn_type = std::function<bool()>;
        using zcount_fn_type = std::function<void()>;
        using reset_fn_type  = std::function<void()>;

        enum { CR = 0, DC = 4, LR = 4, SR = 8, IACK = 12 };

        // Registers:
        //  0: Control Register -> 0 = device disabled, 1 = device enabled
        //  4: Down Counter     -> 16-bit register, current counter value; write = LR (Link Register), start counter value
        //  8: Status Register  -> 0 = no interrupt, 1 = ; write = interrupt accepted
        // 12: Interrupt Acknowledge (write only) -> Device interrupt handling done

        public:
            counter_timer() {}
            counter_timer(ticker_fn_type ticker) : ticker_(ticker) {}
            counter_timer(ticker_fn_type ticker, zcount_fn_type zc_notifier) : ticker_(ticker), zc_notifier_(zc_notifier) {}
            counter_timer(ticker_fn_type ticker, zcount_fn_type zc_notifier, reset_fn_type resetter) : ticker_(ticker), zc_notifier_(zc_notifier), resetter_(resetter) {}

            void work() override
            {
                if (ticker_ && !ticker_()) return;

                DC_ -= 1;
                if (DC_ != 0) return;

                status_ = true;
                DC_ = LR_;

                if (zc_notifier_)
                    zc_notifier_();
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

                if (resetter_)
                    resetter_();
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

            ticker_fn_type ticker_ { nullptr };
            zcount_fn_type zc_notifier_ { nullptr };
            reset_fn_type  resetter_ { nullptr };
    };

    class ct_chainer {
        public:
            void zc_notify() { ticked_ = true; }

            bool should_tick()
            {
                if (!ticked_)
                    return false;

                ticked_ = false;
                return true;
            }

        private:
            bool ticked_ { false };
    };

    class approximate_freq_ticker {
        public:
            approximate_freq_ticker(frequency_t freq, frequency_t approx_proc_freq = 50_MHz)
                : freq_frac_ { static_cast<frequency_t>(static_cast<double>(approx_proc_freq) / freq + 0.5) } { fmt::print("Freq: {}\n", freq_frac_); }
            bool should_tick() { return cnt_++ % freq_frac_ == freq_frac_ - 1; }
            void reset() { cnt_ = 0; }
        private:
            frequency_t freq_frac_;
            counter_t cnt_ { 0 };
    };

    // class fixed_freq_ticker {
    //     static constexpr frequency_t max_allowed_freq = 1_MHz;
    //     static constexpr unsigned default_check_every = 1;

    //     using clock_type = std::chrono::high_resolution_clock;
    //     using time_point = clock_type::time_point;
    //     using useconds = std::chrono::nanoseconds;

    //     public:
    //         fixed_freq_ticker(frequency_t freq, unsigned check_every = default_check_every)
    //             : period_us_ { static_cast<counter_t>(static_cast<double>(1_MHz) / freq + 0.5) }, check_every_(check_every)
    //         {
    //             fmt::print("Period: {}us\n", period_us_);
    //             if (freq > max_allowed_freq)
    //                 throw impossible_value("Frequency value too large");
    //         }

    //         bool should_tick()
    //         {
    //             // if (cnt_++ % check_every_ != check_every_ - 1) return false;

    //             auto current_us = std::chrono::duration_cast<useconds>(clock_type::now() - start_).count();

    //             fmt::print_colored(fmt::BLUE, "current = {}us, last = {}us, delta = {}us, {}\n", current_us, last_us_, current_us - last_us_, current_us - last_us_ >= period_us_ ? "TICK!" : "NO TICK");
    //             if (current_us - last_us_ < period_us_) return false;

    //             last_us_ = current_us;
    //             return true;
    //         }

    //         void reset() { last_us_ = cnt_ = 0; }

    //     private:
    //         counter_t period_us_;
    //         unsigned check_every_;

    //         time_point start_ { clock_type::now() };

    //         counter_t last_us_ { 0 };
    //         counter_t cnt_ { 0 };
    // };
}

#endif  /* _UCLE_CORE_FNSIM_DEVICES_RTC_HPP_ */
