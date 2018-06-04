#ifndef _UCLE_CORE_UTIL_FILESYSTEM_HPP_
#define _UCLE_CORE_UTIL_FILESYSTEM_HPP_

#include <util/string_view.hpp>

namespace ucle::util {

    std::string strip_extension(std::string filename)
    {
        std::string noext { drop_while_r(filename, [](char last) { return last != '.'; } ) };
        return noext.length() > 0 ? noext.substr(0, noext.length() - 1) : filename;
    }

    std::string change_extension(std::string filename, std::string new_ext)
    {
        return strip_extension(filename) + "." + new_ext;
    }

}

#endif  /* _UCLE_CORE_UTIL_FILESYSTEM_HPP_ */
