#ifndef _UCLE_CORE_FNSIM_DEVICE_HPP_
#define _UCLE_CORE_FNSIM_DEVICE_HPP_

#include <fnsim/base.hpp>

#include <fnsim/registers.hpp>

#include <util/const_bin_util.hpp>

#include <array>
#include <cstring>
#include <memory>

namespace ucle::fnsim {

    class device {
        public:
            virtual ~device() = default;

            virtual device_status status() = 0;

            virtual void work() = 0;
            virtual void reset() = 0;

            virtual bool is_worker() const = 0;
            virtual bool can_interrupt() const { return false; }
            virtual priority_t interrupt_priority() const { return 0; }
    };

    using device_ptr = std::shared_ptr<device>;

    template<byte_order endianness, typename AddressType>
    class mapped_device : public device {
        using self_type = mapped_device<endianness, AddressType>;

        public:
            using address_type = AddressType;

            mapped_device()                         = default;
            mapped_device(const self_type&)         = delete;
            mapped_device(self_type&&)              = default;

            self_type& operator=(const self_type&)  = delete;
            self_type& operator=(self_type&&)       = default;

            template <typename T, typename = meta::is_storage_t<T>>
            T read(address_type location)
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
            virtual byte_t read_byte_(address_type location) = 0;
            virtual half_t read_half_(address_type location) = 0;
            virtual word_t read_word_(address_type location) = 0;

            virtual void write_byte_(address_type location, byte_t value) = 0;
            virtual void write_half_(address_type location, half_t value) = 0;
            virtual void write_word_(address_type location, word_t value) = 0;
    };

    template<byte_order endianness, typename AddressType>
    class memory_block_device : public mapped_device<endianness, AddressType> {
        using self_type = memory_block_device<endianness, AddressType>;

        public:
            using address_type = AddressType;

            memory_block_device()                   = delete;
            memory_block_device(size_t memory_size) : size_(memory_size), data_(std::make_unique<byte_t[]>(memory_size)) { reset(); }
            memory_block_device(const self_type&)   = delete;
            memory_block_device(self_type&&)        = default;

            self_type& operator=(const self_type&)  = delete;
            self_type& operator=(self_type&&)       = default;

            ~memory_block_device() override         = default;

            void reset() override { memset(data_.get(), 0, size_); }

        protected:
            byte_t read_byte_(address_type location) override
            {
                return data_[location];
            }

            half_t read_half_(address_type location) override
            {
                return *(reinterpret_cast<half_t*>(&data_[location]));
            }

            word_t read_word_(address_type location) override
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

    template<byte_order endianness, typename AddressType>
    class memory final : public memory_block_device<endianness, AddressType> {
        public:
            using address_type = AddressType;

            using memory_block_device<endianness, address_type>::memory_block_device;
            using memory_block_device<endianness, address_type>::operator=;

            device_status status() override {  return device_status::idle; }
            void work() override {}

            bool is_worker() const override { return false; }
    };

    template<unsigned reg_num, unsigned reg_size, byte_order endianness, typename AddressType>
    class register_set_device : public mapped_device<endianness, AddressType> {
        using self_type = register_set_device<reg_num, reg_size, endianness, AddressType>;

        public:
            using address_type = AddressType;
            using register_type = reg<reg_size>;
            using value_type = typename register_type::value_type;
            using cbu = util::const_bin_util<value_type>;

            constexpr static auto register_size = sizeof(typename register_type::value_type);

            register_set_device()                   = default;
            register_set_device(const self_type&)   = delete;
            register_set_device(self_type&&)        = default;

            self_type& operator=(const self_type&)  = delete;
            self_type& operator=(self_type&&)       = default;

            ~register_set_device() override         = default;

            void reset() override
            {
                for (auto& reg : regs_)
                    reg = 0;
            }

        protected:
            byte_t read_byte_(address_type location) override
            {
                return cbu::nth_byte_of(read_word_(location), location & register_size);
            }

            half_t read_half_(address_type location) override
            {
                if constexpr (register_size < sizeof(half_t))
                    throw invalid_memory_access("Register size too small to read a half-word");

                return cbu::nth_half_of(read_word_(location), (location & register_size) >> 1);
            }

            word_t read_word_(address_type location) override
            {
                if constexpr (register_size < sizeof(word_t))
                    throw invalid_memory_access("Register size too small to read a word");

                return reg_(location);
            }

            void write_byte_(address_type location, byte_t value) override
            {
                write_word_(location, cbu::set_nth_byte_of(regs_[location / register_size], location & register_size, value));
            }

            void write_half_(address_type location, half_t value) override
            {
                if constexpr (register_size < sizeof(half_t))
                    throw invalid_memory_access("Register size too small to write a half-word");

                write_word_(location, cbu::set_nth_half_of(regs_[location / register_size], (location & register_size) >> 1, value));
            }

            void write_word_(address_type location, word_t value) override
            {
                if constexpr (register_size < sizeof(word_t))
                    throw invalid_memory_access("Register size too small to write a word");

                reg_(location) = value;
            }

            register_type reg_(address_type location) const { return regs_[location / register_size]; }
            register_type& reg_(address_type location) { return regs_[location / register_size]; }

        private:
            std::array<register_type, reg_num> regs_;
    };
}

#endif  /* _UCLE_CORE_FNSIM_DEVICE_HPP_ */
