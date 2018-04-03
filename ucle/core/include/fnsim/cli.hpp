#ifndef _UCLE_CORE_FNSIM_CLI_HPP_
#define _UCLE_CORE_FNSIM_CLI_HPP_

#include <fnsim/frisc.hpp>

#include <functional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace ucle::fnsim {

    using fnsim_factory = std::function<functional_simulator_ptr(fnsim::simulator_config)>;

    std::set<std::string> factory_options = {"frisc"};
    std::unordered_map<std::string, fnsim_factory> factory = {
        {"frisc", &make_frisc_simulator}
    };

    struct cli_config {
        std::string simulator_name;
        uint32_t fnsim_mem_size = 4096;

        std::string pfile;

        bool run_interactive = false;
        bool run_checker = false;
        bool print_reg_info = false;

        std::vector<std::string> checks;
    };

}

#endif  /* _UCLE_CORE_FNSIM_CLI_HPP_ */
