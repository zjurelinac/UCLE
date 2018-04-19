#ifndef _UCLE_CORE_UTIL_STRING_HPP_
#define _UCLE_CORE_UTIL_STRING_HPP_

#include <common/meta.hpp>

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

namespace ucle::util {

    namespace detail {

        template <typename T>
        struct parse_int {
            static T parse(const std::string& str, std::size_t* idx = 0, int base = 10);
        };

        template <>
        int parse_int<int>::parse(const std::string& str, std::size_t* idx, int base) { return std::stoi(str, idx, base); }

        template <>
        long parse_int<long>::parse(const std::string& str, std::size_t* idx, int base) { return std::stol(str, idx, base); }

        template <>
        long long parse_int<long long>::parse(const std::string& str, std::size_t* idx, int base) { return std::stoll(str, idx, base); }

        template <>
        unsigned parse_int<unsigned>::parse(const std::string& str, std::size_t* idx, int base) { return std::stoi(str, idx, base); }

        template <>
        unsigned long parse_int<unsigned long>::parse(const std::string& str, std::size_t* idx, int base) { return std::stoul(str, idx, base); }

        template <>
        unsigned long long parse_int<unsigned long long>::parse(const std::string& str, std::size_t* idx, int base) { return std::stoull(str, idx, base); }
    }

    // trim from start (in place)
    inline void ltrim(std::string &s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
        }));
    }

    // trim from end (in place)
    inline void rtrim(std::string &s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    // trim from both ends (in place)
    inline void trim(std::string &s)
    {
        ltrim(s);
        rtrim(s);
    }

    // trim from start (copying)
    inline std::string ltrim_copy(std::string s)
    {
        ltrim(s);
        return s;
    }

    // trim from end (copying)
    inline std::string rtrim_copy(std::string s)
    {
        rtrim(s);
        return s;
    }

    // trim from both ends (copying)
    inline std::string trim_copy(std::string s)
    {
        trim(s);
        return s;
    }

    // split string into words
    inline std::vector<std::string> split(const std::string& s, char delim = ' ')
    {
        std::istringstream iss {s};
        std::vector<std::string> tokens;
        std::string token;

        while (std::getline(iss, token, delim)) {
            tokens.push_back(token);
        }

        return tokens;
    }

    template <typename T, typename = meta::is_integer_t<T>>
    inline bool parse_int(const std::string& str, T* num_ptr, int base = 0)
    {
        try {
            std::size_t pos;
            *num_ptr = detail::parse_int<T>::parse(str, &pos, base);
            return pos == str.size();
        } catch (std::exception) {
            num_ptr = nullptr;
            return false;
        }
    }

}

#endif  /* _UCLE_CORE_UTIL_STRING_HPP_ */
