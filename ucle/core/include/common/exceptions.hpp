#ifndef _UCLE_CORE_COMMON_EXCEPTIONS_HPP_
#define _UCLE_CORE_COMMON_EXCEPTIONS_HPP_

#include <exception>
#include <string>

namespace ucle {

    class base_exception : public std::exception {
        public:
            base_exception(std::string desc) : desc_(desc) {}
            virtual const char* what() const noexcept override { return desc_.c_str(); }
        private:
            std::string desc_;
    };

    class invalid_address_range : public base_exception { using base_exception::base_exception; };
    class invalid_memory_access : public base_exception { using base_exception::base_exception; };
    class invalid_identifier : public base_exception { using base_exception::base_exception; };
}

#endif /* _UCLE_CORE_COMMON_EXCEPTIONS_HPP_ */
