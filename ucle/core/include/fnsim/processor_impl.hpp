#ifndef _UCLE_CORE_FNSIM_PROCESSOR_IMPL_HPP_
#define _UCLE_CORE_FNSIM_PROCESSOR_IMPL_HPP_

#include <fnsim/base.hpp>

#include <fnsim/address_space.hpp>
#include <fnsim/device.hpp>
#include <fnsim/processor.hpp>

#include <util/const_bin_util.hpp>

#include <string>
#include <vector>

namespace ucle::fnsim {

    template <byte_order endianness,                    // Is processor little- or big-endian
              typename AddressType,                     // Primitive type used for memory addressing (ie. uint32_t)
        template<byte_order, typename AddrType>
              typename MappedDeviceType,                // Base class for all devices that could be added to the simulator
        template <typename MappedDevType>
              typename AddressSpace                     // Processor's address space class
    >
    class device_manager {
        public:
            using address_type = AddressType;
            using address_range_type = address_range<address_type>;

            using mapped_device_type = MappedDeviceType<endianness, address_type>;
            using address_space_type = AddressSpace<mapped_device_type>;
            using mapped_device_ptr = typename address_space_type::mapped_device_ptr;

            device_manager(address_range_type addr_range) : asp_ (addr_range) {}

            void add_device(device_ptr dev_ptr, device_config dev_cfg)
            {
                asp_.register_device(dynamic_cast<mapped_device_ptr>(dev_ptr.get()), dev_cfg.addr_range);
                devices_.push_back(std::move(dev_ptr));
            }

            void reset()
            {
                for (auto &dev : devices_)
                    dev->reset();
            }

            template <typename T, typename = meta::is_storage_t<T>>
            T read(address_t location) const { return asp_.template read<T>(util::const_bin_util<T>::address_rounded(location)); }
            template <typename T, typename = meta::is_storage_t<T>>
            void write(address_t location, T value) { asp_.template write<T>(util::const_bin_util<T>::address_rounded(location), value); }

            bool is_address_valid(address_t location) { return location == util::const_bin_util<word_t>::address_rounded(location) &&  asp_.is_address_valid(location); }

        private:
            address_space_type asp_;
            std::vector<device_ptr> devices_;
    };

    template <byte_order endianness,                    // Is processor little- or big-endian
              typename AddressType,                     // Primitive type used for memory addressing (ie. uint32_t)
        template<byte_order, typename AddrType>
              typename MappedDeviceType,                // Base class for all devices that could be added to the simulator
        template <typename MappedDevType>
              typename AddressSpace,                    // Processor's address space class
        template <byte_order, typename AddrType>
              typename Memory,                          // Processor's internal memory device class
              typename Config = processor_config,       // Processor config parameters class

              typename DeviceManager = device_manager<endianness, AddressType, MappedDeviceType, AddressSpace>
    >
    class functional_processor_simulator_impl : public functional_processor_simulator {
        public:
            using address_type = AddressType;

            using mapped_device_type = MappedDeviceType<endianness, address_type>;
            using address_space_type = AddressSpace<mapped_device_type>;
            using mapped_device_ptr = typename address_space_type::mapped_device_ptr;

            using memory_type = Memory<endianness, address_type>;
            using memory_ptr = std::unique_ptr<memory_type>;

            using config_type = Config;

            using device_manager_type = DeviceManager;
            using device_manager_ptr = std::unique_ptr<device_manager_type>;

            functional_processor_simulator_impl(config_type cfg) : cfg_{cfg}
            {
                mem_manager_ = std::make_unique<device_manager_type>(cfg.mem_addr_range);

                device_config mem_cfg { device_class::memory, {0, cfg.mem_size}, false, 0 };
                mem_manager_->add_device(std::make_unique<memory_type>(cfg.mem_size), mem_cfg);

                if (!cfg.separate_device_mapping) return;

                io_manager_ = std::make_unique<device_manager_type>(cfg.dev_addr_range);
            }

            status execute_single() override { return execute_single_(); };

            void reset() override
            {
                clear_internals_();
                mem_manager_.reset();

                if (io_manager_ != nullptr)
                    io_manager_.reset();
            }

            void add_device(device_ptr dev_ptr, device_config dev_cfg) override
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

            byte_t get_mem_byte(address_t location) const override
            {
                return mem_manager_->template read<byte_t>(location);
            }
            void set_mem_byte(address_t location, byte_t value) override
            {
                mem_manager_->template write<byte_t>(location, value);
            }

            bool is_mem_address_valid(address_t location) override { return mem_manager_->is_address_valid(location); }

        protected:
            virtual status execute_single_() = 0;
            virtual void clear_internals_() = 0;

            template <typename T, typename = meta::is_storage_t<T>>
            T read_(address_t location) const { return mem_manager_->template read<T>(location); }
            template <typename T, typename = meta::is_storage_t<T>>
            void write_(address_t location, T value) { mem_manager_->template write<T>(location, value); }

            template <typename T, typename = meta::is_storage_t<T>>
            T io_read_(address_t location) const { return io_manager_->template read<T>(location); }
            template <typename T, typename = meta::is_storage_t<T>>
            void io_write_(address_t location, T value) { io_manager_->template write<T>(location, value); }

        private:
            config_type cfg_;

            device_manager_ptr mem_manager_ = nullptr;
            device_manager_ptr io_manager_ = nullptr;
    };

}

#endif  /* _UCLE_CORE_FNSIM_PROCESSOR_IMPL_HPP_ */
