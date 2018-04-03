#include <fnsim/checker.hpp>

#include <libs/fmt/format.h>

namespace fnsim = ucle::fnsim;

void fnsim::run_checks(std::vector<std::string>& checks, fnsim::functional_simulation& sim)
{
    fmt::print_colored(fmt::YELLOW, "Running checker...\n");

    for (auto check : checks) {
        fmt::print("{}\n", check);

    }
}
