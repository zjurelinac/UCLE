#ifndef _UCLE_CORE_FNSIM_ADDRESS_SPACE_HPP_
#define _UCLE_CORE_FNSIM_ADDRESS_SPACE_HPP_

#include <fnsim/base.hpp>

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

namespace ucle::fnsim {

    template <unsigned N, typename MappedDeviceType>
    class address_space {
        using self_type = address_space<N, MappedDeviceType>;

        public:
            using address_type = meta::arch_address_t<N>;
            using address_range_type = address_range<address_type>;

            using mapped_device_type = MappedDeviceType;
            using mapped_device_ptr = mapped_device_type*;
            using mapped_device_info = std::pair<address_range_type, mapped_device_ptr>;

            address_space()                              = delete;
            address_space(address_range_type total_range) : total_range_{total_range} {}

            address_space(const self_type&)              = delete;
            address_space& operator=(const self_type&)   = delete;

            address_space(self_type&&)                   = default;
            address_space& operator=(self_type&&)        = default;

            ~address_space()                             = default;

            void register_device(mapped_device_ptr dev_ptr, address_range_type range)
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
            T read(address_type location) const
            {
                auto [dev_range, dev_ptr] = find_device_(location);
                return dev_ptr->template read<T>(location - dev_range.low_addr);
            }

            template <typename T, typename = meta::is_storage_t<T>>
            void write(address_type location, T value)
            {
                auto [dev_range, dev_ptr] = find_device_(location);
                return dev_ptr->template write<T>(location - dev_range.low_addr, value);
            }

            bool is_address_valid(address_type location)
            {
                for (const auto& dev : devices_)
                    if (dev.first.contains(location))
                        return true;

                return false;
            }

        protected:
            auto find_device_(address_type location) const
            {
                for (const auto& dev : devices_)
                    if (dev.first.contains(location))
                        return dev;

                throw invalid_memory_access("No memory/device mapped to this address!");
            }

        private:
            address_range_type total_range_ = {0, 0};
            std::vector<mapped_device_info> devices_ = {};
    };
}

#endif  /* _UCLE_CORE_FNSIM_ADDRESS_SPACE_HPP_ */
