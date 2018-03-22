#ifndef _UCLE_CORE_FNSIM_ADDRESS_SPACE_HPP_
#define _UCLE_CORE_FNSIM_ADDRESS_SPACE_HPP_

#include <algorithm>
#include <set>
#include <utility>

#include <common/exceptions.hpp>
#include <common/types.hpp>
#include <fnsim/fnsim.hpp>

namespace ucle::fnsim {

    template <typename MappedDevicePointer>
    class address_space {
        public:
            using mapped_device_type = typename MappedDevicePointer::element_type;
            using mapped_device_ptr = MappedDevicePointer;
            using mapped_device_info = std::pair<address_range, mapped_device_ptr>;

            address_space(address_range total_range) : total_range_(total_range) {}

            void register_device(mapped_device_ptr dev_ptr, address_range range) {
                if (!total_range_.contains(range))
                    throw invalid_address_range("Device address range overflows the available address space.");

                devices_.emplace(range, dev_ptr);
            }
            void unregister_device(mapped_device_ptr dev_ptr) {
                auto dev_it = std::find_if(devices_.cbegin(), devices_.cend(),
                    [dev_ptr](auto mapped){ return mapped.second == dev_ptr; });

                if (dev_it == devices_.cend())
                    throw invalid_identifier("This device wasn't registered in the address space.");

                devices_.erase(dev_it);
            }

            byte_t read_byte(address_t location) { auto dev_ptr = find_device_(location); return dev_ptr->read_byte(location); }
            half_t read_half(address_t location) { auto dev_ptr = find_device_(location); return dev_ptr->read_half(location); }
            word_t read_word(address_t location) { auto dev_ptr = find_device_(location); return dev_ptr->read_word(location); }

            void write_byte(address_t location, byte_t value) { auto dev_ptr = find_device_(location); dev_ptr->write_byte(location, value); }
            void write_half(address_t location, half_t value) { auto dev_ptr = find_device_(location); dev_ptr->write_half(location, value); }
            void write_word(address_t location, word_t value) { auto dev_ptr = find_device_(location); dev_ptr->write_word(location, value); }

        protected:

            mapped_device_ptr find_device_(address_t location) {
                auto dev_it = std::find_if(devices_.cbegin(), devices_.cend(),
                    [location](auto mapped){ return mapped.first.contains(location); });

                if (dev_it != devices_.cend())
                    return dev_it->second;
                else
                    throw invalid_memory_access("No memory/device mapped to this address!");
            }

            address_range total_range_;
            std::set<mapped_device_info> devices_;
    };
}

#endif  /* _UCLE_CORE_FNSIM_ADDRESS_SPACE_HPP_ */
