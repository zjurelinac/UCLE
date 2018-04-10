#ifndef _UCLE_CORE_FNSIM_ADDRESS_SPACE_HPP_
#define _UCLE_CORE_FNSIM_ADDRESS_SPACE_HPP_

#include <common/meta.hpp>
#include <common/types.hpp>

#include <fnsim/exceptions.hpp>

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

namespace ucle::fnsim {

    template <typename MappedDeviceType>
    class address_space {
        public:
            using mapped_device_type = MappedDeviceType;
            using mapped_device_ptr = mapped_device_type*;
            using mapped_device_info = std::pair<address_range, mapped_device_ptr>;

            address_space() = delete;
            address_space(address_range total_range) : total_range_(total_range) {}

            address_space(const address_space<MappedDeviceType>&) = delete;
            address_space& operator=(const address_space<MappedDeviceType>&) = delete;

            address_space(address_space<MappedDeviceType>&&) = default;
            address_space& operator=(address_space<MappedDeviceType>&&) = default;

            ~address_space() = default;

            void register_device(mapped_device_ptr dev_ptr, address_range range)
            {
                if (!total_range_.contains(range))
                    throw invalid_address_range("Device address range overflows the available address space.");

                devices_.emplace_back(range, dev_ptr);
            }
            void unregister_device(mapped_device_ptr dev_ptr)
            {
                auto dev_it = std::find_if(devices_.cbegin(), devices_.cend(),
                    [dev_ptr](auto mapped){ return mapped.second == dev_ptr; });

                if (dev_it == devices_.cend())
                    throw invalid_identifier("This device wasn't registered in the address space.");

                devices_.erase(dev_it);
            }

            template <typename T, typename = meta::is_storage_t<T>>
            T read(address_t location) const
            {
                auto [dev_range, dev_ptr] = find_device_(location);
                return dev_ptr->template read<T>(location - dev_range.low_addr);
            }

            template <typename T, typename = meta::is_storage_t<T>>
            void write(address_t location, T value)
            {
                auto [dev_range, dev_ptr] = find_device_(location);
                return dev_ptr->template write<T>(location - dev_range.low_addr, value);
            }

        protected:
            auto find_device_(address_t location) const
            {
                for (const auto& dev : devices_)
                    if (dev.first.contains(location))
                        return dev;
                
                throw invalid_memory_access("No memory/device mapped to this address!");
            }

        private:
            address_range total_range_ = {0, 0};
            std::vector<mapped_device_info> devices_ = {};
    };
}

#endif  /* _UCLE_CORE_FNSIM_ADDRESS_SPACE_HPP_ */
