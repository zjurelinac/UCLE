#include <fnsim/cli.hpp>

#include <libs/cli/cli11.hpp>
#include <libs/fmt/format.h>

#include <cstdlib>
#include <memory>

/*
    fmt colors { BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE };

    Color codes:
        - YELLOW - important events
        - GREEN - success
        - RED - errors
        - BLUE - info
        - CYAN - regular highlight
*/

int main(int argc, char* argv[]) {
    using namespace ucle::fnsim;

    CLI::App app {"Functional simulator CLI"};

    cli_config cli_cfg {};

    app.add_set_ignore_case("simulator", cli_cfg.simulator_name, factory_options, "Processor simulator to use for simulation")->required();
    app.add_option("pfile", cli_cfg.pfile, "A path to the machine-code .p file to be executed")->required()->check(CLI::ExistingFile);

    app.add_option("-m,--memory_size", cli_cfg.fnsim_mem_size, "Internal memory size for the processor in the simulation", true);

    app.add_flag("-i,--run-interactive", cli_cfg.run_interactive, "Run the simulation interactively");
    app.add_flag("-p,--print_reg_info", cli_cfg.print_reg_info, "Print register info after simulation run");

    auto checker_cmd = app.add_subcommand("check", "Simulation results checker");
    checker_cmd->add_option("checks", cli_cfg.checks, "A list of checks to perform once simulation finishes")->required();
    checker_cmd->set_callback([&cli_cfg]() { cli_cfg.run_checker = true; });

    CLI11_PARSE(app, argc, argv);

    processor_config sim_cfg {cli_cfg.fnsim_mem_size};
    functional_simulation sim(factory[cli_cfg.simulator_name](sim_cfg));

    if (cli_cfg.run_interactive) {
        run_interactive_simulation(sim, cli_cfg.pfile);
    } else {
        sim.load_pfile(cli_cfg.pfile);
        sim.run();

        if (cli_cfg.print_reg_info)
            print_reg_info(sim.get_reg_info());
        if (cli_cfg.run_checker)
            run_checks(cli_cfg.checks, sim);
    }
}