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

    enum class exec_mode { basic, stats, testing, interactive, interactive_json };

    template <unsigned N, exec_mode mode>
    inline void run_simple_helper(std::string program_path, nlohmann::json config)
    {
        auto fnsim = build_fnsim<functional_simulation<N, false, false, false, (mode == exec_mode::stats)>>(config);
        fnsim.load_pfile(program_path);
        fnsim.run();

        auto sim_options = util::get(config, "simulation_options");

        if constexpr (mode == exec_mode::basic) {
            if (util::get(sim_options, "print_registers", false))
                print_reg_info(fnsim.get_reg_info());
        } else if constexpr (mode == exec_mode::stats) {
            print_exec_info(fnsim.get_exec_info());
        } else if constexpr (mode == exec_mode::testing) {
            checker chk { fnsim, program_path, util::get(sim_options, "verbosity", 0) };
            auto checks = util::get(sim_options, "checks", std::vector<std::string> {} );
            chk.run(checks);
        } else {
            throw design_error("This function should not be called with this particular exec_mode.");
        }
    }

    template <exec_mode mode>
    inline void run_simple(std::string program_path, nlohmann::json config)
    {
        int width = util::get(config, "width", 32);
        switch (width) {
            case 32:
                return run_simple_helper<32, mode>(program_path, config);
            case 64:
                return run_simple_helper<64, mode>(program_path, config);
            default:
                throw incorrect_call(fmt::format("Architecture width of {} is not supported.", width));
        }
    }

    // void run_interactive_text(std::string program_path, nlohmann::json config) {}
    // void run_interactive_json(std::string program_path, nlohmann::json config) {}

    using runner_type = std::function<void(std::string, nlohmann::json)>;

    std::unordered_map<std::string, runner_type> runners = {
        { "basic", &run_simple<exec_mode::basic> },
        // { "interactive", &run_interactive_text },
        // { "interactive_json", &run_interactive_json },
        { "stats", &run_simple<exec_mode::stats> },
        { "testing", &run_simple<exec_mode::testing> }
    };
}

#endif  /* _UCLE_CORE_FNSIM_CLI_HPP_ */
