#ifndef _UCLE_CORE_FNSIM_CLI_HPP_
#define _UCLE_CORE_FNSIM_CLI_HPP_

#include <fnsim/base.hpp>

#include <fnsim/checker.hpp>
#include <fnsim/simulation.hpp>

#include <fnsim/processors/frisc.hpp>

#include <functional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace ucle::fnsim::cli {

    template <unsigned N>
    using fnsim_factory = std::function<functional_processor_simulator_ptr<N>(fnsim::processor_config)>;

    std::set<std::string> factory_options = {"frisc"};
    std::unordered_map<std::string, fnsim_factory<32>> factory_32 = {
        {"frisc", &frisc::make_frisc_simulator}
    };

    struct config {
        // Processor options
        std::string simulator_name;
        uint32_t fnsim_mem_size = 4096;

        // Source options
        std::string pfile;

        // Runtime options
        bool run_json = false;
        bool run_interactive = false;
        bool run_checker = false;
        bool print_reg_info = false;
        bool print_exec_info = false;

        // Checker options
        std::vector<std::string> checks;
        int verbosity;
    };

}

#endif  /* _UCLE_CORE_FNSIM_CLI_HPP_ */
