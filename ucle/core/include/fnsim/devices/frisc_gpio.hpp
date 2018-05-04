#ifndef _UCLE_CORE_FNSIM_DEVICES_GPIO_HPP_
#define _UCLE_CORE_FNSIM_DEVICES_GPIO_HPP_

#include <fnsim/base.hpp>
#include <fnsim/device.hpp>

#include <fnsim/processors/frisc.hpp>

namespace ucle::fnsim::frisc {

    class base_io_device {
        public:
            virtual bool ready()             { return false; }
            virtual byte_t read()            { return 0; }
            virtual void write(byte_t value) {}
    };

    class gpio : public register_set_device<0, 32, byte_order::little_endian, address32_t>{
        using parent = register_set_device<0, 32, byte_order::little_endian, address32_t>;

        enum { CR = 0, DR = 4, SR = 8, IACK = 12 };
        enum work_mode { mode_output = 0b00, mode_input = 0b01, mode_bitset = 0b10, mode_bittest = 0b11 };

        // Registers:
        //  0: Control Register
        //  4: Data Register    -> 8-bit register
        //  8: Status Register  -> 0 = no interrupt, 1 = ; write = interrupt accepted
        // 12: Interrupt Acknowledge (write only) -> Device interrupt handling done

        public:
            static constexpr unsigned address_space_size = 16;

            gpio(base_io_device* io_dev) : io_dev_ { io_dev } {}

            device_status status() override
            {
                switch (mode_()) {
                    case mode_output:
                    case mode_input:
                    case mode_bittest:
                        return (can_interrupt() && status_ && int_ack_) ? device_status::interrupt : device_status::pending;
                    case mode_bitset:
                    default:
                        return device_status::idle;
                }
            }

            void work() override
            {
                switch (mode_()) {
                    case mode_output:
                        if (io_dev_->ready())
                            status_ = true;
                        return;

                    case mode_input:
                        if (!io_dev_->ready())
                            return;

                        DR_ = io_dev_->read();
                        status_ = true;
                        return;

                    case mode_bittest:
                        DR_ = io_dev_->read();

                        if (DR_ != last_read_ && test_bits_()) {
                            status_ = true;
                            last_read_ = DR_;
                        }
                        return;

                    case mode_bitset:
                    default:
                        return;
                }
            }

            void reset() override
            {
                CR_ = 0;
                DR_ = 0;

                int_ack_ = true;
                last_read_ = 0;
            }

            bool is_worker() const override                { return true; }
            bool can_interrupt() const override            { return CR_[2]; }
            priority_t interrupt_priority() const override { return CR_[3] ? frisc_nmi : frisc_int; }

        protected:

            word_t read_word_(address_type location) override
            {
                switch (location) {
                    case CR:
                        return CR_;
                    case DR:
                        return DR_;
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
                    case DR:
                        switch (mode_()) {
                            case mode_bitset:
                                io_dev_->write(DR_ = value);
                            case mode_output:
                                io_dev_->write(DR_ = value);
                            case mode_input:
                            case mode_bittest:
                            default:
                                break;
                        }
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
            work_mode mode_() const          { return static_cast<work_mode>(CR_[{1, 0}]); }
            bool aor_() const                { return CR_[4]; }
            byte_t mask_active_bits_() const { return CR_[{23, 16}]; }
            byte_t mask_() const             { return CR_[{15, 8}]; }

            bool test_bits_()
            {
                auto val = DR_ ^ ~mask_active_bits_();
                return aor_() ? (val == mask_()) : (val & mask_() != 0);
            }

            reg<32> CR_ { 0 };
            reg<8> DR_ { 0 };

            bool status_ { false };
            bool int_ack_ { true };
            byte_t last_read_ { 0 };

            base_io_device* io_dev_;
    };
}

#endif  /* _UCLE_CORE_FNSIM_DEVICES_GPIO_HPP_ */
