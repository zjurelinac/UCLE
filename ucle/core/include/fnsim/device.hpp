#ifndef _UCLE_CORE_FNSIM_DEVICE_HPP_
#define _UCLE_CORE_FNSIM_DEVICE_HPP_

#include <common/meta.hpp>
#include <common/structures.hpp>
#include <common/types.hpp>

#include <fnsim/address_space.hpp>
#include <fnsim/base.hpp>
#include <fnsim/exceptions.hpp>
#include <fnsim/registers.hpp>

#include <array>
#include <cstring>
#include <memory>

namespace ucle::fnsim {

    class device {
        public:
            virtual ~device() = default;

            virtual void work() = 0;
            virtual void status() = 0;
            virtual void reset() = 0;
    };

    using device_ptr = std::unique_ptr<device>;

    template<byte_order endianness, typename AddressType = address_t>
    class mapped_device : public device {
        public:
            using address_type = AddressType;

            mapped_device() = default;

            mapped_device(const mapped_device<endianness, address_type>&) = delete;
            mapped_device<endianness, address_type>& operator=(const mapped_device<endianness, address_type>&) = delete;

            mapped_device(mapped_device<endianness, address_type>&&) = default;
            mapped_device<endianness, address_type>& operator=(mapped_device<endianness, address_type>&&) = default;

            ~mapped_device() override = default;

            template <typename T, typename = meta::is_storage_t<T>>
            T read(address_type location) const
            {
                if constexpr (std::is_same_v<T, byte_t>) return read_byte_(location);
                if constexpr (std::is_same_v<T, half_t>) return read_half_(location);
                if constexpr (std::is_same_v<T, word_t>) return read_word_(location);
            }

            template <typename T, typename = meta::is_storage_t<T>>
            void write(address_type location, T value)
            {
                if constexpr (std::is_same_v<T, byte_t>) write_byte_(location, value);
                if constexpr (std::is_same_v<T, half_t>) write_half_(location, value);
                if constexpr (std::is_same_v<T, word_t>) write_word_(location, value);
            }

        protected:
            virtual byte_t read_byte_(address_type location) const = 0;
            virtual half_t read_half_(address_type location) const = 0;
            virtual word_t read_word_(address_type location) const = 0;
            
            virtual void write_byte_(address_type location, byte_t value) = 0;
            virtual void write_half_(address_type location, half_t value) = 0;
            virtual void write_word_(address_type location, word_t value) = 0;
    };

    template<byte_order endianness, typename AddressType = address_t>
    class memory_block_device : public mapped_device<endianness, AddressType> {
        public:
            using address_type = AddressType;

            memory_block_device() = delete;
            memory_block_device(size_t memory_size)
                : size_(memory_size), data_(std::make_unique<byte_t[]>(memory_size)) { reset(); }

            memory_block_device(const memory_block_device<endianness, address_type>&) = delete;
            memory_block_device<endianness, address_type>& operator=(const memory_block_device<endianness, address_type>&) = delete;

            memory_block_device(memory_block_device<endianness, address_type>&&) = default;
            memory_block_device<endianness, address_type>& operator=(memory_block_device<endianness, address_type>&&) = default;

            ~memory_block_device() override = default;

            void reset() override { memset(data_.get(), 0, size_); }

        protected:
            byte_t read_byte_(address_type location) const override
            {
                return data_[location];
            }

            half_t read_half_(address_type location) const override
            {
                return *(reinterpret_cast<half_t*>(&data_[location]));
            }

            word_t read_word_(address_type location) const override
            {
                return *(reinterpret_cast<word_t*>(&data_[location]));
            }
            
            void write_byte_(address_type location, byte_t value) override
            {
                data_[location] = value;
            }

            void write_half_(address_type location, half_t value) override
            {
                *(reinterpret_cast<half_t*>(&data_[location])) = value;
            }

            void write_word_(address_type location, word_t value) override
            {
                *(reinterpret_cast<word_t*>(&data_[location])) = value;
            }

        private:
            size_t size_ = 0;
            std::unique_ptr<byte_t[]> data_;
    };

    template<byte_order endianness = byte_order::little_endian, typename AddressType = address_t>
    class memory final : public memory_block_device<endianness, AddressType> {
        public:
            using memory_block_device<endianness>::memory_block_device;
            using memory_block_device<endianness>::operator=;

            void work() override {}
            void status() override {}
    };


    // Interrupt lines

    /*template <unsigned num_levels>
    class interrupt_lines {
        public:

    };*/

}

#endif  /* _UCLE_CORE_FNSIM_DEVICE_HPP_ */
