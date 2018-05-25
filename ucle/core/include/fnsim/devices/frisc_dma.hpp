#ifndef _UCLE_CORE_FNSIM_DEVICES_DMA_HPP_
#define _UCLE_CORE_FNSIM_DEVICES_DMA_HPP_

#include <fnsim/base.hpp>
#include <fnsim/device.hpp>

#include <fnsim/processors/frisc.hpp>

namespace ucle::fnsim::frisc {

    template <typename ProcessorPtr>
    class dma : public register_set_device<3, 32, byte_order::little_endian, address32_t> {
        using parent = register_set_device<3, 32, byte_order::little_endian, address32_t>;

        using processor_ptr_type = ProcessorPtr;

        enum { SRC_ADDR = 0, DST_ADDR = 4, DATA_CNT = 8, CR = 12, START = 16, BS = 20 };
        enum work_mode { STOP_PROC = 0, CYCLE_STEAL = 1 };

        public:
            static constexpr unsigned address_space_size = 24;

            dma() = delete;
            dma(processor_ptr_type proc_ptr) : proc_ptr_ { proc_ptr } {}

            device_status status() override
            {
                if (status_ && can_interrupt() && mode_() == CYCLE_STEAL)
                    return device_status::interrupt;
                else if (running_)
                    return device_status::pending;
                else
                    return device_status::idle;
            }

            void work() override
            {
                if (!running_) return;

                if (mode_() == STOP_PROC) {
                    while (data_cnt_())
                        transfer_one_();

                    running_ = false;
                } else { /* == CYCLE_STEAL */
                    transfer_one_();

                    if (data_cnt_() == 0) {
                        running_ = false;
                        status_ = true;
                    }
                }
            }

            void reset() override
            {
                parent::reset();
                CR_ = 0;
                running_ = status_ = false;
            }

            virtual bool is_worker() const { return true; };
            virtual bool can_interrupt() const { return CR_[0]; }
            virtual priority_t interrupt_priority() const { return frisc_int; }

        protected:
            word_t read_word_(address_type location) override
            {
                switch (location) {
                    case CR:
                        return CR_;
                    case START:
                        return 0;
                    case BS:
                        return status_;
                    default:
                        return parent::read_word_(location);
                }
            }

            void write_word_(address_type location, word_t value) override
            {
                switch (location) {
                    case CR:
                        CR_ = value;
                        return;
                    case START:
                        running_ = true;
                        return;
                    case BS:
                        status_ = false;
                        return;
                    default:
                        parent::write_word_(location, value);
                        return;
                }
            }

        private:
            void transfer_one_()
            {
                proc_ptr_->set_mem_word(dst_addr_(), proc_ptr_->get_mem_word(src_addr_()));

                if (inc_src_addr_())
                    src_addr_() += 4;

                if (inc_dst_addr_())
                    dst_addr_() += 4;

                data_cnt_() -= 1;
            }

            auto mode_()          { return static_cast<work_mode>(CR_[1]); }
            bool inc_src_addr_()  { return !CR_[2]; }
            bool inc_dst_addr_()  { return !CR_[3]; }

            auto& src_addr_() { return reg_(SRC_ADDR); }
            auto& dst_addr_() { return reg_(DST_ADDR); }
            auto& data_cnt_() { return reg_(DATA_CNT); }

            reg<32> CR_ { 0 };

            bool running_ { false };
            bool status_ { false };

            processor_ptr_type proc_ptr_;
    };

};

#endif  /* _UCLE_CORE_FNSIM_DEVICES_DMA_HPP_ */
