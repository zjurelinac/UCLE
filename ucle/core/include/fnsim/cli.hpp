#ifndef _UCLE_CORE_FNSIM_CLI_HPP_
#define _UCLE_CORE_FNSIM_CLI_HPP_

#include <fnsim/base.hpp>

#include <fnsim/checker.hpp>
#include <fnsim/exceptions.hpp>
#include <fnsim/simulation.hpp>

#include <fnsim/processors/frisc.hpp>

#include <libs/nlohmann/json.hpp>

#include <util/json.hpp>
#include <util/string.hpp>

#include <functional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace ucle::fnsim::cli {

    template <unsigned N>
    using proc_factory_type = std::function<functional_processor_simulator_ptr<N>(fnsim::processor_config)>;

    template <unsigned N> std::unordered_map<std::string, proc_factory_type<N>> proc_factory;
    template <> std::unordered_map<std::string, proc_factory_type<32>> proc_factory<32> = {
        { "frisc", &frisc::make_frisc_simulator }
    };
    template <> std::unordered_map<std::string, proc_factory_type<64>> proc_factory<64> = {};


    inline auto parse_config(nlohmann::json config)
    {
        auto sys_config = util::get(config, "system_options");
        processor_config cfg { util::get(sys_config, "memory_size") };
        return cfg;
    }

    template <typename FunctionalSimulation>
    inline FunctionalSimulation build_fnsim(nlohmann::json config)
    {
        std::string arch = util::lowercase(util::get(config, "architecture"));

        if (!proc_factory<FunctionalSimulation::arch_width>.count(arch))
            throw incorrect_call(fmt::format("Unknown architecture '{}' for given architecture width", arch));

        return FunctionalSimulation { proc_factory<FunctionalSimulation::arch_width>[arch](parse_config(config)) };
    }

    template <unsigned N>
    inline void run_basic_helper(std::string program_path, nlohmann::json config)
    {
        auto sim_options = util::get(config, "simulation_options");
        auto fnsim = build_fnsim<functional_simulation<N, false, false, false, true>>(config);
        fnsim.load_pfile(program_path);
        fnsim.run();

        if (util::get(sim_options, "print_registers", false))
            print_reg_info(fnsim.get_reg_info());
    }

    void run_basic(std::string program_path, nlohmann::json config)
    {
        int width = util::get(config, "width", 32);
        switch (width) {
            case 32:
                return run_basic_helper<32>(program_path, config);
            case 64:
                return run_basic_helper<64>(program_path, config);
            default:
                throw incorrect_call(fmt::format("Architecture width of {} is not supported.", width));
        }
    }

    // void run_interactive_text(std::string program_path, nlohmann::json config) {}
    // void run_interactive_json(std::string program_path, nlohmann::json config) {}
    // void run_statistics(std::string program_path, nlohmann::json config) {}
    // void run_testing(std::string program_path, nlohmann::json config) {}

    using runner_type = std::function<void(std::string, nlohmann::json)>;

    std::unordered_map<std::string, runner_type> runners = {
        { "basic", &run_basic }
        // { "interactive", &run_interactive_text },
        // { "interactive_json", &run_interactive_json },
        // { "statistics", &run_statistics },
        // { "testing", &run_testing }
    };
}

#endif  /* _UCLE_CORE_FNSIM_CLI_HPP_ */
