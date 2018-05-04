#ifndef _UCLE_CORE_FNSIM_PROCESSOR_IMPL_HPP_
#define _UCLE_CORE_FNSIM_PROCESSOR_IMPL_HPP_

#include <fnsim/base.hpp>

#include <fnsim/device.hpp>
#include <fnsim/processor.hpp>

#include <util/const_bin_util.hpp>

#include <string>
#include <vector>

namespace ucle::fnsim {

    template <byte_order endianness,                    // Is processor little- or big-endian
              unsigned N,                               // Processor architecture - 8, 16, 32 or 64 bits
        template<byte_order, typename AddrType>
              typename MappedDeviceType,                // Base class for all devices that could be added to the simulator
        template <unsigned AddrSpN, typename MappedDevType>
              typename AddressSpace,                    // Processor's address space class

              priority_t max_int_prio = 0               // Maximum processor interrupt level (0 = no interrupts)
    >
    class device_manager {
        public:
            using address_type = meta::arch_address_t<N>;
            using address_range_type = address_range<address_type>;

            using mapped_device_type = MappedDeviceType<endianness, address_type>;
            using address_space_type = AddressSpace<N, mapped_device_type>;
            using mapped_device_ptr = typename address_space_type::mapped_device_ptr;

            using device_config_type = device_config<address_type>;

            using bitfield_type = bitfield<max_int_prio>;

            using cbu = util::const_bin_util<address_type>;

        public:
            device_manager(address_range_type addr_range) : asp_ (addr_range) {}

            void add_device(device_ptr dev_ptr, device_config_type dev_cfg)
            {
                asp_.register_device(dynamic_cast<mapped_device_ptr>(dev_ptr.get()), { dev_cfg.start_address, dev_cfg.start_address + dev_cfg.addr_space_size - 1 });

                if (dev_ptr->is_worker())
                    workers_.push_back(dev_ptr.get());

                devices_.push_back(std::move(dev_ptr));
            }

            void do_work()
            {
                active_ints_ = 0;
                for (const auto& worker : workers_) {
                    switch(worker->status()) {
                        case device_status::interrupt:
                            active_ints_[worker->interrupt_priority()] = true;
                            [[fallthrough]];
                        case device_status::pending:
                            worker->work();
                            break;
                        case device_status::idle:
                        default:
                            break;
                    }
                }
            }

            void do_reset()
            {
                for (auto &dev : devices_)
                    dev->reset();
            }

            bool has_interrupt() { return (active_ints_ & enabled_ints_).any(); }

            priority_t get_max_interrupt()
            {
                for (auto i = max_int_prio; i > 0; --i)
                    if (active_ints_[i])
                        return i;

                return 0;
            }

            template <typename T, typename = meta::is_storage_t<T>>
            T read(address_type location) const { return asp_.template read<T>(cbu::address_rounded(location, sizeof(T))); }
            template <typename T, typename = meta::is_storage_t<T>>
            void write(address_type location, T value) { asp_.template write<T>(cbu::address_rounded(location, sizeof(T)), value); }

            bool is_address_valid(address_type location) { return location == cbu::address_rounded(location, sizeof(address_type)) &&  asp_.is_address_valid(location); }

            void enable_interrupt(priority_t int_prio) { enabled_ints_[int_prio] = true; }
            void disable_interrupt(priority_t int_prio) { enabled_ints_[int_prio] = false; }

        private:
            address_space_type asp_;
            std::vector<device_ptr> devices_;
            std::vector<device*> workers_;

            bitfield_type enabled_ints_ {0};
            bitfield_type active_ints_ {0};

            counter_t cycle_cnt_ {0};
    };

    template <unsigned N,                               // Processor architecture - 8, 16, 32 or 64 bits
              byte_order endianness,                    // Is processor little- or big-endian
        template<byte_order, typename AddrType>
              typename MappedDeviceType,                // Base class for all devices that could be added to the simulator
        template <unsigned AddrSpN, typename MappedDevType>
              typename AddressSpace,                    // Processor's address space class
        template <byte_order, typename AddrType>
              typename Memory,                          // Processor's internal memory device class
              typename Config = processor_config,       // Processor config parameters class

              bool separate_device_mapping = false,     // Does processor use separate IO device address space
              priority_t max_int_prio = 0,              // Maximum processor interrupt level (0 = no interrupts)

              typename DeviceManager = device_manager<endianness, N, MappedDeviceType, AddressSpace, max_int_prio>
    >
    class functional_processor_simulator_impl : public functional_processor_simulator<N> {
        public:
            using address_type = meta::arch_address_t<N>;

            using mapped_device_type = MappedDeviceType<endianness, address_type>;
            using address_space_type = AddressSpace<N, mapped_device_type>;
            using mapped_device_ptr = typename address_space_type::mapped_device_ptr;

            using memory_type = Memory<endianness, address_type>;
            using memory_ptr = std::unique_ptr<memory_type>;

            using processor_config_type = Config;
            using device_config_type = device_config<address_type>;

            using device_manager_type = DeviceManager;
            using device_manager_ptr = std::unique_ptr<device_manager_type>;

            functional_processor_simulator_impl(processor_config_type cfg) : cfg_{cfg}
            {
                mem_manager_ = std::make_unique<device_manager_type>(cfg.mem_addr_range);

                device_config_type mem_cfg { 0, cfg.mem_size, device_class::memory };
                mem_manager_->add_device(std::make_unique<memory_type>(cfg.mem_size), mem_cfg);

                if constexpr (separate_device_mapping)
                    io_manager_ = std::make_unique<device_manager_type>(cfg.dev_addr_range);
            }

            status execute_single() override
            {
                auto stat = execute_single_();

                mem_manager_->do_work();

                if (mem_manager_->has_interrupt())
                    process_interrupt_(mem_manager_->get_max_interrupt());

                // if (io_manager_ != nullptr)
                //     io_manager_->do_work();

                return stat;
            };

            void reset() override
            {
                clear_internals_();
                mem_manager_->do_reset();

                // if (io_manager_ != nullptr)
                //     io_manager_->do_reset();
            }

            void add_device(device_ptr dev_ptr, device_config_type dev_cfg) override
            {
                auto mapping = [&](){
                    switch (dev_cfg.dev_class) {
                        case device_class::memory:              return device_mapping::memory;
                        case device_class::addressable_device:  return cfg_.default_mapping;
                        default:                                return device_mapping::none;
                    }
                }();

                if (mapping == device_mapping::memory)
                    mem_manager_->add_device(std::move(dev_ptr), dev_cfg);
                else if (mapping == device_mapping::port)
                    io_manager_->add_device(std::move(dev_ptr), dev_cfg);
            }

            byte_t get_mem_byte(address_type location) const override { return mem_manager_->template read<byte_t>(location); }
            half_t get_mem_half(address_type location) const override { return mem_manager_->template read<half_t>(location); }
            word_t get_mem_word(address_type location) const override { return mem_manager_->template read<word_t>(location); }

            void set_mem_byte(address_type location, byte_t value) override { mem_manager_->template write<byte_t>(location, value); }
            void set_mem_half(address_type location, half_t value) override { mem_manager_->template write<half_t>(location, value); }
            void set_mem_word(address_type location, word_t value) override { mem_manager_->template write<word_t>(location, value); }

            bool is_mem_address_valid(address_type location) override { return mem_manager_->is_address_valid(location); }

        protected:
            virtual status execute_single_() = 0;
            virtual void process_interrupt_(priority_t int_prio) = 0;
            virtual void clear_internals_() = 0;

            template <typename T, typename = meta::is_storage_t<T>>
            T read_(address_type location) const { return mem_manager_->template read<T>(location); }
            template <typename T, typename = meta::is_storage_t<T>>
            void write_(address_type location, T value) { mem_manager_->template write<T>(location, value); }

            template <typename T, typename = meta::is_storage_t<T>>
            T io_read_(address_type location) const { return io_manager_->template read<T>(location); }
            template <typename T, typename = meta::is_storage_t<T>>
            void io_write_(address_type location, T value) { io_manager_->template write<T>(location, value); }

            void enable_interrupt_(priority_t prio)
            {
                mem_manager_->enable_interrupt(prio);

                if constexpr (separate_device_mapping)
                    io_manager_->enable_interrupt(prio);
            }

            void disable_interrupt_(priority_t prio)
            {
                mem_manager_->disable_interrupt(prio);

                if constexpr (separate_device_mapping)
                    io_manager_->disable_interrupt(prio);
            }

        private:
            processor_config_type cfg_;

            device_manager_ptr mem_manager_ = nullptr;
            device_manager_ptr io_manager_ = nullptr;
    };

}

#endif  /* _UCLE_CORE_FNSIM_PROCESSOR_IMPL_HPP_ */
