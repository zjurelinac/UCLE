#ifndef _UCLE_CORE_FNSIM_CHECKER_HPP_
#define _UCLE_CORE_FNSIM_CHECKER_HPP_

#include <fnsim/simulation.hpp>

#include <string>
#include <vector>

namespace ucle::fnsim {

    void run_checks(std::vector<std::string>& checks, functional_simulation& sim);

}

#endif  /* _UCLE_CORE_FNSIM_CHECKER_HPP_ */