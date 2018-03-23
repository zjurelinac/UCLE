#ifndef _UCLE_CORE_FNSIM_BASIC_DEVICES_HPP_
#define _UCLE_CORE_FNSIM_BASIC_DEVICES_HPP_

#include <common/exceptions.hpp>
#include <common/types.hpp>

#include <fnsim/fnsim.hpp>
#include <fnsim/address_space.hpp>
#include <fnsim/registers.hpp>

#include <array>
#include <cstring>
#include <memory>

/*
    TODO: Variable address_t size
*/


namespace ucle::fnsim {

    class mapped_device : public device {
        public:
            virtual byte_t read_byte(address_t location) = 0;
            virtual half_t read_half(address_t location) = 0;
            virtual word_t read_word(address_t location) = 0;

            virtual void write_byte(address_t location, byte_t value) = 0;
            virtual void write_half(address_t location, half_t value) = 0;
            virtual void write_word(address_t location, word_t value) = 0;
    };

    using mapped_device_ptr = std::shared_ptr<mapped_device>;

    template<endianness layout_type>
    class memory_block_device : public mapped_device {
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


    template<size_t reg_num, size_t reg_size = 32>
    class register_group_device : public mapped_device {
        public:
            // TODO read/write

        protected:
            std::array<reg<reg_size>, reg_num> registers_;
    };


    template<endianness layout_type = endianness::LE>
    class memory : public memory_block_device<layout_type> {
        using memory_block_device<layout_type>::memory_block_device;

        public:
            virtual void work() override {}
            virtual void status() override {}
    };


    // Interrupt lines

    template <unsigned num_levels>
    class interrupt_lines {
        public:

    };

}

#endif  /* _UCLE_CORE_FNSIM_BASIC_DEVICES_HPP_ */
