#ifndef _UCLE_CORE_SIMULATORS_FUNCTIONAL_REGISTER_HPP_
#define _UCLE_CORE_SIMULATORS_FUNCTIONAL_REGISTER_HPP_

#include <common/types.hpp>

namespace ucle::fnsim {

    namespace detail {
        template<typename ValueType>
        class reg_inst_ {
            public:
                using value_type = ValueType;

                reg_inst_() : value_(0) {}
                reg_inst_(value_type value) : value_(value) {}
                reg_inst_(const reg_inst_<value_type>& other) : value_(other.value_) {}
                reg_inst_(reg_inst_<value_type>&& other) : value_(other.value_) {}

                reg_inst_<value_type>& operator=(const value_type& value)
                    { value_ = value; return *this; }
                reg_inst_<value_type>& operator=(const reg_inst_<value_type>& other)
                    { if (this != &other) value_ = other.value; return *this; }
                reg_inst_<value_type>& operator=(reg_inst_<value_type>&& other)
                    { if (this != &other) value_ = other.value; return *this; }

                void set(value_type value) { value_ = value; }
                value_type get() const { return value_; }

                // TODO: Bitrange

            protected:
                value_type value_;
        };
    }

    template<unsigned bits> class reg {};

    template<> class reg<8>  : public detail::reg_inst_<byte_t> {
        public: using detail::reg_inst_<byte_t>::operator=;
    };

    template<> class reg<16> : public detail::reg_inst_<half_t> {
        public: using detail::reg_inst_<half_t>::operator=;
    };

    template<> class reg<32> : public detail::reg_inst_<word_t> {
        public: using detail::reg_inst_<word_t>::operator=;
    };

    template <unsigned bits>
    class flags_reg {
        protected:

    };

    class register_file {
        // Abstract base class for all processor register files

        public:
            virtual void clear() = 0;
    };
}

#endif  /* _UCLE_CORE_SIMULATORS_FUNCTIONAL_REGISTER_HPP_ */
