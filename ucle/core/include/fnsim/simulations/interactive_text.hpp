#ifndef _UCLE_CORE_FNSIM_SIMULATIONS_INTERACTIVE_TEXT_HPP_
#define _UCLE_CORE_FNSIM_SIMULATIONS_INTERACTIVE_TEXT_HPP_

#include <fnsim/base.hpp>

#include <fnsim/simulations/interactive.hpp>

#include <libs/fmt/format.h>

#include <algorithm>
#include <iostream>

namespace ucle::fnsim::cli {

    template <typename FunctionalSimulation>
    class interactive_text_simulation : public interactive_simulation<FunctionalSimulation> {
        using interactive_simulation<FunctionalSimulation>::interactive_simulation;

        struct cmd_help {
            std::string name;
            std::string use;
            std::string description;
        };

        protected:
            interactive_cmd_info get_cmd_() override
            {
                while (true) {
                    std::string cmd;
                    fmt::print("> "); getline(std::cin, cmd);

                    auto cmd_tokens = util::split(util::trim_copy(cmd));
                    if (!cmd_tokens.size()) continue;

                    interactive_cmd_info cinfo = { cmd_tokens[0], {} };

                    if (cmd_tokens.size() > 1) {
                        cinfo.args.resize(cmd_tokens.size() - 1);
                        std::copy(cmd_tokens.begin() + 1, cmd_tokens.end(), cinfo.args.begin());
                    }

                    return cinfo;
                }
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

            void info_ (std::string msg) override       { fmt::print_colored(fmt::BLUE, "* {}\n", msg); }
            void warn_ (std::string msg) override       { fmt::print_colored(fmt::YELLOW, "* {}\n", msg); }
            void error_ (std::string msg) override      { fmt::print_colored(fmt::RED, "! {}\n", msg); }
            void success_ (std::string msg) override    { fmt::print_colored(fmt::GREEN, "* {}\n", msg); }
            void notify_ (std::string msg) override     { fmt::print("* {}\n", msg); }

            void emph_(std::string msg)                 { fmt::print_colored(fmt::CYAN, msg); };

        private:
            std::string location_action_(std::string msg, address32_t location)                      { return fmt::format("{} @ 0x{:08X}", msg, location); }
            std::string location_action_(std::string msg, address32_t location, std::string extra)   { return fmt::format("{} @ 0x{:08X} ({})", msg, location, extra); }

            void show_simulation_state_()
            {
                auto [state, location, annotation] = this->get_sim_().get_state_info();
                info_(location_action_("Simulator " + to_string(state), location, annotation));
            }

            void show_cmd_info_(cmd_help help)
            {
                if (help.use.size() > 16) {
                    emph_("  " + help.use);
                    fmt::print("\n  {:<16}{}\n", "", help.description);
                } else {
                    emph_(fmt::format("  {:<16}", help.use));
                    fmt::print("{}\n", help.description);
                }
            }

            std::vector<cmd_help> help_ = {
                { "help", "[h]elp", "Show this help message" },
                { "run", "[r]un [ADDR]", "Run simulation from the start (or from a given ADDR)" },
                { "start", "st[a]rt [ADDR]", "Init simulation to the start (or to a given ADDR)" },
                { "cont", "[c]ont", "Continue paused simulation" },
                { "step", "[s]tep", "Execute one simulation step (ie. instruction)" },
                { "step_n", "step_[n] [N]", "Execute N (or less) simulation steps" },
                { "until", "[u]ntil ADDR", "Run simulation until reaching ADDR", },
                { "reset", "rese[t]", "Reset simulator - clear registers and memory contents" },
                { "quit", "[q]uit", "Quit interactive simulation" },
                { "break", "[b]reak (list | add ADDR | del ADDR | clear)", "List breakpt | Add/remove breakpt at ADDR | Clear all breakpts" },
                { "watch", "[w]atch (list | add ADDR | del ADDR | clear)", "List watches | Add/remove watch at ADDR | Clear all watches" },
                { "info", "[i]nfo", "Display processor state (= value of it's registers) "}
            };
    };

    template <typename FunctionalSimulation>
    inline void interactive_text_simulation<FunctionalSimulation>::do_help_(argument_list)
    {
        notify_("Help - interactive processor simulation");
        fmt::print("======================================================================\n");

        for (const auto& h : help_)
            show_cmd_info_(h);

        fmt::print("\n");
    }

    template <typename FunctionalSimulation>
    inline void interactive_text_simulation<FunctionalSimulation>::do_run_(argument_list args)
    {
        do_start_(args);

        if (auto stat = this->get_sim_().cont(); is_error(stat))
            throw runtime_error(to_string(stat));

        show_simulation_state_();
    }

    template <typename FunctionalSimulation>
    inline void interactive_text_simulation<FunctionalSimulation>::do_start_(argument_list args)
    {
        address32_t start_location = 0;

        if (args.size() > 0) {
            if (!util::parse_int<address32_t>(std::any_cast<std::string>(args[0]), &start_location))
                throw malformed_argument(fmt::format("Cannot parse argument as an address (must be an integer >= 0)"));
        }

        notify_(location_action_("Starting program", start_location));

        if (auto stat = this->get_sim_().start(start_location); is_error(stat))
            throw runtime_error(to_string(stat));

        success_("Program started");
    }

    template <typename FunctionalSimulation>
    inline void interactive_text_simulation<FunctionalSimulation>::do_cont_(argument_list)
    {
        notify_("Continuing program...");

        if (auto stat = this->get_sim_().cont(); is_error(stat))
            throw runtime_error(to_string(stat));

        show_simulation_state_();
    }

    template <typename FunctionalSimulation>
    inline void interactive_text_simulation<FunctionalSimulation>::do_step_(argument_list)
    {
        if (auto stat = this->get_sim_().step(); is_error(stat))
            throw runtime_error(to_string(stat));

        show_simulation_state_();
    }

    template <typename FunctionalSimulation>
    inline void interactive_text_simulation<FunctionalSimulation>::do_step_n_(argument_list args)
    {
        if (args.size() == 0)
            throw incorrect_call("step_n");

        int num_steps;
        if (!util::parse_int<int>(std::any_cast<std::string>(args[0]), &num_steps) || num_steps <= 0)
            throw malformed_argument(fmt::format("Cannot parse argument as number of steps (must be an integer > 0)"));

        notify_(fmt::format("Executing {} steps of the simulation...", num_steps));

        if (auto stat = this->get_sim_().step_n(num_steps); is_error(stat))
            throw runtime_error(to_string(stat));

        show_simulation_state_();
    }

    template <typename FunctionalSimulation>
    inline void interactive_text_simulation<FunctionalSimulation>::do_until_(argument_list args)
    {
        if (args.size() == 0)
            throw incorrect_call("until");

        address32_t end_location;
        if (!util::parse_int<address32_t>(std::any_cast<std::string>(args[0]), &end_location))
            throw malformed_argument(fmt::format("Cannot parse argument as an address (must be an integer >= 0)"));

        notify_(location_action_("Executing simulation until", end_location));

        if (auto stat = this->get_sim_().until(end_location); is_error(stat))
            throw runtime_error(to_string(stat));

        show_simulation_state_();
    }

    template <typename FunctionalSimulation>
    inline void interactive_text_simulation<FunctionalSimulation>::do_reset_(argument_list)
    {
        notify_("Reset devices, processor memory and registers");
        this->reload_();
    }

    template <typename FunctionalSimulation>
    inline void interactive_text_simulation<FunctionalSimulation>::do_quit_(argument_list)
    {
        notify_("Received quit.");
        this->quit_();
    }

    template <typename FunctionalSimulation>
    inline void interactive_text_simulation<FunctionalSimulation>::do_break_(argument_list args)
    {
        if (args.size() == 0)
            throw incorrect_call("break");

        auto subcmd = std::any_cast<std::string>(args[0]);

        if (subcmd == "list") {
            auto breakpoints = this->get_sim_().get_breakpoints();
            info_(fmt::format("Active breakpoints ({}):", breakpoints.size()));

            auto i = 0u;
            for (const auto bp : breakpoints)
                notify_(location_action_(fmt::format("Breakpoint {}", ++i), bp));

        } else if (subcmd == "add") {
            if (args.size() < 2)
                throw incorrect_call("break");

            address32_t location;
            if (!util::parse_int<address32_t>(std::any_cast<std::string>(args[1]), &location))
                throw malformed_argument(fmt::format("Cannot parse argument as an address (must be an integer >= 0)"));

            this->get_sim_().add_breakpoint(location);
            success_(location_action_("Added breakpoint", location));

        } else if (subcmd == "del") {
            if (args.size() < 2)
                throw incorrect_call("break");

            address32_t location;
            if (!util::parse_int<address32_t>(std::any_cast<std::string>(args[1]), &location))
                throw malformed_argument(fmt::format("Cannot parse argument as an address (must be an integer >= 0)"));

            this->get_sim_().remove_breakpoint(location);
            success_(location_action_("Removed breakpoint", location));

        } else if (subcmd == "clear") {
            this->get_sim_().clear_breakpoints();
            success_("Cleared all breakpoints");
        } else {
            this->report_incorrect_use_("break");
        }
    }

    template <typename FunctionalSimulation>
    inline void interactive_text_simulation<FunctionalSimulation>::do_watch_(argument_list)
    {
        error_("Watch command unsupported as of yet.");
    }

    template <typename FunctionalSimulation>
    inline void interactive_text_simulation<FunctionalSimulation>::do_info_(argument_list)
    {
        info_("Processor state:");
        print_reg_info(this->get_sim_().get_reg_info());
    }

    template <typename FunctionalSimulation>
    void run_interactive_text_simulation(FunctionalSimulation& sim, config& cfg)
    {
        cfg.verbosity = 1;
        interactive_text_simulation<FunctionalSimulation> tsim {sim, cfg};
        tsim.run();
    }

}

#endif  /* _UCLE_CORE_FNSIM_SIMULATIONS_INTERACTIVE_TEXT_HPP_ */
