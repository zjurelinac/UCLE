#ifndef _UCLE_CORE_FNSIM_REGISTERS_HPP_
#define _UCLE_CORE_FNSIM_REGISTERS_HPP_

#include <fnsim/base.hpp>

namespace ucle::fnsim {

    template<unsigned N, typename T = meta::sized_uint_t<N>>
    class reg {
        using self_type = reg<N>;

        public:
            using value_type = T;

            constexpr reg() noexcept : value_{0} {}
            constexpr explicit reg(value_type value) noexcept : value_{value} {}
            constexpr reg(const self_type& other) noexcept : value_{other.value_} {}
            constexpr reg(self_type&& other) noexcept : value_{other.value_} {}

            constexpr self_type& operator=(value_type value) noexcept
                { value_ = value; return *this; }
            constexpr self_type& operator=(const self_type& other) noexcept
                { value_ = other.value_; return *this; }
            constexpr self_type& operator=(self_type&& other) noexcept
                { value_ = other.value_; return *this; }

            constexpr operator value_type() const noexcept { return value_; }

            constexpr value_type operator[](const bitrange br) const noexcept
                { return (value_ >> br.shift()) & br.mask(); }
            constexpr bool operator[](index_t i) const noexcept
                { return value_ & (static_cast<index_t>(1) << i); }

            constexpr self_type& operator+=(value_type v) noexcept { value_ += v; return *this; }
            constexpr self_type& operator-=(value_type v) noexcept { value_ -= v; return *this; }
            constexpr self_type& operator&=(value_type v) noexcept { value_ &= v; return *this; }
            constexpr self_type& operator|=(value_type v) noexcept { value_ |= v; return *this; }
            constexpr self_type& operator^=(value_type v) noexcept { value_ ^= v; return *this; }

            friend constexpr bool operator==(const self_type& lhs, const self_type& rhs) noexcept { return lhs.value_ == rhs.value_; }
            friend constexpr bool operator!=(const self_type& lhs, const self_type& rhs) noexcept { return lhs.value_ != rhs.value_; }
            friend constexpr bool operator<(const self_type& lhs, const self_type& rhs) noexcept { return lhs.value_ < rhs.value_; }
            friend constexpr bool operator>(const self_type& lhs, const self_type& rhs) noexcept { return lhs.value_ > rhs.value_; }
            friend constexpr bool operator<=(const self_type& lhs, const self_type& rhs) noexcept { return lhs.value_ <= rhs.value_; }
            friend constexpr bool operator>=(const self_type& lhs, const self_type& rhs) noexcept { return lhs.value_ >= rhs.value_; }

            friend constexpr value_type operator+(self_type lhs, self_type rhs) noexcept { return lhs.value_ + rhs.value_; }
            friend constexpr value_type operator-(self_type lhs, self_type rhs) noexcept { return lhs.value_ - rhs.value_; }
            friend constexpr value_type operator&(self_type lhs, self_type rhs) noexcept { return lhs.value_ & rhs.value_; }
            friend constexpr value_type operator|(self_type lhs, self_type rhs) noexcept { return lhs.value_ | rhs.value_; }
            friend constexpr value_type operator^(self_type lhs, self_type rhs) noexcept { return lhs.value_ ^ rhs.value_; }

            friend constexpr void swap(self_type& lhs, self_type& rhs) noexcept { std::swap(lhs.value_, rhs.value_); }
        private:
            value_type value_ = 0;
    };

    template<unsigned N, typename T = meta::sized_uint_t<N>>
    class flags_reg {
        using self_type = flags_reg<N>;

        public:
            using value_type = T;
            using flag_reference = typename bitfield<N>::reference;

            constexpr flags_reg() noexcept : value_{0} {}
            constexpr flags_reg(value_type value) noexcept : value_{value} {}
            constexpr flags_reg(const self_type& other) noexcept : value_{other.value_} {}
            constexpr flags_reg(self_type&& other) noexcept : value_{other.value_} {}

            constexpr self_type& operator=(const self_type& other) noexcept
                {value_ = other.value_; return *this; }
            constexpr self_type& operator=(self_type&& other) noexcept
                { value_ = other.value_; return *this; }

            constexpr operator value_type() const noexcept { return value_type(value_); }

            constexpr value_type operator[](const bitrange br) const noexcept
                { return (value_type(value_) >> br.shift()) & br.mask(); }

            constexpr flag_reference operator[](index_t i) noexcept { return value_[i]; }
            constexpr bool operator[](index_t i) const noexcept { return value_[i]; }

            constexpr value_type operator&=(value_type v) noexcept { return value_ &= v; }
            constexpr value_type operator|=(value_type v) noexcept { return value_ |= v; }
            constexpr value_type operator^=(value_type v) noexcept { return value_ ^= v; }

            friend constexpr bool operator==(const self_type& lhs, const self_type& rhs) noexcept { return lhs.value_ == rhs.value_; }
            friend constexpr bool operator!=(const self_type& lhs, const self_type& rhs) noexcept { return lhs.value_ != rhs.value_; }
            friend constexpr bool operator<(const self_type& lhs, const self_type& rhs) noexcept { return lhs.value_ < rhs.value_; }
            friend constexpr bool operator>(const self_type& lhs, const self_type& rhs) noexcept { return lhs.value_ > rhs.value_; }
            friend constexpr bool operator<=(const self_type& lhs, const self_type& rhs) noexcept { return lhs.value_ <= rhs.value_; }
            friend constexpr bool operator>=(const self_type& lhs, const self_type& rhs) noexcept { return lhs.value_ >= rhs.value_; }

            friend constexpr value_type operator&(self_type lhs, self_type rhs) noexcept { return lhs.value_ & rhs.value_; }
            friend constexpr value_type operator|(self_type lhs, self_type rhs) noexcept { return lhs.value_ | rhs.value_; }
            friend constexpr value_type operator^(self_type lhs, self_type rhs) noexcept { return lhs.value_ ^ rhs.value_; }

            friend constexpr void swap(self_type& lhs, self_type& rhs) noexcept { swap(lhs.value_, rhs.value_); }
        private:
            bitfield<N> value_ = 0;
    };

    class base_register_file {
        // Abstract base class for all processor register files

        public:
            virtual void clear() = 0;
    };
}

#endif  /* _UCLE_CORE_FNSIM_REGISTERS_HPP_ */
