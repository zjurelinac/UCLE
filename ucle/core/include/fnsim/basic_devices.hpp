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
                constexpr auto size = sizeof(T);
                auto bytes = read_bytes_(location, sizeof(T));

                T value = 0;
                if constexpr (layout_type == byte_order::LE) {
                    for (auto i = 0u; i < size; ++i)
                        value = value << 8 | bytes[size - i - 1];
                } else {  /* byte_order::BE */
                    for (auto i = 0u; i < size; ++i)
                        value = value << 8 | bytes[i];
                }

                return value;
            }

            template <typename T, typename = meta::is_storage_t<T>>
            void write(address_t location, T value)
            {
                auto size = sizeof(T);
                small_byte_vector bytes(size);

                if constexpr (layout_type == byte_order::LE) {
                    for (auto i = 0u; i < size; ++i) {
                        bytes[i] = value & 0xFF;
                        value >>= 8;
                    }
                } else {  /* byte_order::BE */
                    for (auto i = 0u; i < size; ++i) {
                        bytes[size - i - 1] = value & 0xFF;
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
                small_byte_vector bytes(amount);
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
