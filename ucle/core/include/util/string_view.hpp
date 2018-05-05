#ifndef _UCLE_CORE_UTIL_STRING_VIEW_HPP_
#define _UCLE_CORE_UTIL_STRING_VIEW_HPP_

#include <cstring>
#include <functional>
#include <string_view>
#include <vector>

namespace ucle::util {

    using char_predicate = std::function<bool(char)>;
    using view_predicate = std::function<bool(std::string_view)>;

    // auto split(std::string_view input, const char* delim)
    // {
    //     std::vector<std::string_view> parts;
    //     std::size_t last_pos = 0, pos = 0;
    //     auto delim_len = strlen(delim);

    //     while ((pos = input.find(delim, last_pos)) != std::string_view::npos) {
    //         auto item = trim(input.substr(last_pos, pos - last_pos + 1 - delim_len));

    //         if (item.length() > 0)
    //             parts.push_back(item);

    //         last_pos = pos + 1;
    //     }

    //     return parts;
    // }

    // auto split_on_any_of(std::string_view input, const char* delims)
    // {
    //     std::vector<std::string_view> parts;
    //     std::size_t last_pos = 0, pos = 0;

    //     while ((pos = input.find_first_of(delims, last_pos)) != std::string_view::npos) {
    //         auto item = trim(input.substr(last_pos, pos - last_pos));

    //         if (item.length() > 0)
    //             parts.push_back(item);

    //         last_pos = pos + 1;
    //     }

    //     return parts;
    // }

    // using split_result = std::pair<std::string_view, std::string_view>;

    // split_result split_once(std::string_view input, const char* delim)
    // {
    //     auto pos = input.find(delim);
    //     if (pos == std::string_view::npos)
    //         return { input, {} };

    //     return { input.substr(0, pos), input.substr(pos + strlen(delim)) };
    // }

    // Predicates

    bool starts_with(std::string_view input, std::string_view x)
    {
        auto len = input.length();
        auto xlen = x.length();

        if (len < xlen) return false;

        auto i = 0u;
        for (auto i = 0u; i < xlen && input[i] == x[i]; ++i);

        return i == xlen;
    }

    bool starts_with(std::string_view input, const char* x)
    {
        auto len = input.length();
        auto xlen = strlen(x);

        if (len < xlen) return false;

        auto i = 0u;
        for (; i < xlen && input[i] == x[i]; ++i);

        return i == xlen;
    }

    bool starts_with(std::string_view input, char x)
    {
        return input.length() > 0 && input[0] == x;
    }

    bool ends_with(std::string_view input, std::string_view x)
    {
        auto len = input.length();
        auto xlen = x.length();

        if (len < xlen) return false;

        auto i = 0u;
        for (; i < xlen && input[len - i - 1] == x[xlen - i - 1]; ++i);

        return i == xlen;
    }

    bool ends_with(std::string_view input, const char* x)
    {
        auto len = input.length();
        auto xlen = strlen(x);

        if (len < xlen) return false;

        auto i = 0u;
        for (; i < xlen && input[len - i - 1] == x[xlen - i - 1]; ++i);

        return i == xlen;
    }

    bool ends_with(std::string_view input, char x)
    {
        return input.length() > 0 && input[input.length() - 1] == x;
    }

    // Selectors

    auto take_while(std::string_view input, char_predicate p)
    {
        auto len = input.length();
        int i;

        for (i = 0; i < len && p(input[i]); ++i);

        return input.substr(0, i);
    }

    auto take_while(std::string_view input, view_predicate p)
    {
        auto len = input.length();
        int i;

        for (i = 0; i < len && p(input.substr(i)); ++i);

        return input.substr(0, i);
    }

    auto drop_while(std::string_view input, char_predicate p)
    {
        auto len = input.length();
        int i;

        for (i = 0; i < len && p(input[i]); ++i);

        return input.substr(i);
    }

    auto drop_while(std::string_view input, view_predicate p)
    {
        auto len = input.length();
        int i;

        for (i = 0; i < len && p(input.substr(i)); ++i);

        return input.substr(i);
    }

    auto take_while_r(std::string_view input, char_predicate p)
    {
        auto len = input.length();
        int i;

        for (i = len - 1; i >= 0 && p(input[i]); --i);

        return input.substr(i + 1);
    }

    auto take_while_r(std::string_view input, view_predicate p)
    {
        auto len = input.length();
        int i;

        for (i = len - 1; i >= 0 && p(input.substr(0, i + 1)); --i);

        return input.substr(i + 1);
    }

    auto drop_while_r(std::string_view input, char_predicate p)
    {
        auto len = input.length();
        int i;

        for (i = len - 1; i >= 0 && p(input[i]); --i);

        return input.substr(0, i + 1);
    }

    auto drop_while_r(std::string_view input, view_predicate p)
    {
        auto len = input.length();
        int i;

        for (i = len - 1; i >= 0 && p(input.substr(0, i + 1)); --i);

        return input.substr(0, i + 1);
    }

    auto trim(std::string_view input)
    {
        return drop_while_r(drop_while(input, isspace), isspace);
    }
}

#endif  /* _UCLE_CORE_UTIL_STRING_VIEW_HPP_ */
