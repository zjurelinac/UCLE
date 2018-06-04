#ifndef _UCLE_CORE_UTIL_STRING_VIEW_HPP_
#define _UCLE_CORE_UTIL_STRING_VIEW_HPP_

#include <cctype>
#include <cstring>
#include <functional>
#include <string_view>
#include <vector>

namespace ucle::util {

    using char_predicate = std::function<bool(char)>;
    using view_predicate = std::function<bool(std::string_view)>;

    // Comparators

    bool iequals(const std::string_view& x, const std::string_view& y) {
        if (x.length() != y.length())
            return false;

        for (auto i = 0u; i < x.length(); ++i)
            if (tolower(x[i]) != tolower(y[i]))
                return false;

        return true;
    }

    bool iequals(const std::string_view& x, const char* y) {
        if (x.length() != strlen(y))
            return false;

        for (auto i = 0u; i < x.length(); ++i)
            if (tolower(x[i]) != tolower(y[i]))
                return false;

        return true;
    }

    // Predicates

    bool starts_with(const std::string_view& input, const std::string_view& x)
    {
        auto len = input.length();
        auto xlen = x.length();

        if (len < xlen) return false;

        auto i = 0u;
        for (auto i = 0u; i < xlen && input[i] == x[i]; ++i);

        return i == xlen;
    }

    bool starts_with(const std::string_view& input, const char* x)
    {
        auto len = input.length();
        auto xlen = strlen(x);

        if (len < xlen) return false;

        auto i = 0u;
        for (; i < xlen && input[i] == x[i]; ++i);

        return i == xlen;
    }

    bool starts_with(const std::string_view& input, char x)
    {
        return input.length() > 0 && input[0] == x;
    }

    bool ends_with(const std::string_view& input, const std::string_view& x)
    {
        auto len = input.length();
        auto xlen = x.length();

        if (len < xlen) return false;

        auto i = 0u;
        for (; i < xlen && input[len - i - 1] == x[xlen - i - 1]; ++i);

        return i == xlen;
    }

    bool ends_with(const std::string_view& input, const char* x)
    {
        auto len = input.length();
        auto xlen = strlen(x);

        if (len < xlen) return false;

        auto i = 0u;
        for (; i < xlen && input[len - i - 1] == x[xlen - i - 1]; ++i);

        return i == xlen;
    }

    bool ends_with(const std::string_view& input, char x)
    {
        return input.length() > 0 && input[input.length() - 1] == x;
    }

    bool istarts_with(const std::string_view& input, const std::string_view& x)
    {
        auto len = input.length();
        auto xlen = x.length();

        if (len < xlen) return false;

        auto i = 0u;
        for (auto i = 0u; i < xlen && tolower(input[i]) == tolower(x[i]); ++i);

        return i == xlen;
    }

    bool istarts_with(const std::string_view& input, const char* x)
    {
        auto len = input.length();
        auto xlen = strlen(x);

        if (len < xlen) return false;

        auto i = 0u;
        for (; i < xlen && tolower(input[i]) == tolower(x[i]); ++i);

        return i == xlen;
    }

    bool istarts_with(const std::string_view& input, char x)
    {
        return input.length() > 0 && tolower(input[0]) == tolower(x);
    }

    bool iends_with(const std::string_view& input, const std::string_view& x)
    {
        auto len = input.length();
        auto xlen = x.length();

        if (len < xlen) return false;

        auto i = 0u;
        for (; i < xlen && tolower(input[len - i - 1]) == tolower(x[xlen - i - 1]); ++i);

        return i == xlen;
    }

    bool iends_with(const std::string_view& input, const char* x)
    {
        auto len = input.length();
        auto xlen = strlen(x);

        if (len < xlen) return false;

        auto i = 0u;
        for (; i < xlen && tolower(input[len - i - 1]) == tolower(x[xlen - i - 1]); ++i);

        return i == xlen;
    }

    bool iends_with(const std::string_view& input, char x)
    {
        return input.length() > 0 && tolower(input[input.length() - 1]) == tolower(x);
    }

    // Selectors

    auto take_while(const std::string_view& input, char_predicate p)
    {
        auto len = input.length();
        unsigned i;

        for (i = 0; i < len && p(input[i]); ++i);

        return input.substr(0, i);
    }

    auto take_while(const std::string_view& input, view_predicate p)
    {
        auto len = input.length();
        unsigned i;

        for (i = 0; i < len && p(input.substr(i)); ++i);

        return input.substr(0, i);
    }

    auto drop_while(const std::string_view& input, char_predicate p)
    {
        auto len = input.length();
        unsigned i;

        for (i = 0; i < len && p(input[i]); ++i);

        return input.substr(i);
    }

    auto drop_while(const std::string_view& input, view_predicate p)
    {
        auto len = input.length();
        unsigned i;

        for (i = 0; i < len && p(input.substr(i)); ++i);

        return input.substr(i);
    }

    auto take_while_r(const std::string_view& input, char_predicate p)
    {
        auto len = input.length();
        int i;

        for (i = len - 1; i >= 0 && p(input[i]); --i);

        return input.substr(i + 1);
    }

    auto take_while_r(const std::string_view& input, view_predicate p)
    {
        auto len = input.length();
        int i;

        for (i = len - 1; i >= 0 && p(input.substr(0, i + 1)); --i);

        return input.substr(i + 1);
    }

    auto drop_while_r(const std::string_view& input, char_predicate p)
    {
        auto len = input.length();
        int i;

        for (i = len - 1; i >= 0 && p(input[i]); --i);

        return input.substr(0, i + 1);
    }

    auto drop_while_r(const std::string_view& input, view_predicate p)
    {
        auto len = input.length();
        int i;

        for (i = len - 1; i >= 0 && p(input.substr(0, i + 1)); --i);

        return input.substr(0, i + 1);
    }

    auto trim(const std::string_view& input)
    {
        return drop_while_r(drop_while(input, isspace), isspace);
    }

    auto split(std::string_view input, char_predicate p)
    {
        std::vector<std::string_view> parts;

        while (!input.empty()) {
            auto part = take_while(input, std::not_fn(p));
            parts.push_back(part);

            input = input.substr(part.length() + 1); // drop_while(input.substr(part.length()), p);
        }

        return parts;
    }
}

#endif  /* _UCLE_CORE_UTIL_STRING_VIEW_HPP_ */
