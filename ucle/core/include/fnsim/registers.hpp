#ifndef _UCLE_CORE_FNSIM_REGISTERS_HPP_
#define _UCLE_CORE_FNSIM_REGISTERS_HPP_

#include <common/meta.hpp>
#include <common/structures.hpp>
#include <common/types.hpp>

namespace ucle::fnsim {

    template<unsigned N, typename T = meta::sized_uint<N>>
    class reg {
        public:
            using value_type = T;

            constexpr reg() : value_(0) {}
            constexpr reg(value_type value) : value_(value) {}
            constexpr reg(const reg<N>& other) : value_(other.value_) {}
            constexpr reg(reg<N>&& other) : value_(other.value_) {}

            constexpr reg<N>& operator=(const reg<N>& other)
                {value_ = other.value_; return *this; }
            constexpr reg<N>& operator=(reg<N>&& other)
                { value_ = other.value_; return *this; }

            constexpr operator value_type() const { return value_; }

            constexpr value_type operator[](const bitrange br) const
                { return (value_ >> br.shift()) & br.mask(); }
            constexpr bool operator[](index_t i) const
                { return value_ & (static_cast<index_t>(1) << i); }

            constexpr reg<N>& operator+=(value_type v) { value_ += v; return *this; }
            constexpr reg<N>& operator-=(value_type v) { value_ -= v; return *this; }
            constexpr reg<N>& operator&=(value_type v) { value_ &= v; return *this; }
            constexpr reg<N>& operator|=(value_type v) { value_ |= v; return *this; }
            constexpr reg<N>& operator^=(value_type v) { value_ ^= v; return *this; }

            constexpr bool operator<(value_type v) const { return value_ < v; }
            constexpr bool operator>(value_type v) const { return value_ > v; }
            constexpr bool operator<=(value_type v) const { return value_ <= v; }
            constexpr bool operator>=(value_type v) const { return value_ >= v; }
            constexpr bool operator==(value_type v) const { return value_ == v; }
            constexpr bool operator!=(value_type v) const { return value_ != v; }

        private:
            value_type value_;
    };

    template<unsigned N, typename T = meta::sized_uint<N>>
    class flags_reg {
        public:
            using value_type = T;
            using flag_reference = typename bitfield<N>::reference;

            constexpr flags_reg() : value_(0) {}
            constexpr flags_reg(value_type value) : value_(value) {}
            constexpr flags_reg(const flags_reg<N>& other) : value_(other.value_) {}
            constexpr flags_reg(flags_reg<N>&& other) : value_(other.value_) {}

            constexpr flags_reg<N>& operator=(const flags_reg<N>& other)
                {value_ = other.value_; return *this; }
            constexpr flags_reg<N>& operator=(flags_reg<N>&& other)
                { value_ = other.value_; return *this; }

            constexpr operator value_type() const { return value_type(value_); }

            constexpr value_type operator[](const bitrange br) const
                { return (value_ >> br.shift()) & br.mask(); }

            constexpr flag_reference operator[](index_t i)
                { return value_[i]; }
            constexpr bool operator[](index_t i) const
                { return value_[i]; }

            constexpr value_type operator&=(value_type v) { return value_ &= v; }
            constexpr value_type operator|=(value_type v) { return value_ |= v; }
            constexpr value_type operator^=(value_type v) { return value_ ^= v; }

        private:
            bitfield<N> value_;
    };

    class register_file {
        // Abstract base class for all processor register files

        public:
            virtual void clear() = 0;
    };
}

#endif  /* _UCLE_CORE_FNSIM_REGISTERS_HPP_ */
