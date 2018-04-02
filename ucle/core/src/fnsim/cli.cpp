#include <fnsim/base.hpp>
#include <fnsim/checker.hpp>
#include <fnsim/simulation.hpp>

#include <fnsim/frisc.hpp>

#include <libs/cli/cli11.hpp>
#include <libs/fmt/format.h>

#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>

namespace fnsim = ucle::fnsim;

using fnsim_factory = std::function<fnsim::functional_simulator_ptr(fnsim::simulator_config)>;

std::set<std::string> factory_options = {"frisc"};
std::unordered_map<std::string, fnsim_factory> factory = {
    {"frisc", &fnsim::make_frisc_simulator}
};

// fmt colors { BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE };
int main(int argc, char* argv[]) {
    CLI::App app {"Functional simulator CLI"};

    std::string simulator_name;
    app.add_set_ignore_case("simulator", simulator_name, factory_options, "Processor simulator to use for simulation")->required();

    std::string pfile;
    app.add_option("pfile", pfile, "A path to the machine-code .p file to be executed")->required()->check(CLI::ExistingFile);

    uint32_t fnsim_mem_size = 4096;
    app.add_option("-m,--memory_size", fnsim_mem_size, "Internal memory size for the processor in the simulation", true);

    bool print_reg_info_after_run = false;
    app.add_flag("-p,--print_reg_info_after_run", print_reg_info_after_run);

    bool run_checker = false;
    auto checker_cmd = app.add_subcommand("check", "Simulation results checker");

    std::vector<std::string> checks;
    checker_cmd->add_option("checks", checks, "A list of checks to perform once simulation finishes")->required();
    checker_cmd->set_callback([&run_checker]() { run_checker = true; });

    CLI11_PARSE(app, argc, argv);

    fnsim::simulator_config cfg {fnsim_mem_size};
    fnsim::functional_simulation sim(factory[simulator_name](cfg));

    sim.load_pfile(pfile);
    sim.run();

    if (print_reg_info_after_run)
        fnsim::print_reg_info(sim.get_reg_info());

    if (run_checker)
        fnsim::run_checks(checks, sim);

}