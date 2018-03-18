#ifndef _UCLE_CORE_SIMULATORS_FUNCTIONAL_COMPONENTS_HPP_
#define _UCLE_CORE_SIMULATORS_FUNCTIONAL_COMPONENTS_HPP_

#include <algorithm>
#include <cassert>
#include <cstring>
#include <memory>
#include <vector>

#include <common/types.hpp>


namespace ucle {
namespace fnsim {

    /*** Registers ***/

    // Helper templated class for representing processor registers
    template<typename ValueType>
    class register_inst_ {
        public:
            using value_type = ValueType;

            register_inst_() : value_(0) {}
            register_inst_(value_type value) : value_(value) {}
            register_inst_(const register_inst_<value_type>& other) : value_(other.value_) {}
            register_inst_(register_inst_<value_type>&& other) : value_(other.value_) {}

            // register_inst_<value_type>& operator&() { return &value_; };
            // value_type& operator&() { return &value_; };

            register_inst_<value_type>& operator=(const value_type& value)
                { value_ = value; return *this; }
            register_inst_<value_type>& operator=(const register_inst_<value_type>& other)
                { if (this != &other) value_ = other.value; return *this; }
            register_inst_<value_type>& operator=(register_inst_<value_type>&& other)
                { if (this != &other) value_ = other.value; return *this; }

            void set(value_type value) { value_ = value; }
            value_type get() { return value_; }

        protected:
            value_type value_;
    };

    // Class representing a processor register
    template<unsigned bits> struct register_inst {};
    template<>              struct register_inst<8>  : public register_inst_<byte_t> { using register_inst_<byte_t>::operator=; };
    template<>              struct register_inst<16> : public register_inst_<half_t> { using register_inst_<half_t>::operator=; };
    template<>              struct register_inst<32> : public register_inst_<word_t> { using register_inst_<word_t>::operator=; };

    // TODO: Flags register

    // Abstract base class for all processor register files
    struct register_file {
        virtual void clear() = 0;
    };

    /*** Devices ***/

    // Memory layout enum - options are LittleEndian and BigEndian
    enum class endianness { LE, BE };

    // Device mapping type enum - options are Memory-Mapped and Port-Mapped
    enum class mapping_type { MEMORY, PORT };

    // Device status enum - TODO: Select possible options
    enum class device_status {};

    // Abstract base class for all devices, both memory-mapped and isolated
    struct device {
        virtual void work() = 0;
        virtual void status() = 0;
        virtual void reset() = 0;
    };

    using device_ptr = std::shared_ptr<device>;
    using device_id = uint32_t;

    struct memory_mapped_device : public device {
        // get/set memory
    };

    using memory_mapped_device_ptr = std::shared_ptr<memory_mapped_device>;

    // Generic memory-block based device?
    class memory_block_device : public memory_mapped_device {
        // TODO: Implement
    };

    // Generic register-group based device
    class register_group_device : public memory_mapped_device {
        // TODO: Implement
    };

    // Basic RAM memory device representation - TODO: Endianness support?
    class memory : public memory_block_device {
        public:
            memory(size_t memory_size) : size_(memory_size) { data_ = new byte_t[size_]; reset(); }
            ~memory() { delete[] data_; }

            virtual void work() override {}
            virtual void reset() override { memset(data_, 0, size_); }

            template <typename T> T get(address_t location) { assert("Incorrect return type!"); };
            template <typename T> void set(address_t location, T value) { assert("Incorrect return type!"); };

        private:
            size_t size_;
            byte_t *data_;
    };
    /*
    template<> byte_t memory::get<byte_t>(address_t location) { return data_[location]; }
    template<> half_t memory::get<half_t>(address_t location) { return (data_[location + 1] << 8) | data_[location]; }
    template<> word_t memory::get<word_t>(address_t location) { return (data_[location + 3] << 24) | (data_[location + 2] << 16) | (data_[location + 1] << 8) | data_[location]; }

    template<> void memory::set<byte_t>(address_t location, byte_t value) { data_[location] = value; }
    template<> void memory::set<half_t>(address_t location, half_t value) { data_[location + 1] = value >> 8; data_[location] = value & 0xFF; }
    template<> void memory::set<word_t>(address_t location, word_t value) { data_[location + 3] = value >> 24; data_[location + 2] = (value >> 16) & 0xFF; data_[location + 1] = (value >> 8) & 0xFF; data_[location] = value & 0xFF; }
    */

    // TODO: Come up with error type for this

    //
    template <typename DevicePtr = memory_mapped_device_ptr, endianness Layout = endianness::LE>
    class address_space {
        using device_ptr = DevicePtr;
        using mapped_device = std::pair<address_range, device_ptr>;

        public:
            address_space(address_range total_range) : total_range_(total_range) {}

            bool register_device(device_ptr dev_ptr, address_range range) {}    // TODO: Implement, change return type to outcome
            void unregister_device(device_ptr dev_ptr) {}                       // TODO: Implement, change return type to outcome

            /*void register_device(AddressableDevicePtr dev_ptr, address_range range) {
                if (!total_range_.contains(range))
                    throw memory_access_error(range.low_addr < total_range_.low_addr ? range.low_addr : range.high_addr,
                                              "Device address range doesn't fit into the address space!");
                devices_.emplace_back(range, dev_ptr);
            }
            void unregister_device(AddressableDevicePtr dev_ptr) {
                auto dev_it = std::find_if(devices_.cbegin(), devices_.cend(), [dev_ptr](auto mdev){ return mdev.second == dev_ptr; });
                if (dev_it != devices_.cend()) devices_.erase(dev_it);
            }*/

        protected:
            address_range total_range_;
            std::vector<mapped_device> devices_;
    };

}
}

#endif  /* _UCLE_CORE_SIMULATORS_FUNCTIONAL_COMPONENTS_HPP_ */
