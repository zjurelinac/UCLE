#ifndef _UCLE_CORE_COMMON_STRUCTURES_HPP_
#define _UCLE_CORE_COMMON_STRUCTURES_HPP_

#include <common/types.hpp>
#include <common/meta.hpp>

#include <util/const_bit_util.hpp>

#include <array>

namespace ucle {

    template <unsigned N>
    class bitfield {
        public:
            using value_type = meta::sized_uint<N>;
            using cbu = util::const_bit_util<value_type>;

            class reference {
                public:
                    reference() = delete;
                    constexpr reference(value_type& val, index_t idx) : val_(val), idx_(idx) {}
                    constexpr reference(const reference& other) : val_(other.val_), idx_(other.idx_) {}
                    constexpr reference(reference&& other) : val_(other.val_), idx_(other.idx_) {}

                    reference& operator=(bool val)
                        { set(val); return *this; }
                    reference& operator=(const reference& ref)
                        { set(ref.get()); return *this; }
                    reference& operator=(reference&& ref)
                        { set(ref.get()); return *this; }

                    operator bool() const
                        { return get() != 0; }

                    constexpr bool operator==(const reference& other) const
                        { return val_ == other.val_ && idx_ == other.idx_; }
                    constexpr bool operator!=(const reference& other) const
                        { return val_ != other.val_ || idx_ != other.idx_; }

                    bool operator~() const
                        { return !get(); }

                    reference& flip()
                        { set(!get()); return *this; }

                private:
                    constexpr bool get() const { return val_ & cbu::nth_bit(idx_); }
                    void set(bool x)
                    {
                        if (x) val_ |=  cbu::nth_bit(idx_);
                        else   val_ &= ~cbu::nth_bit(idx_);
                    };

                    value_type& val_;
                    index_t idx_;
            };

            constexpr bitfield() {}
            constexpr bitfield(value_type value) : value_(value) {}
            constexpr bitfield(const bitfield<N>& other) : value_(other.value_) {}
            constexpr bitfield(bitfield<N>&& other) : value_(other.value_) {}

            constexpr bitfield<N>& operator=(value_type value)
                { value_ = value; return *this; }
            constexpr bitfield<N>& operator=(const bitfield<N>& other)
                { value_ = other.value_; return *this; }
            constexpr bitfield<N>& operator=(bitfield<N>&& other)
                { value_ = other.value_; return *this; }

            constexpr explicit operator value_type() const { return value_; }

            constexpr bool operator==(value_type value) const
                { return value_ == value; }
            constexpr bool operator!=(value_type value) const
                { return value_ != value; }
            constexpr bool operator==(const bitfield<N>& other) const
                { return value_ == other.value_; }
            constexpr bool operator!=(const bitfield<N>& other) const
                { return value_ != other.value_; }

            constexpr bool operator[](index_t idx) const
                { return value_ & cbu::nth_bit(idx); }
            constexpr reference operator[](index_t idx)
                { return reference(value_, idx); }

            constexpr bitfield<N>& operator&=(const bitfield<N>& other)
                { value_ &= other.value_; return *this; }
            constexpr bitfield<N>& operator|=(const bitfield<N>& other)
                { value_ |= other.value_; return *this; }
            constexpr bitfield<N>& operator^=(const bitfield<N>& other)
                { value_ ^= other.value_; return *this; }
            constexpr bitfield<N> operator~() const
                { return { ~value_ }; }

            constexpr bitfield<N> operator<<(size_t shift) const
                { return { value_ << shift }; }
            constexpr bitfield<N> operator>>(size_t shift) const
                { return { value_ >> shift }; }
            constexpr bitfield<N>& operator<<=(size_t shift)
                { value_ <<= shift; return *this; }
            constexpr bitfield<N>& operator>>=(size_t shift)
                { value_ >>= shift; return *this; }

            constexpr bool test(index_t idx) const
                { return value_ & cbu::nth_bit(idx); }

            constexpr bool all() const { return value_ == cbu::all_bits(); }
            constexpr bool any() const { return value_ != 0; }
            constexpr bool none() const { return value_ == 0; }

            // size_t count() const { /* TODO */ }

            constexpr size_t size() const { return N; }

            constexpr bitfield<N>& reset()
                { value_ = 0; return *this; }
            constexpr bitfield<N>& reset(index_t idx)
                { value_ &= ~cbu::nth_bit(idx); return *this; }

            constexpr bitfield<N>& flip()
                { value_ = ~value_; return *this; }
            constexpr bitfield<N>& flip(index_t idx)
                { value_[idx].flip(); return *this; }

        private:
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

            constexpr small_vector() {}
            constexpr small_vector(size_t size) : tsize_(size) {}
            constexpr small_vector(const small_vector<T, N>& other)
                : tsize_(other.tsize_), data_(other.data_) {}
            constexpr small_vector(small_vector<T, N>&& other)
                : tsize_(other.tsize_), data_(std::move(other.data_)) {}

            constexpr small_vector<T, N>& operator=(const small_vector<T, N>& other)
                { if (*this != other) { tsize_ = other.tsize_; data_ = other.data_; } return *this; }
            constexpr small_vector<T, N>& operator=(small_vector<T, N>&& other)
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

        private:
            int tsize_ = 0;
            std::array<T, N> data_ = {0};
    };

    using small_byte_vector = small_vector<byte_t, 8>;

}

#endif  /* _UCLE_CORE_COMMON_STRUCTURES_HPP_ */
