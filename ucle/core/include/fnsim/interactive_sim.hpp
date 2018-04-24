#ifndef _UCLE_CORE_FNSIM_INTERACTIVE_SIM_HPP_
#define _UCLE_CORE_FNSIM_INTERACTIVE_SIM_HPP_

#include <fnsim/base.hpp>
#include <fnsim/simulation.hpp>

#include <libs/fmt/format.h>

#include <util/string.hpp>

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace ucle::fnsim {

    using args_list = std::vector<std::string>;

    template <typename FunctionalSimulation>
    class interactive_simulation;

    template <typename FunctionalSimulation>
    using interactive_cmd = std::function<void(interactive_simulation<FunctionalSimulation>&, args_list)>;

    template <typename ICmd>
    struct interactive_cmd_descr {
        char shortcut;
        std::string name;
        ICmd cmd;
        std::string use;
        std::string descr;
    };

    template <typename ICmdDescr>
    inline void print_icmd_descr(const ICmdDescr& icmd)
    {
        if (icmd.use.size() > 16) {
            fmt::print_colored(fmt::CYAN, "  {}\n", icmd.use);
            fmt::print("  {:<16}{}\n", "", icmd.descr);
        } else {
            fmt::print_colored(fmt::CYAN, "  {:<16}", icmd.use);
            fmt::print("{}\n", icmd.descr);
        }
    }

    template <typename FunctionalSimulation>
    class interactive_simulation {
        using icmd_descr_type = interactive_cmd_descr<interactive_cmd<FunctionalSimulation>>;

        public:
            interactive_simulation() = delete;
            interactive_simulation(FunctionalSimulation& sim, std::string pfile)
                : sim_{sim}, pfile_{pfile} { init_cmd_descrs_(); }

            void run();

        private:
            void cmd_help(args_list);

            void cmd_run(args_list);
            void cmd_start(args_list);
            void cmd_cont(args_list);
            void cmd_step(args_list);
            void cmd_step_n(args_list);
            void cmd_until(args_list);
            void cmd_reset(args_list);
            void cmd_quit(args_list);

            void cmd_break(args_list);
            void cmd_watch(args_list);
            void cmd_info(args_list);

            void init_cmd_descrs_();
            void show_simulation_state_();

            void warn_unknown_cmd_(std::string cmd_name);
            void warn_incorrect_use_(std::string cmd_name);

            FunctionalSimulation& sim_;
            std::string pfile_;

            bool running_ {true};

            std::vector<icmd_descr_type> icmds_;
    };

    template <typename FunctionalSimulation>
    inline void interactive_simulation<FunctionalSimulation>::cmd_help(args_list)
    {
        fmt::print("* Help - interactive processor simulation\n");
        fmt::print("======================================================================\n");

        for (const auto& icmd : icmds_)
            print_icmd_descr(icmd);

        fmt::print("\n");
    }

    template <typename FunctionalSimulation>
    inline void interactive_simulation<FunctionalSimulation>::cmd_run(args_list args)
    {
        address_t start_location = 0;

        if (args.size() > 0) {
            if (!util::parse_int<address_t>(args[0], &start_location))
                throw malformed_argument(fmt::format("Cannot parse {} as an address (must be an integer >= 0)", args[0]));
        }

        fmt::print("* Starting program @ 0x{:08X}\n", start_location);
        if (auto stat = sim_.start(start_location); is_error(stat))
            throw runtime_error(to_string(stat));

        fmt::print_colored(fmt::GREEN, "* Program started.\n");

        if (auto stat = sim_.cont(); is_error(stat))
            throw runtime_error(to_string(stat));
        else
            show_simulation_state_();
    }

    template <typename FunctionalSimulation>
    inline void interactive_simulation<FunctionalSimulation>::cmd_start(args_list args)
    {
        address_t start_location = 0;

        if (args.size() > 0) {
            if (!util::parse_int<address_t>(args[0], &start_location))
                throw malformed_argument(fmt::format("Cannot parse {} as an address (must be an integer >= 0)", args[0]));
        }

        fmt::print("* Starting program @ 0x{:08X}\n", start_location);
        if (auto stat = sim_.start(start_location); is_error(stat))
            throw runtime_error(to_string(stat));

        fmt::print_colored(fmt::GREEN, "* Program started.\n");
    }

    template <typename FunctionalSimulation>
    inline void interactive_simulation<FunctionalSimulation>::cmd_cont(args_list)
    {
        fmt::print("* Continuing program...\n");

        if (auto stat = sim_.cont(); is_error(stat))
            throw runtime_error(to_string(stat));
        else
            show_simulation_state_();
    }

    template <typename FunctionalSimulation>
    inline void interactive_simulation<FunctionalSimulation>::cmd_step(args_list)
    {
        if (auto stat = sim_.step(); is_error(stat))
            throw runtime_error(to_string(stat));
        else
            show_simulation_state_();
    }

    template <typename FunctionalSimulation>
    inline void interactive_simulation<FunctionalSimulation>::cmd_step_n(args_list args)
    {
        if (args.size() == 0)
            throw wrong_call("step_n");

        int num_steps;
        if (!util::parse_int<int>(args[0], &num_steps) || num_steps <= 0)
            throw malformed_argument(fmt::format("Cannot parse {} as number of steps (must be an integer > 0)", args[0]));

        fmt::print("* Executing {} steps of the simulation...\n", num_steps);

        if (auto stat = sim_.step_n(num_steps); is_error(stat))
            throw runtime_error(to_string(stat));
        else
            show_simulation_state_();
    }

    template <typename FunctionalSimulation>
    inline void interactive_simulation<FunctionalSimulation>::cmd_until(args_list args)
    {
        if (args.size() == 0)
            throw wrong_call("until");

        address_t end_location;
        if (!util::parse_int<address_t>(args[0], &end_location))
            throw malformed_argument(fmt::format("Cannot parse {} as an address (must be an integer >= 0)", args[0]));

        fmt::print("* Executing simulation until @ 0x{:08X}\n", end_location);

        if (auto stat = sim_.until(end_location); is_error(stat))
            throw runtime_error(to_string(stat));
        else
            show_simulation_state_();
    }

    template <typename FunctionalSimulation>
    inline void interactive_simulation<FunctionalSimulation>::cmd_reset(fnsim::args_list)
    {
        sim_.reset();
        fmt::print("* Reset devices, processor memory and registers\n");

        if (auto stat = sim_.load_pfile(pfile_); is_error(stat))
            throw fatal_error(fmt::format("Cannot reload program from {}", pfile_));

        fmt::print_colored(fmt::GREEN, "* Reloaded program from {}\n", pfile_);
    }

    template <typename FunctionalSimulation>
    inline void interactive_simulation<FunctionalSimulation>::cmd_quit(args_list)
    {
        fmt::print("* Received quit.\n");
        running_ = false;
        fmt::print_colored(fmt::YELLOW, "* Ending interactive simulation.\n");
    }

    template <typename FunctionalSimulation>
    inline void interactive_simulation<FunctionalSimulation>::cmd_break(args_list args)
    {
        if (args.size() == 0)
            throw wrong_call("break");

        if (args[0] == "list") {
            auto breakpoints = sim_.get_breakpoints();
            fmt::print_colored(fmt::BLUE, "Active breakpoints ({}):\n", breakpoints.size());

            auto i = 0u;
            for (const auto bp : breakpoints)
                fmt::print("Breakpoint {} @ 0x{:08X}\n", ++i, bp);

        } else if (args[0] == "add") {
            if (args.size() < 2)
                throw wrong_call("break");

            address_t location;
            if (!util::parse_int<address_t>(args[0], &location))
                throw malformed_argument(fmt::format("Cannot parse {} as an address (must be an integer >= 0)", args[0]));

            sim_.add_breakpoint(location);
            fmt::print_colored(fmt::GREEN, "* Added a breakpoint @ 0x{:08X}\n", location);

        } else if (args[0] == "del") {
            if (args.size() < 2)
                throw wrong_call("break");

            address_t location;
            if (!util::parse_int<address_t>(args[0], &location))
                throw malformed_argument(fmt::format("Cannot parse {} as an address (must be an integer >= 0)", args[0]));

            sim_.remove_breakpoint(location);
            fmt::print_colored(fmt::GREEN, "* Removed a breakpoint @ 0x{:08X}\n", location);

        } else if (args[0] == "clear") {
            sim_.clear_breakpoints();
            fmt::print_colored(fmt::GREEN, "* Cleared all breakpoints\n");
        } else {
            warn_incorrect_use_("break");
        }
    }

    template <typename FunctionalSimulation>
    inline void interactive_simulation<FunctionalSimulation>::cmd_watch(args_list)
    {
        // fmt::print_colored(fmt::BLUE, "* Watch!\n");
    }

    template <typename FunctionalSimulation>
    inline void interactive_simulation<FunctionalSimulation>::cmd_info(args_list)
    {
        fmt::print_colored(fmt::BLUE, "* Processor state:\n");
        print_reg_info(sim_.get_reg_info());
    }

    template <typename FunctionalSimulation>
    inline void interactive_simulation<FunctionalSimulation>::warn_unknown_cmd_(std::string cmd_name)
    {
        fmt::print_colored(fmt::RED, "! Unknown command: {}\n", cmd_name);
        fmt::print("? Perhaps try one of the following:\n");
        cmd_help({});
    }

    template <typename FunctionalSimulation>
    inline void interactive_simulation<FunctionalSimulation>::warn_incorrect_use_(std::string cmd_name)
    {
        auto icmd_it = std::find_if(icmds_.begin(), icmds_.end(), [&cmd_name](auto icmd) { return icmd.name == cmd_name; });

        if (icmd_it == icmds_.end()) {
            warn_unknown_cmd_(cmd_name);
            return;
        }

        fmt::print_colored(fmt::RED, "! Incorrect command call of: {}\n", cmd_name);
        fmt::print("? Try matching the following format:\n");
        print_icmd_descr(*icmd_it);
    }

    template <typename FunctionalSimulation>
    inline void interactive_simulation<FunctionalSimulation>::show_simulation_state_()
    {
        auto [state, location, annotation] = sim_.get_state_info();
        fmt::print_colored(fmt::BLUE, "* Simulator {} @ 0x{:08X} ({})\n", to_string(state), location, annotation);
        // TODO: Test if stopped at a breakpoint
    }

    template <typename FunctionalSimulation>
    inline void interactive_simulation<FunctionalSimulation>::init_cmd_descrs_()
    {
        icmds_ = {
            {'h', "help", &interactive_simulation::cmd_help, "[h]elp", "Show this help message"},

            {'r', "run", &interactive_simulation::cmd_run, "[r]un [ADDR]", "Run simulation from the start (or from a given ADDR)"},
            {'a', "start", &interactive_simulation::cmd_start, "st[a]rt [ADDR]", "Init simulation to the start (or to a given ADDR)"},
            {'c', "cont", &interactive_simulation::cmd_cont, "[c]ont", "Continue paused simulation"},
            {'s', "step", &interactive_simulation::cmd_step, "[s]tep", "Execute one simulation step (ie. instruction)"},
            {'n', "step_n", &interactive_simulation::cmd_step_n, "step_[n] [N]", "Execute N (or less) simulation steps"},
            {'u', "until", &interactive_simulation::cmd_until, "[u]ntil ADDR", "Run simulation until reaching ADDR",},
            {'t', "reset", &interactive_simulation::cmd_reset, "rese[t]", "Reset simulator - clear registers and memory contents"},
            {'q', "quit", &interactive_simulation::cmd_quit, "[q]uit", "Quit interactive simulation"},

            {'b', "break", &interactive_simulation::cmd_break, "[b]reak (list | add ADDR | del ADDR | clear)", "List breakpt | Add/remove breakpt at ADDR | Clear all breakpts"},
            {'w', "watch", &interactive_simulation::cmd_watch, "[w]atch (list | add ADDR | del ADDR | clear)", "List watches | Add/remove watch at ADDR | Clear all watches"},
            {'i', "info", &interactive_simulation::cmd_info, "[i]nfo", "Display processor state (= value of it's registers)"}
        };
    }

    static inline std::string get_user_cmd()
    {
        fmt::print("> ");

        std::string cmd;
        getline(std::cin, cmd);

        return cmd;
    }

    template <typename FunctionalSimulation>
    void interactive_simulation<FunctionalSimulation>::run()
    {
        fmt::print_colored(fmt::YELLOW, "* Starting interactive simulation...\n");

        if (auto stat = sim_.load_pfile(pfile_); is_error(stat)) {
            fmt::print_colored(fmt::RED, "! Error loading program from {}", pfile_);
            std::exit(1);
        }

        fmt::print_colored(fmt::GREEN, "* Loaded program from {}\n", pfile_);

        while (running_) {
            auto cmd = util::trim_copy(get_user_cmd());
            auto cmd_tokens = util::split(cmd);

            if (!cmd_tokens.size()) continue;

            auto cmd_name = cmd_tokens[0];

            args_list args {};
            args.resize(cmd_tokens.size() - 1);

            if (cmd_tokens.size() > 1)
                std::copy(cmd_tokens.begin() + 1, cmd_tokens.end(), args.begin());

            try {
                bool found_cmd = false;
                for (const auto& icmd : icmds_)
                    if ((cmd_name.size() == 1 && icmd.shortcut == cmd_name[0]) ||
                        (cmd_name.size() > 1 && icmd.name == cmd_name)) {
                            icmd.cmd(*this, args);
                            found_cmd = true;
                            break;
                        }

                if (!found_cmd)
                    warn_unknown_cmd_(cmd_name);
            } catch (wrong_call& e) {
                warn_incorrect_use_(e.what());
            } catch (fatal_error& e) {
                fmt::print_colored(fmt::RED, "! Fatal error: {}.\n", e.what());
                std::exit(1);
            } catch (base_exception& e) {
                fmt::print_colored(fmt::RED, "! Error: {}.\n", e.what());
            }
        }
    }

    template <typename FunctionalSimulation>
    void run_interactive_simulation(FunctionalSimulation& sim, std::string pfile)
    {
        interactive_simulation isim {sim, pfile};
        isim.run();
    }

}

#endif  /* _UCLE_CORE_FNSIM_INTERACTIVE_SIM_HPP_ */
