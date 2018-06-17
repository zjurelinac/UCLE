#include <fnsim/cli.hpp>

// #include <fnsim/simulations/interactive_text.hpp>
// #include <fnsim/simulations/interactive_json.hpp>

#include <libs/cli/cli11.hpp>
#include <libs/fmt/format.h>
#include <libs/nlohmann/json.hpp>

#include <util/json.hpp>
#include <util/string.hpp>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>

/*
    Simulator config format:
    {
        "architecture": string,
        "width": int,
        "simulation_type": string,
        "simulation_options": {
            ["checks": [ string ]],
            ["print_registers": bool],
            ["verbosity": int],
            ...
        },
        "system_options": {
            "memory_size": int,
            "devices": [
                {
                    "device_type": "frisc_gpio_led",
                    "device_name": "gpio_led",
                    "memory_"
                }
            ]
        }
    }
*/

int main(int argc, char* argv[]) {
    using namespace ucle;
    using namespace ucle::fnsim;
    using namespace ucle::fnsim::cli;
    using namespace nlohmann;

    CLI::App app {"Functional simulator CLI"};

    std::string program_path, config_path;

    app.add_option(
        "program_file", program_path,
        "A path to the file containing machine code which should be executed in the simulation"
    )->required()->check(CLI::ExistingFile);

    app.add_option(
        "config_file", config_path,
        "A path to the file containing simulator configuration options"
    )->required()->check(CLI::ExistingFile);

    CLI11_PARSE(app, argc, argv);

    try {
        std::ifstream config_file { config_path };
        json config = json::parse(config_file);

        std::string simulation_type = util::get(config, "simulation_type");

        if (!runners.count(simulation_type))
            throw incorrect_call(fmt::format("Unknown simulation type '{}'", simulation_type));

        runners[simulation_type](program_path, config);

    } catch (std::exception& e) {
        fmt::print_colored(stderr, fmt::RED, "Error: {}\n", e.what());
    }

    // 1. DETERMINE SIMULATION TYPE
    // 2. CONSTRUCT FUNCTIONAL_SIMULATION
    // 3. ADD DEVICES
    // 4. RUN SIMULATION
    // 5. PROCESS & OUTPUT RESULTS

    // if (cfg.run_interactive) {
    //     functional_simulation<> sim {factory_32[cfg.simulator_name](proc_cfg)};
    //     run_interactive_text_simulation(sim, cfg);
    // } else if (cfg.run_json) {
    //     functional_simulation<> sim {factory_32[cfg.simulator_name](proc_cfg)};
    //     run_interactive_json_simulation(sim, cfg);
    // } else {
    //     functional_simulation<false, false, false, true> sim {factory_32[cfg.simulator_name](proc_cfg)};
    //     sim.load_pfile(cfg.pfile);
    //     sim.run();

    //     if (cfg.print_reg_info)
    //         print_reg_info(sim.get_reg_info());

    //     if (cfg.print_exec_info)
    //         print_exec_info(sim.get_exec_info());

    //     if (cfg.run_checker) {
    //         checker chk {sim, cfg.pfile, cfg.verbosity};
    //         chk.run(cfg.checks);
    //     }
    // }
}
