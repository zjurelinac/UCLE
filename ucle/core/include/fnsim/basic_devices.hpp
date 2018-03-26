#ifndef _UCLE_CORE_FNSIM_BASIC_DEVICES_HPP_
#define _UCLE_CORE_FNSIM_BASIC_DEVICES_HPP_

#include <common/exceptions.hpp>
#include <common/meta.hpp>
#include <common/structures.hpp>
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

    template<byte_order layout_type>
    class mapped_device : public device {
        public:
            template <typename T, typename = meta::is_storage_t<T>>
            T read(address_t location) const
            {
                auto bytes = read_bytes_(location, sizeof(T));
                T value = 0;

                if constexpr (layout_type == byte_order::LE) {
                    for (auto i = 3u; i >= 0; --i)
                        value = value << 8 | bytes[i];
                } else {  /* byte_order::BE */
                    for (auto i = 0; i < 4; ++i)
                        value = value << 8 | bytes[i];
                }

                return value;
            }

            template <typename T, typename = meta::is_storage_t<T>>
            void write(address_t location, T value)
            {
                small_byte_vector bytes;

                if constexpr (layout_type == byte_order::LE) {
                    for (auto i = 0u; i < 4; ++i) {
                        bytes[i] = value & 0xFF;
                        value >>= 8;
                    }
                } else {  /* byte_order::BE */
                    for (auto i = 3u; i >= 0; -i) {
                        bytes[i] = value & 0xFF;
                        value >>= 8;
                    }
                }

                write_bytes_(location, bytes);
            }

        protected:
            virtual small_byte_vector read_bytes_(address_t location, size_t amount) const = 0;
            virtual void write_bytes_(address_t location, small_byte_vector& bytes) = 0;
    };

    template <byte_order layout_type>
    using mapped_device_ptr = std::shared_ptr<mapped_device<layout_type>>;

    template<byte_order layout_type>
    class memory_block_device : public mapped_device<layout_type> {
         public:
            memory_block_device(size_t memory_size) : size_(memory_size) { data_ = new byte_t[size_]; reset(); }
            ~memory_block_device() { delete[] data_; }

            virtual small_byte_vector read_bytes_(address_t location, size_t amount) const override
            {
                small_byte_vector bytes;
                for (auto i = 0u; i < amount; ++i)
                    bytes[i] = data_[location++];
                return bytes;
            }

            virtual void write_bytes_(address_t location, small_byte_vector& bytes)
            {
                for (auto i = 0u; i < bytes.size(); ++i)
                    data_[location++] = bytes[i];
            }

            virtual void reset() override { memset(data_, 0, size_); }

        private:
            size_t size_;
            byte_t *data_;
    };

    // Little-endian memory block device specialization

    /*template<>
    inline byte_t memory_block_device<byte_order::LE>::read_byte(address_t location) {
        return data_[location];
    }

    template<>
    inline half_t memory_block_device<byte_order::LE>::read_half(address_t location) {
        return (data_[location + 1] << 8) | data_[location];
    }

    template<>
    inline word_t memory_block_device<byte_order::LE>::read_word(address_t location) {
        return (data_[location + 3] << 24) | (data_[location + 2] << 16) |
               (data_[location + 1] << 8)  | (data_[location]);
    }

    template<>
    inline void memory_block_device<byte_order::LE>::write_byte(address_t location, byte_t value) {
        data_[location] = value;
    }

    template<>
    inline void memory_block_device<byte_order::LE>::write_half(address_t location, half_t value) {
        data_[location + 1] = value >> 8;
        data_[location]     = value & 0xFF;
    }

    template<>
    inline void memory_block_device<byte_order::LE>::write_word(address_t location, word_t value) {
        data_[location + 3] = value >> 24;
        data_[location + 2] = (value >> 16) & 0xFF;
        data_[location + 1] = (value >> 8) & 0xFF;
        data_[location] = value & 0xFF;
    }*/

    // Big-endian memory block device specialization

    /*template<>
    inline byte_t memory_block_device<byte_order::BE>::read_byte(address_t location) {
        return data_[location];
    }

    template<>
    inline half_t memory_block_device<byte_order::BE>::read_half(address_t location) {
        return (data_[location] << 8) | data_[location + 1];
    }

    template<>
    inline word_t memory_block_device<byte_order::BE>::read_word(address_t location) {
        return (data_[location] << 24)    | (data_[location + 1] << 16) |
               (data_[location + 2] << 8) | (data_[location + 3]);
    }

    template<>
    inline void memory_block_device<byte_order::BE>::write_byte(address_t location, byte_t value) {
        data_[location] = value;
    }

    template<>
    inline void memory_block_device<byte_order::BE>::write_half(address_t location, half_t value) {
        data_[location]     = value >> 8;
        data_[location + 1] = value & 0xFF;
    }

    template<>
    inline void memory_block_device<byte_order::BE>::write_word(address_t location, word_t value) {
        data_[location] = value >> 24;
        data_[location + 1] = (value >> 16) & 0xFF;
        data_[location + 2] = (value >> 8) & 0xFF;
        data_[location + 3] = value & 0xFF;
    }*/


    /*template<size_t reg_num, size_t reg_size = 32>
    class register_group_device : public mapped_device {
        public:
            // TODO read/write

        protected:
            std::array<reg<reg_size>, reg_num> registers_;
    };*/


    template<byte_order endianness = byte_order::LE>
    class memory : public memory_block_device<endianness> {
        using memory_block_device<endianness>::memory_block_device;

        public:
            virtual void work() override {}
            virtual void status() override {}
    };


    // Interrupt lines

    /*template <unsigned num_levels>
    class interrupt_lines {
        public:

    };*/

}

#endif  /* _UCLE_CORE_FNSIM_BASIC_DEVICES_HPP_ */
