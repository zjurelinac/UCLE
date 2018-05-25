#ifndef _UCLE_CORE_FNSIM_EXCEPTIONS_HPP_
#define _UCLE_CORE_FNSIM_EXCEPTIONS_HPP_

#include <common/types.hpp>

#include <exception>
#include <string>

namespace ucle::fnsim {

    class invalid_address_range : public base_exception { using base_exception::base_exception; };
    class invalid_memory_access : public base_exception { using base_exception::base_exception; };
    class invalid_identifier    : public base_exception { using base_exception::base_exception; };
    class unsupported_feature   : public base_exception { using base_exception::base_exception; };

    class malformed_argument    : public base_exception { using base_exception::base_exception; };
    class malformed_check       : public base_exception { using base_exception::base_exception; };
    class unknown_command       : public base_exception { using base_exception::base_exception; };
    class incorrect_call        : public base_exception { using base_exception::base_exception; };
    class json_parse_error      : public base_exception { using base_exception::base_exception; };

    class impossible_value      : public base_exception { using base_exception::base_exception; };

    class runtime_error         : public base_exception { using base_exception::base_exception; };
}

#endif /* _UCLE_CORE_FNSIM_EXCEPTIONS_HPP_ */
