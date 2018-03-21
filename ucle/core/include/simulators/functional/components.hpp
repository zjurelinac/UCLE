#ifndef _UCLE_CORE_SIMULATORS_FUNCTIONAL_COMPONENTS_HPP_
#define _UCLE_CORE_SIMULATORS_FUNCTIONAL_COMPONENTS_HPP_

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cstring>
#include <memory>
#include <vector>
#include <set>

#include <common/types.hpp>
#include <common/exceptions.hpp>

/*
    TODO:
    - Variable address_t size
*/


namespace ucle::fnsim {

    /*** Registers ***/

    template<typename ValueType>
    class reg_inst_ {
        public:
            using value_type = ValueType;

            reg_inst_() : value_(0) {}
            reg_inst_(value_type value) : value_(value) {}
            reg_inst_(const reg_inst_<value_type>& other) : value_(other.value_) {}
            reg_inst_(reg_inst_<value_type>&& other) : value_(other.value_) {}

            reg_inst_<value_type>& operator=(const value_type& value)
                { value_ = value; return *this; }
            reg_inst_<value_type>& operator=(const reg_inst_<value_type>& other)
                { if (this != &other) value_ = other.value; return *this; }
            reg_inst_<value_type>& operator=(reg_inst_<value_type>&& other)
                { if (this != &other) value_ = other.value; return *this; }

            void set(value_type value) { value_ = value; }
            value_type get() const { return value_; }

        protected:
            value_type value_;
    };

    template<unsigned bits> class reg {};
    template<>              class reg<8>  : public reg_inst_<byte_t> { public: using reg_inst_<byte_t>::operator=; };
    template<>              class reg<16> : public reg_inst_<half_t> { public: using reg_inst_<half_t>::operator=; };
    template<>              class reg<32> : public reg_inst_<word_t> { public: using reg_inst_<word_t>::operator=; };

    template <unsigned bits>
    class flags_reg {
        protected:

    };

    class register_file {
        // Abstract base class for all processor register files

        public:
            virtual void clear() = 0;
    };

    /*** Devices ***/

    enum class endianness { LE, BE };

    enum class mapping_type { DEFAULT, MEMORY, PORT, NONE };

    // TODO: Select possible options
    enum class device_status {};

    class device {
        // Abstract base class for all devices, both memory-mapped and isolated

        public:
            virtual void work() = 0;
            virtual void status() = 0;
            virtual void reset() = 0;
    };

    using device_ptr = std::shared_ptr<device>;

    // Abstract base class for all memory-mapped devices

    class memory_mapped_device : public device {
        public:
            virtual byte_t read_byte(address_t location) = 0;
            virtual half_t read_half(address_t location) = 0;
            virtual word_t read_word(address_t location) = 0;

            virtual void write_byte(address_t location, byte_t value) = 0;
            virtual void write_half(address_t location, half_t value) = 0;
            virtual void write_word(address_t location, word_t value) = 0;
    };

    using memory_mapped_device_ptr = std::shared_ptr<memory_mapped_device>;

    // Generic memory-block based device

    template<endianness layout_type>
    class memory_block_device : public memory_mapped_device {
         public:
            memory_block_device(size_t memory_size) : size_(memory_size) { data_ = new byte_t[size_]; reset(); }
            ~memory_block_device() { delete[] data_; }

            virtual byte_t read_byte(address_t location) override;
            virtual half_t read_half(address_t location) override;
            virtual word_t read_word(address_t location) override;

            virtual void write_byte(address_t location, byte_t value) override;
            virtual void write_half(address_t location, half_t value) override;
            virtual void write_word(address_t location, word_t value) override;

            virtual void reset() override { memset(data_, 0, size_); }

        private:
            size_t size_;
            byte_t *data_;
    };

    // Little-endian memory block device specialization

    template<>
    inline byte_t memory_block_device<endianness::LE>::read_byte(address_t location) {
        return data_[location];
    }

    template<>
    inline half_t memory_block_device<endianness::LE>::read_half(address_t location) {
        return (data_[location + 1] << 8) | data_[location];
    }

    template<>
    inline word_t memory_block_device<endianness::LE>::read_word(address_t location) {
        return (data_[location + 3] << 24) | (data_[location + 2] << 16) |
               (data_[location + 1] << 8)  | (data_[location]);
    }

    template<>
    inline void memory_block_device<endianness::LE>::write_byte(address_t location, byte_t value) {
        data_[location] = value;
    }

    template<>
    inline void memory_block_device<endianness::LE>::write_half(address_t location, half_t value) {
        data_[location + 1] = value >> 8;
        data_[location]     = value & 0xFF;
    }

    template<>
    inline void memory_block_device<endianness::LE>::write_word(address_t location, word_t value) {
        data_[location + 3] = value >> 24;
        data_[location + 2] = (value >> 16) & 0xFF;
        data_[location + 1] = (value >> 8) & 0xFF;
        data_[location] = value & 0xFF;
    }

    // Big-endian memory block device specialization

    template<>
    inline byte_t memory_block_device<endianness::BE>::read_byte(address_t location) {
        return data_[location];
    }

    template<>
    inline half_t memory_block_device<endianness::BE>::read_half(address_t location) {
        return (data_[location] << 8) | data_[location + 1];
    }

    template<>
    inline word_t memory_block_device<endianness::BE>::read_word(address_t location) {
        return (data_[location] << 24)    | (data_[location + 1] << 16) |
               (data_[location + 2] << 8) | (data_[location + 3]);
    }

    template<>
    inline void memory_block_device<endianness::BE>::write_byte(address_t location, byte_t value) {
        data_[location] = value;
    }

    template<>
    inline void memory_block_device<endianness::BE>::write_half(address_t location, half_t value) {
        data_[location]     = value >> 8;
        data_[location + 1] = value & 0xFF;
    }

    template<>
    inline void memory_block_device<endianness::BE>::write_word(address_t location, word_t value) {
        data_[location] = value >> 24;
        data_[location + 1] = (value >> 16) & 0xFF;
        data_[location + 2] = (value >> 8) & 0xFF;
        data_[location + 3] = value & 0xFF;
    }

    // Generic register-group based device

    template<size_t reg_num, size_t reg_size = 32>
    class register_group_device : public memory_mapped_device {
        public:
            // TODO read/write

        protected:
            std::array<reg<reg_size>, reg_num> registers_;
    };


    // Basic RAM memory device representation

    template<endianness layout_type = endianness::LE>
    class memory : public memory_block_device<layout_type> {
        using memory_block_device<layout_type>::memory_block_device;

        public:
            virtual void work() override {}
            virtual void status() override {}
    };

    // Generic address space impl - does the mapping of memory operations to corresponding devices

    template <typename DevicePtr = memory_mapped_device_ptr>
    class address_space {
        using device_ptr = DevicePtr;
        using mapped_device = std::pair<address_range, device_ptr>;

        public:
            address_space(address_range total_range) : total_range_(total_range) {}

            void register_device(device_ptr dev_ptr, address_range range) {
                if (!total_range_.contains(range))
                    throw invalid_address_range("Device address range overflows the available address space.");

                devices_.emplace(range, dev_ptr);
            }
            void unregister_device(device_ptr dev_ptr) {
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
            device_ptr find_device_(address_t location) {
                auto dev_it = std::find_if(devices_.cbegin(), devices_.cend(),
                    [location](auto mapped){ return mapped.first.contains(location); });

                if (dev_it != devices_.cend())
                    return dev_it->second;
                else
                    throw invalid_memory_access("No memory/device mapped to this address!");
            }

            address_range total_range_;
            std::set<mapped_device> devices_;
    };

    // Interrupt lines

    template <unsigned num_levels>
    class interrupt_lines {
        public:

    };

}

#endif  /* _UCLE_CORE_SIMULATORS_FUNCTIONAL_COMPONENTS_HPP_ */
