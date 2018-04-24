#ifndef _UCLE_CORE_COMMON_STRUCTURES_HPP_
#define _UCLE_CORE_COMMON_STRUCTURES_HPP_

#include <common/types.hpp>
#include <common/meta.hpp>

#include <util/const_bin_util.hpp>

#include <array>

namespace ucle {

    template <unsigned N>
    class bitfield {
        public:
            using value_type = meta::sized_uint_t<N>;
            using cbu = util::const_bin_util<value_type>;

            class reference {
                public:
                    reference() = delete;
                    constexpr reference(value_type& val, index_t idx) noexcept : val_(val), idx_(idx) {}
                    constexpr reference(const reference& other) noexcept : val_(other.val_), idx_(other.idx_) {}
                    constexpr reference(reference&& other) noexcept : val_(other.val_), idx_(other.idx_) {}

                    constexpr reference& operator=(bool val) noexcept
                        { set_(val); return *this; }
                    constexpr reference& operator=(const reference& ref) noexcept
                        { set_(ref.get_()); return *this; }
                    constexpr reference& operator=(reference&& ref) noexcept
                        { set_(ref.get_()); return *this; }

                    constexpr operator bool() const { return get_(); }
                    constexpr bool operator~() const { return !get_(); }

                    constexpr reference& flip() { set_(!get_()); return *this; }

                    friend constexpr bool operator==(const reference& lhs, const reference& rhs) noexcept
                        { return lhs.val_ == rhs.val_ && lhs.idx_ == rhs.idx_; }
                    friend constexpr bool operator!=(const reference& lhs, const reference& rhs) noexcept
                        { return lhs.val_ != rhs.val_ || lhs.idx_ != rhs.idx_; }

                private:
                    constexpr bool get_() const { return cbu::nth_bit_of(val_, idx_); }
                    constexpr void set_(bool x)
                    {
                        if (x) val_ |=  cbu::nth_bit(idx_);
                        else   val_ &= ~cbu::nth_bit(idx_);
                    };

                    value_type& val_;
                    index_t idx_;
            };

            constexpr bitfield() noexcept {}
            constexpr bitfield(value_type value) noexcept : value_(value) {}
            constexpr bitfield(const bitfield<N>& other) noexcept : value_(other.value_) {}
            constexpr bitfield(bitfield<N>&& other) noexcept : value_(other.value_) {}

            constexpr bitfield<N>& operator=(value_type value) noexcept
                { value_ = value; return *this; }
            constexpr bitfield<N>& operator=(const bitfield<N>& other) noexcept
                { value_ = other.value_; return *this; }
            constexpr bitfield<N>& operator=(bitfield<N>&& other) noexcept
                { value_ = other.value_; return *this; }

            constexpr explicit operator value_type() const { return value_; }

            constexpr bool operator[](index_t idx) const
                { return cbu::nth_bit_of(value_, idx); }
            constexpr reference operator[](index_t idx)
                { return reference(value_, idx); }

            constexpr bitfield<N>& operator&=(const bitfield<N>& other) noexcept
                { value_ &= other.value_; return *this; }
            constexpr bitfield<N>& operator|=(const bitfield<N>& other) noexcept
                { value_ |= other.value_; return *this; }
            constexpr bitfield<N>& operator^=(const bitfield<N>& other) noexcept
                { value_ ^= other.value_; return *this; }
            constexpr bitfield<N> operator~() const
                { return { ~value_ }; }

            constexpr bitfield<N> operator<<(size_t shift) const noexcept
                { return { value_ << shift }; }
            constexpr bitfield<N> operator>>(size_t shift) const noexcept
                { return { value_ >> shift }; }
            constexpr bitfield<N>& operator<<=(size_t shift) noexcept
                { value_ <<= shift; return *this; }
            constexpr bitfield<N>& operator>>=(size_t shift) noexcept
                { value_ >>= shift; return *this; }

            // size_t count() const noexcept { /* TODO */ }
            constexpr size_t size() const noexcept { return N; }

            constexpr bool test(index_t idx) const noexcept { return cbu::nth_bit_of(value_, idx); }

            constexpr bool any() const noexcept { return value_ != 0; }
            constexpr bool all() const noexcept { return cbu::all_bits_set(value_); }
            constexpr bool none() const noexcept { return value_ == 0; }

            constexpr bitfield<N>& set() noexcept
                { value_ = -1; return *this; }
            constexpr bitfield<N>& set (index_t idx, bool val = true)
                { (*this)[idx] = val; return *this; }

            constexpr bitfield<N>& reset() noexcept
                { value_ = 0; return *this; }
            constexpr bitfield<N>& reset(index_t idx)
                { value_ &= ~cbu::nth_bit(idx); return *this; }

            constexpr bitfield<N>& flip() noexcept
                { value_ = ~value_; return *this; }
            constexpr bitfield<N>& flip(index_t idx)
                { (*this)[idx].flip(); return *this; }

            friend constexpr bool operator==(const bitfield<N>& lhs, const bitfield<N>& rhs) noexcept { return lhs.value_ == rhs.value_; }
            friend constexpr bool operator!=(const bitfield<N>& lhs, const bitfield<N>& rhs) noexcept { return lhs.value_ != rhs.value_; }

            friend constexpr bitfield<N> operator&(const bitfield<N>& lhs, const bitfield<N>& rhs) noexcept { return lhs.value_ & rhs.value_; }
            friend constexpr bitfield<N> operator|(const bitfield<N>& lhs, const bitfield<N>& rhs) noexcept { return lhs.value_ | rhs.value_; }
            friend constexpr bitfield<N> operator^(const bitfield<N>& lhs, const bitfield<N>& rhs) noexcept { return lhs.value_ ^ rhs.value_; }

            friend constexpr void swap(bitfield<N>& lhs, bitfield<N>& rhs) noexcept { std::swap(lhs.value_, rhs.value_); }

        public:
            value_type value_ = 0;
    };


    template <typename T, size_t N = 8>
    class small_vector {
        public:
            using value_type = T;
            using size_type = size_t;
            using difference_type = std::ptrdiff_t;
            using reference = value_type&;
            using const_reference = const value_type&;

            constexpr small_vector() noexcept {}
            constexpr small_vector(size_t size) noexcept : tsize_(size) {}
            constexpr small_vector(const small_vector<T, N>& other) noexcept
                : tsize_(other.tsize_), data_(other.data_) {}
            constexpr small_vector(small_vector<T, N>&& other) noexcept
                : tsize_(other.tsize_), data_(std::move(other.data_)) {}

            constexpr small_vector<T, N>& operator=(const small_vector<T, N>& other) noexcept
                { if (*this != other) { tsize_ = other.tsize_; data_ = other.data_; } return *this; }
            constexpr small_vector<T, N>& operator=(small_vector<T, N>&& other) noexcept
                { if (*this != other) { tsize_ = other.tsize_; data_ = std::move(other.data_); } return *this; }

            constexpr reference at(index_t idx) { return data_[idx]; }
            constexpr const_reference at(index_t idx) const { return data_[idx]; }

            constexpr reference operator[](index_t idx) { return data_[idx]; }
            constexpr const_reference operator[](index_t idx) const { return data_[idx]; }

            constexpr bool empty() const { return tsize_ == 0; }
            constexpr size_t size() const { return tsize_; }
            constexpr size_t capacity() const { return N; }

            constexpr void clear() { tsize_ = 0; }
            constexpr void resize(size_t size) { tsize_ = size; }
            constexpr void push_back(T value) { data_[tsize_++] = value; }
            constexpr void pop_back() { --tsize_; }

            // TODO: Implement iterators

            friend constexpr void swap(small_vector<T, N>& lhs, small_vector<T, N>& rhs) noexcept { std::swap(lhs.data_, rhs.data_); }

        private:
            size_t tsize_ = 0;
            std::array<T, N> data_;
    };

    using small_byte_vector = small_vector<byte_t, 8>;

}

#endif  /* _UCLE_CORE_COMMON_STRUCTURES_HPP_ */
