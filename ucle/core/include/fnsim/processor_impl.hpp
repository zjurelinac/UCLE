#ifndef _UCLE_CORE_FNSIM_PROCESSOR_IMPL_HPP_
#define _UCLE_CORE_FNSIM_PROCESSOR_IMPL_HPP_

#include <common/meta.hpp>
#include <common/types.hpp>

#include <fnsim/address_space.hpp>
#include <fnsim/device.hpp>
#include <fnsim/exceptions.hpp>
#include <fnsim/processor.hpp>

#include <util/const_bin_util.hpp>

#include <set>
#include <string>
#include <unordered_map>

namespace ucle::fnsim {

    template <byte_order endianness,                // Is processor little- or big-endian
              typename AddressType,                 // Primitive type used for memory addressing (ie. uint32_t)
        template<byte_order, typename AddrType>
              typename MappedDeviceType,            // Base class for all devices that could be added to the simulator
        template <typename MappedDevType>
              typename AddressSpace,                // Processor's address space class
        template <byte_order, typename AddrType>
              typename Memory,                      // Processor's internal memory device class
              typename Config = processor_config    // Processor config parameters class
    >
    class functional_processor_simulator_impl : public functional_processor_simulator {
            struct device_info {
                identifier_t    id;
                device_ptr      ptr;
                device_mapping  mapping;
            };

        public:
            using address_type = AddressType;

            using mapped_device_type = MappedDeviceType<endianness, address_type>;
            using address_space_type = AddressSpace<mapped_device_type>;
            using mapped_device_ptr = typename address_space_type::mapped_device_ptr;

            using memory_type = Memory<endianness, address_type>;
            using memory_ptr = std::unique_ptr<memory_type>;

            using config_type = Config;

            functional_processor_simulator_impl(config_type cfg)
                : cfg_{cfg}, mem_asp_{cfg.mem_addr_range}, dev_asp_{cfg.dev_addr_range}
            {
                device_config mem_cfg { device_class::memory, {0, cfg.mem_size}, false, 0 };
                mem_id_ = add_device(std::make_unique<memory_type>(cfg.mem_size), mem_cfg);
            }

            ~functional_processor_simulator_impl() override { remove_device(mem_id_); }

            void reset() override
            {
                clear_internals_();
                for (auto &dev : devs_)
                    dev.second.ptr->reset();
            }

            byte_t get_byte(address_t location) const override
            {
                return read_<byte_t>(location);
            }
            void set_byte(address_t location, byte_t value) override
            {
                write_<byte_t>(location, value);
            }

            identifier_t add_device(device_ptr dev_ptr, device_config dev_cfg) override
            {
                auto mapping = [&](){
                    switch (dev_cfg.dev_class) {
                        case device_class::memory:              return device_mapping::memory;
                        case device_class::addressable_device:  return cfg_.default_mapping;
                        default:                                return device_mapping::none;
                    }
                }();

                if (mapping == device_mapping::memory)
                    mem_asp_.register_device(dynamic_cast<mapped_device_ptr>(dev_ptr.get()), dev_cfg.addr_range);
                else if (mapping == device_mapping::port)
                    dev_asp_.register_device(dynamic_cast<mapped_device_ptr>(dev_ptr.get()), dev_cfg.addr_range);

                devs_[next_dev_id_] = { next_dev_id_, std::move(dev_ptr), mapping };

                return next_dev_id_++;
            }

            void remove_device(identifier_t dev_id) override
            {
                auto dev_it = devs_.find(dev_id);
                if (dev_it == devs_.end())
                    throw invalid_identifier("Device with this ID wasn't registered.");

                const auto &info = dev_it->second;

                if (info.mapping == device_mapping::memory)
                    mem_asp_.unregister_device(dynamic_cast<mapped_device_ptr>(info.ptr.get()));
                else if (info.mapping == device_mapping::port)
                    dev_asp_.unregister_device(dynamic_cast<mapped_device_ptr>(info.ptr.get()));
            }

        protected:
            virtual void clear_internals_() = 0;

            template <typename T, typename = meta::is_storage_t<T>>
            T read_(address_t location) const { return mem_asp_.template read<T>(util::const_bin_util<T>::address_rounded(location)); }
            template <typename T, typename = meta::is_storage_t<T>>
            void write_(address_t location, T value) { mem_asp_.template write<T>(util::const_bin_util<T>::address_rounded(location), value); }

            template <typename T, typename = meta::is_storage_t<T>>
            T read_dev_(address_t location) const { return dev_asp_.template read<T>(util::const_bin_util<T>::address_rounded(location)); }
            template <typename T, typename = meta::is_storage_t<T>>
            void write_dev_(address_t location, T value) { dev_asp_.template write<T>(util::const_bin_util<T>::address_rounded(location), value); }

        private:
            config_type             cfg_;
            address_space_type      mem_asp_;
            address_space_type      dev_asp_;
            identifier_t            mem_id_;
            std::unordered_map<identifier_t, device_info> devs_;    /* Pointers and info about all used devices */
            identifier_t            next_dev_id_ = 0;               /* Next unique ID to be assigned to a device upon registration */
            // TODO: Interrupt handling

    };

}

#endif  /* _UCLE_CORE_FNSIM_PROCESSOR_IMPL_HPP_ */
