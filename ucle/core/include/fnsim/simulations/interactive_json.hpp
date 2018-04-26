#ifndef _UCLE_CORE_FNSIM_SIMULATIONS_INTERACTIVE_JSON_HPP_
#define _UCLE_CORE_FNSIM_SIMULATIONS_INTERACTIVE_JSON_HPP_

#include <fnsim/base.hpp>

#include <fnsim/simulations/interactive.hpp>

#include <libs/fmt/format.h>
#include <libs/nlohmann/json.hpp>

#include <iostream>

namespace ucle::fnsim {

    template <typename FunctionalSimulation>
    class interactive_json_simulation : public interactive_simulation<FunctionalSimulation> {
        using interactive_simulation<FunctionalSimulation>::interactive_simulation;

        protected:
            interactive_cmd_info get_cmd_() override
            {
                using json = nlohmann::json;

                json j;
                std::cin >> j;
                std::cout << std::setw(4) << j;
                return { "h", {} };
                // while (true) {
                //     std::string cmd;
                //     fmt::print("> "); getline(std::cin, cmd);

                //     auto cmd_tokens = util::split(util::trim_copy(cmd));
                //     if (!cmd_tokens.size()) continue;

                //     interactive_cmd_info cinfo = { cmd_tokens[0], {} };

                //     if (cmd_tokens.size() > 1) {
                //         cinfo.args.resize(cmd_tokens.size() - 1);
                //         std::copy(cmd_tokens.begin() + 1, cmd_tokens.end(), cinfo.args.begin());
                //     }

                //     return cinfo;
                // }
            }

            void do_help_(argument_list) override;
            void do_run_(argument_list) override;
            void do_start_(argument_list) override;
            void do_cont_(argument_list) override;
            void do_step_(argument_list) override;
            void do_step_n_(argument_list) override;
            void do_until_(argument_list) override;
            void do_reset_(argument_list) override;
            void do_quit_(argument_list) override;
            void do_break_(argument_list) override;
            void do_watch_(argument_list) override;
            void do_info_(argument_list) override;

            void info_ (std::string msg) override       {}
            void warn_ (std::string msg) override       {}
            void error_ (std::string msg) override      {}
            void success_ (std::string msg) override    {}
    };

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_help_(argument_list)
    {

    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_run_(argument_list)
    {

    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_start_(argument_list)
    {

    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_cont_(argument_list)
    {

    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_step_(argument_list)
    {

    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_step_n_(argument_list)
    {

    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_until_(argument_list)
    {

    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_reset_(argument_list)
    {

    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_quit_(argument_list)
    {

    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_break_(argument_list)
    {

    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_watch_(argument_list)
    {

    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_info_(argument_list)
    {

    }

    template <typename FunctionalSimulation>
    void run_interactive_json_simulation(FunctionalSimulation& sim, std::string pfile)
    {
        interactive_json_simulation<FunctionalSimulation> tsim {sim, pfile};
        tsim.run();
    }
}

#endif  /* _UCLE_CORE_FNSIM_SIMULATIONS_INTERACTIVE_JSON_HPP_ */
