#pragma once

#include <common/types.hpp>

#include <libs/fmt/format.h>
#include <libs/nlohmann/json.hpp>

namespace ucle::util {

    class json_error : public base_exception { using base_exception::base_exception; };

    auto get(nlohmann::json j, std::string key)
    {
        if (!j.count(key))
            throw json_error(fmt::format("Key '{}' missing from JSON object", key));

        return j[key];
    }

    template <typename T>
    T get(nlohmann::json j, std::string key, T default_val)
    {
        if (!j.count(key))
            return default_val;

        return j[key];
    }

}
