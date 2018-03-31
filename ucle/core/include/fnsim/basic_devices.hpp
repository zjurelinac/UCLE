#ifndef _UCLE_CORE_FNSIM_BASIC_DEVICES_HPP_
#define _UCLE_CORE_FNSIM_BASIC_DEVICES_HPP_

#include <common/exceptions.hpp>
#include <common/meta.hpp>
#include <common/structures.hpp>
#include <common/types.hpp>

#include <fnsim/address_space.hpp>
#include <fnsim/base.hpp>
#include <fnsim/registers.hpp>

#include <array>
#include <cstring>
#include <memory>

namespace ucle::fnsim {

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
                constexpr auto size = sizeof(T);
                auto bytes = read_bytes_(location, sizeof(T));

                T value = 0;

                for (auto i = 0u; i < size; ++i) {
                    if constexpr (endianness == byte_order::LE)
                        value = value << 8 | bytes[size - i - 1];
                    else
                        value = value << 8 | bytes[i];
                }

                return value;
            }
            template <typename T, typename = meta::is_storage_t<T>>
            void write(address_type location, T value)
            {
                auto size = sizeof(T);
                small_byte_vector bytes(size);

                for (auto i = 0u; i < size; ++i, value >>= 8) {
                    if constexpr (endianness == byte_order::LE)
                        bytes[i] = value & 0xFF;
                    else
                        bytes[size - i - 1] = value & 0xFF;
                }

                write_bytes_(location, bytes);
            }
        protected:
            virtual small_byte_vector read_bytes_(address_type location, size_t amount) const = 0;
            virtual void write_bytes_(address_type location, small_byte_vector& bytes) = 0;
    };

    template <byte_order endianness, typename AddressType = address_t>
    using mapped_device_ptr = std::shared_ptr<mapped_device<endianness, AddressType>>;

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


            virtual void reset() override { memset(data_.get(), 0, size_); }

        protected:
            virtual small_byte_vector read_bytes_(address_t location, size_t amount) const override
            {
                small_byte_vector bytes(amount);
                for (auto i = 0u; i < amount; ++i)
                    bytes[i] = data_[location++];
                return bytes;
            }

            virtual void write_bytes_(address_t location, small_byte_vector& bytes) override
            {
                for (auto i = 0u; i < bytes.size(); ++i)
                    data_[location++] = bytes[i];
            }

        private:
            size_t size_ = 0;
            std::unique_ptr<byte_t[]> data_;
    };

    template<byte_order endianness = byte_order::LE, typename AddressType = address_t>
    class memory : public memory_block_device<endianness, AddressType> {
        public:
            using memory_block_device<endianness>::memory_block_device;
            using memory_block_device<endianness>::operator=;

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
