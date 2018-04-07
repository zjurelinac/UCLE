#ifndef _UCLE_CORE_FNSIM_CHECKER_HPP_
#define _UCLE_CORE_FNSIM_CHECKER_HPP_

#include <fnsim/simulation.hpp>

#include <libs/fmt/format.h>

#include <string>
#include <vector>

namespace ucle::fnsim {

    template <typename FunctionalSimulation>
    void run_checks(std::vector<std::string>& checks, FunctionalSimulation& sim)
    {
        fmt::print_colored(fmt::YELLOW, "Running checker...\n");

        auto reg_vals = sim.get_reg_info();

        for (auto check : checks) {
            fmt::print("{}\n", check);

        }
    }

}

#endif  /* _UCLE_CORE_FNSIM_CHECKER_HPP_ */
