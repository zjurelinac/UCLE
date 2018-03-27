#ifndef _UCLE_CORE_COMMON_STRUCTURES_HPP_
#define _UCLE_CORE_COMMON_STRUCTURES_HPP_

#include <common/types.hpp>

#include <array>

namespace ucle {

    template <typename T, size_t vector_size = 8>
    class small_vector {
        public:
            using value_type = T;
            using size_type = size_t;
            using difference_type = std::ptrdiff_t;
            using reference = value_type&;
            using const_reference = const value_type&;

            small_vector() {}
            small_vector(size_t size) : tsize_(size) {}
            small_vector(const small_vector<T, vector_size>& other)
                : tsize_(other.tsize_), data_(other.data_) {}
            small_vector(small_vector<T, vector_size>&& other)
                : tsize_(other.tsize_), data_(std::move(other.data_)) {}

            small_vector<T, vector_size>& operator=(const small_vector<T, vector_size>& other)
                { if (*this != other) { tsize_ = other.tsize_; data_ = other.data_; } return *this; }
            small_vector<T, vector_size>& operator=(small_vector<T, vector_size>&& other)
                { if (*this != other) { tsize_ = other.tsize_; data_ = std::move(other.data_); } return *this; }

            reference at(index_t idx) { return data_[idx]; }
            const_reference at(index_t idx) const { return data_[idx]; }

            reference operator[](index_t idx) { return data_[idx]; }
            const_reference operator[](index_t idx) const { return data_[idx]; }

            bool empty() const { return tsize_ == 0; }
            size_t size() const { return tsize_; }

            void clear() { tsize_ = 0; }
            void resize(size_t size) { tsize_ = size; }
            void push_back(T value) { data_[tsize_++] = value; }
            void pop_back() { --tsize_; }

            // TODO: Implement iterators

        private:
            int tsize_ = 0;
            std::array<T, vector_size> data_ = {0};
    };

    using small_byte_vector = small_vector<byte_t, 8>;

}

#endif  /* _UCLE_CORE_COMMON_STRUCTURES_HPP_ */
