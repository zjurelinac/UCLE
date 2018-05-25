#include <fnsim/cli.hpp>

#include <fnsim/simulations/interactive_text.hpp>
#include <fnsim/simulations/interactive_json.hpp>

#include <libs/cli/cli11.hpp>
#include <libs/fmt/format.h>
#include <libs/nlohmann/json.hpp>

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
    using namespace ucle;
    using namespace ucle::fnsim;
    using namespace ucle::fnsim::cli;

    CLI::App app {"Functional simulator CLI"};

    fnsim::cli::config cfg {};

    app.add_set_ignore_case("simulator", cfg.simulator_name, factory_options, "Processor simulator to use for simulation")->required();
    app.add_option("pfile", cfg.pfile, "A path to the machine-code .p file to be executed")->required()->check(CLI::ExistingFile);

    app.add_option("-m,--memory_size", cfg.fnsim_mem_size, "Internal memory size for the processor in the simulation", true);

    app.add_flag("-i,--run-interactive", cfg.run_interactive, "Run the simulation interactively");
    app.add_flag("-j,--run-json", cfg.run_json, "Run a JSON-controlled simulation");
    app.add_flag("-r,--print-reg-info", cfg.print_reg_info, "Print register info after simulation run");
    app.add_flag("-x,--print-exec-info", cfg.print_exec_info, "Print execution info after simulation run");

    auto checker_cmd = app.add_subcommand("check", "Simulation results checker");
    checker_cmd->set_callback([&cfg]() { cfg.run_checker = true; });
    checker_cmd->add_option("check", cfg.checks, "A list of checks to perform once simulation finishes")->required();
    auto verbose_callback = [&cfg](std::size_t count) { cfg.verbosity = count; };
    checker_cmd->add_flag("-v,--verbose-output", verbose_callback, "Controls how verbose will the checker output be");

    CLI11_PARSE(app, argc, argv);

    processor_config proc_cfg {cfg.fnsim_mem_size};

    if (cfg.run_interactive) {
        functional_simulation<> sim {factory_32[cfg.simulator_name](proc_cfg)};
        run_interactive_text_simulation(sim, cfg);
    } else if (cfg.run_json) {
        functional_simulation<> sim {factory_32[cfg.simulator_name](proc_cfg)};
        run_interactive_json_simulation(sim, cfg);
    } else {
        functional_simulation<false, false, false, true> sim {factory_32[cfg.simulator_name](proc_cfg)};
        sim.load_pfile(cfg.pfile);
        sim.run();

        if (cfg.print_reg_info)
            print_reg_info(sim.get_reg_info());

        if (cfg.print_exec_info)
            print_exec_info(sim.get_exec_info());

        if (cfg.run_checker) {
            checker chk {sim, cfg.pfile, cfg.verbosity};
            chk.run(cfg.checks);
        }
    }
}
