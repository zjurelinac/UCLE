#ifndef _UCLE_CORE_FNSIM_REGISTER_HPP_
#define _UCLE_CORE_FNSIM_REGISTER_HPP_

#include <common/meta.hpp>
#include <common/structures.hpp>
#include <common/types.hpp>

namespace ucle::fnsim {

    template<unsigned bits, typename T = meta::sized_uint<bits>>
    class reg {
        public:
            using value_type = T;

            constexpr reg() : value_(0) {}
            constexpr reg(value_type value) : value_(value) {}
            constexpr reg(const reg<bits>& other) : value_(other.value_) {}
            constexpr reg(reg<bits>&& other) : value_(other.value_) {}

            reg<bits>& operator=(value_type value)
                { value_ = value; return *this; }
            reg<bits>& operator=(const reg<bits>& other)
                {value_ = other.value_; return *this; }
            reg<bits>& operator=(reg<bits>&& other)
                { value_ = other.value_; return *this; }

            constexpr explicit operator value_type() const { return value_; }

            constexpr value_type operator[](const bitrange br) const
                { return (value_ >> br.shift()) & br.mask(); }
            constexpr bool operator[](const index_t i) const
                { return value_ & (static_cast<index_t>(1) << i); }

            constexpr value_type operator+=(value_type v) { return value_ += v; }
            constexpr value_type operator-=(value_type v) { return value_ -= v; }
            constexpr value_type operator&=(value_type v) { return value_ &= v; }
            constexpr value_type operator|=(value_type v) { return value_ |= v; }
            constexpr value_type operator^=(value_type v) { return value_ ^= v; }

            constexpr bool operator<(value_type v) const { return value_ < v; }
            constexpr bool operator>(value_type v) const { return value_ > v; }
            constexpr bool operator<=(value_type v) const { return value_ <= v; }
            constexpr bool operator>=(value_type v) const { return value_ >= v; }
            constexpr bool operator==(value_type v) const { return value_ == v; }
            constexpr bool operator!=(value_type v) const { return value_ != v; }

            constexpr value_type get() const { return value_; }

        protected:
            value_type value_;
    };

    template <unsigned bits>
    class flags_reg {
        public:

        private:
            bitfield<bits> bf_;
    };

    /*template <unsigned bits>
    class flags_reg {
        public:

            constexpr flags_reg() {}
            template <typename T, std::enable_if_t<std::is_integral_v<T>>>
            constexpr flags_reg(T value) : flags_(value) {}
            constexpr flags_reg(const flags_reg<bits>& other) : flags_(other.flags_) {}
            constexpr flags_reg(flags_reg<bits>&& other) : flags_(std::move(other.flags_)) {}

            template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
            flags_reg<bits>& operator=(T value)
                { flags_ = std::bitset(value); return *this; }
            flags_reg<bits>& operator=(const flags_reg<bits>& other)
                { if (this != &other) flags_ = other.flags_; return *this; }
            flags_reg<bits>& operator=(flags_reg<bits>&& other)
                { if (this != &other) flags_ = std::move(other.flags_); return *this; }

            constexpr bool operator[](const index_t i) const
                { return flags_[i]; }

            template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
            constexpr explicit operator T() const;

            constexpr flags_reg<bits>& operator&=(value_type v) { flags_ &= std::bitset(v); return *this; }
            constexpr flags_reg<bits>& operator|=(value_type v) { flags_ |= std::bitset(v); return *this; }
            constexpr flags_reg<bits>& operator^=(value_type v) { flags_ ^= std::bitset(v); return *this; }

            template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
            constexpr T get() const { return static_cast<T>(flags_.to_ulong()); }

        protected:
            std::bitset<bits> flags_;
    };*/

    /*template <> template <>
    constexpr word_t flags_reg<64>::get<dword_t>() const { return static_cast<dword_t>(flags_.to_ullong()); }*/


    class register_file {
        // Abstract base class for all processor register files

        public:
            virtual void clear() = 0;
    };
}

#endif  /* _UCLE_CORE_SIMULATORS_FUNCTIONAL_REGISTER_HPP_ */
