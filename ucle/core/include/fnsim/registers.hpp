#ifndef _UCLE_CORE_FNSIM_REGISTERS_HPP_
#define _UCLE_CORE_FNSIM_REGISTERS_HPP_

#include <fnsim/base.hpp>

namespace ucle::fnsim {

    template<unsigned N, typename T = meta::sized_uint_t<N>>
    class reg {
        public:
            using value_type = T;

            constexpr reg() noexcept : value_{0} {}
            constexpr reg(value_type value) noexcept : value_{value} {}
            constexpr reg(const reg<N>& other) noexcept : value_{other.value_} {}
            constexpr reg(reg<N>&& other) noexcept : value_{other.value_} {}

            constexpr reg<N>& operator=(value_type value) noexcept
                { value_ = value; return *this; }
            constexpr reg<N>& operator=(const reg<N>& other) noexcept
                { value_ = other.value_; return *this; }
            constexpr reg<N>& operator=(reg<N>&& other) noexcept
                { value_ = other.value_; return *this; }

            constexpr operator value_type() const noexcept { return value_; }

            constexpr value_type operator[](const bitrange br) const noexcept
                { return (value_ >> br.shift()) & br.mask(); }
            constexpr bool operator[](index_t i) const noexcept
                { return value_ & (static_cast<index_t>(1) << i); }

            constexpr reg<N>& operator+=(value_type v) noexcept { value_ += v; return *this; }
            constexpr reg<N>& operator-=(value_type v) noexcept { value_ -= v; return *this; }
            constexpr reg<N>& operator&=(value_type v) noexcept { value_ &= v; return *this; }
            constexpr reg<N>& operator|=(value_type v) noexcept { value_ |= v; return *this; }
            constexpr reg<N>& operator^=(value_type v) noexcept { value_ ^= v; return *this; }

            friend constexpr bool operator==(const reg<N>& lhs, const reg<N>& rhs) noexcept { return lhs.value_ == rhs.value_; }
            friend constexpr bool operator!=(const reg<N>& lhs, const reg<N>& rhs) noexcept { return lhs.value_ != rhs.value_; }
            friend constexpr bool operator<(const reg<N>& lhs, const reg<N>& rhs) noexcept { return lhs.value_ < rhs.value_; }
            friend constexpr bool operator>(const reg<N>& lhs, const reg<N>& rhs) noexcept { return lhs.value_ > rhs.value_; }
            friend constexpr bool operator<=(const reg<N>& lhs, const reg<N>& rhs) noexcept { return lhs.value_ <= rhs.value_; }
            friend constexpr bool operator>=(const reg<N>& lhs, const reg<N>& rhs) noexcept { return lhs.value_ >= rhs.value_; }

            friend constexpr value_type operator+(reg<N> lhs, reg<N> rhs) noexcept { return lhs.value_ + rhs.value_; }
            friend constexpr value_type operator-(reg<N> lhs, reg<N> rhs) noexcept { return lhs.value_ - rhs.value_; }
            friend constexpr value_type operator&(reg<N> lhs, reg<N> rhs) noexcept { return lhs.value_ & rhs.value_; }
            friend constexpr value_type operator|(reg<N> lhs, reg<N> rhs) noexcept { return lhs.value_ | rhs.value_; }
            friend constexpr value_type operator^(reg<N> lhs, reg<N> rhs) noexcept { return lhs.value_ ^ rhs.value_; }

            friend constexpr void swap(reg<N>& lhs, reg<N>& rhs) noexcept { std::swap(lhs.value_, rhs.value_); }
        private:
            value_type value_ = 0;
    };

    template<unsigned N, typename T = meta::sized_uint_t<N>>
    class flags_reg {
        public:
            using value_type = T;
            using flag_reference = typename bitfield<N>::reference;

            constexpr flags_reg() noexcept : value_{0} {}
            constexpr flags_reg(value_type value) noexcept : value_{value} {}
            constexpr flags_reg(const flags_reg<N>& other) noexcept : value_{other.value_} {}
            constexpr flags_reg(flags_reg<N>&& other) noexcept : value_{other.value_} {}

            constexpr flags_reg<N>& operator=(const flags_reg<N>& other) noexcept
                {value_ = other.value_; return *this; }
            constexpr flags_reg<N>& operator=(flags_reg<N>&& other) noexcept
                { value_ = other.value_; return *this; }

            constexpr operator value_type() const noexcept { return value_type(value_); }

            constexpr value_type operator[](const bitrange br) const noexcept
                { return (value_type(value_) >> br.shift()) & br.mask(); }

            constexpr flag_reference operator[](index_t i) noexcept { return value_[i]; }
            constexpr bool operator[](index_t i) const noexcept { return value_[i]; }

            constexpr value_type operator&=(value_type v) noexcept { return value_ &= v; }
            constexpr value_type operator|=(value_type v) noexcept { return value_ |= v; }
            constexpr value_type operator^=(value_type v) noexcept { return value_ ^= v; }

            friend constexpr bool operator==(const flags_reg<N>& lhs, const flags_reg<N>& rhs) noexcept { return lhs.value_ == rhs.value_; }
            friend constexpr bool operator!=(const flags_reg<N>& lhs, const flags_reg<N>& rhs) noexcept { return lhs.value_ != rhs.value_; }
            friend constexpr bool operator<(const flags_reg<N>& lhs, const flags_reg<N>& rhs) noexcept { return lhs.value_ < rhs.value_; }
            friend constexpr bool operator>(const flags_reg<N>& lhs, const flags_reg<N>& rhs) noexcept { return lhs.value_ > rhs.value_; }
            friend constexpr bool operator<=(const flags_reg<N>& lhs, const flags_reg<N>& rhs) noexcept { return lhs.value_ <= rhs.value_; }
            friend constexpr bool operator>=(const flags_reg<N>& lhs, const flags_reg<N>& rhs) noexcept { return lhs.value_ >= rhs.value_; }

            friend constexpr value_type operator&(flags_reg<N> lhs, flags_reg<N> rhs) noexcept { return lhs.value_ & rhs.value_; }
            friend constexpr value_type operator|(flags_reg<N> lhs, flags_reg<N> rhs) noexcept { return lhs.value_ | rhs.value_; }
            friend constexpr value_type operator^(flags_reg<N> lhs, flags_reg<N> rhs) noexcept { return lhs.value_ ^ rhs.value_; }

            friend constexpr void swap(flags_reg<N>& lhs, flags_reg<N>& rhs) noexcept { swap(lhs.value_, rhs.value_); }
        private:
            bitfield<N> value_ = 0;
    };

    class register_file {
        // Abstract base class for all processor register files

        public:
            virtual void clear() = 0;
    };
}

#endif  /* _UCLE_CORE_FNSIM_REGISTERS_HPP_ */
