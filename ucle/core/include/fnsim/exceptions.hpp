#ifndef _UCLE_CORE_FNSIM_EXCEPTIONS_HPP_
#define _UCLE_CORE_FNSIM_EXCEPTIONS_HPP_

#include <common/types.hpp>

#include <exception>
#include <string>

namespace ucle::fnsim {

    class invalid_address_range : public base_exception { using base_exception::base_exception; };
    class invalid_memory_access : public base_exception { using base_exception::base_exception; };
    class invalid_identifier : public base_exception { using base_exception::base_exception; };
}

#endif /* _UCLE_CORE_FNSIM_EXCEPTIONS_HPP_ */