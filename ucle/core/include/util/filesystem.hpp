#ifndef _UCLE_CORE_UTIL_FILESYSTEM_HPP_
#define _UCLE_CORE_UTIL_FILESYSTEM_HPP_

#include <common/types.hpp>

#include <libs/fmt/format.h>

#include <util/string_view.hpp>

#include <cerrno>
#include <cstdio>
#include <cstring>

namespace ucle::util {

    class filesystem_error : public base_exception { using base_exception::base_exception; };

    auto open_file(const char* filename, const char* mode)
    {
        auto handle = fopen(filename, mode);
        if (!handle)
            throw filesystem_error(fmt::format("Error, cannot open file `{}`: {}.", filename, strerror(errno)));
        return handle;
    }

    void close_file(FILE* handle)
    {
        fclose(handle);
    }

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
