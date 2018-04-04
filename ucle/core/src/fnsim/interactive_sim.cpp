#include <fnsim/interactive_sim.hpp>

#include <fnsim/base.hpp>
#include <libs/fmt/format.h>
#include <util/string.hpp>

#include <algorithm>
#include <cstdlib>
#include <iostream>

namespace fnsim = ucle::fnsim;

void fnsim::interactive_simulation::cmd_help(fnsim::args_list)
{
    fmt::print("* Help - interactive processor simulation\n");
    fmt::print("======================================================================\n");

    for (const auto& icmd : icmds_)
        print_icmd_descr(icmd);

    fmt::print("\n");
}

void fnsim::interactive_simulation::cmd_run(fnsim::args_list args)
{
    address_t start_location = 0;

    if (args.size() > 0)
        start_location = std::stol(args[0]);

    fmt::print("* Starting program from {}...\n", start_location);
    if (auto stat = sim_.start(start_location); is_error(stat)) {
        fmt::print_colored(fmt::RED, "! Error occurred: {}\n", to_string(stat));
        return;
    }

    fmt::print_colored(fmt::GREEN, "* Program started.\n");

    if (auto stat = sim_.cont(); is_error(stat)) {
        fmt::print_colored(fmt::RED, "! Error occurred: {}\n", to_string(stat));
    } else {
        show_simulation_state_();
    }
}

void fnsim::interactive_simulation::cmd_start(fnsim::args_list args)
{
    address_t start_location = 0;

    if (args.size() > 0)
        start_location = std::stol(args[0]);

    fmt::print("* Starting program from {}...\n", start_location);
    if (auto stat = sim_.start(start_location); is_error(stat)) {
        fmt::print_colored(fmt::RED, "! Error occurred: {}\n", to_string(stat));
        return;
    }

    fmt::print_colored(fmt::GREEN, "* Program started.\n");
}

void fnsim::interactive_simulation::cmd_cont(fnsim::args_list)
{
    fmt::print("* Continuing program...\n");

    if (auto stat = sim_.cont(); is_error(stat)) {
        fmt::print_colored(fmt::RED, "! Error occurred: {}\n", to_string(stat));
    } else {
        show_simulation_state_();
    }
}

void fnsim::interactive_simulation::cmd_step(fnsim::args_list)
{
    fmt::print("* Stepping...\n");

    if (auto stat = sim_.step(); is_error(stat)) {
        fmt::print_colored(fmt::RED, "! Error occurred: {}\n", to_string(stat));
    } else {
        show_simulation_state_();
    }
}

void fnsim::interactive_simulation::cmd_step_n(fnsim::args_list args)
{

}

void fnsim::interactive_simulation::cmd_until(fnsim::args_list)
{
    fmt::print("* Until!\n");
}

void fnsim::interactive_simulation::cmd_reset(fnsim::args_list)
{
    fmt::print("* Reseting simulator...\n");

    sim_.reset();

    fmt::print("* Reset devices, processor memory and registers\n");

    if (auto stat = sim_.load_pfile(pfile_); is_error(stat)) {
        fmt::print_colored(fmt::RED, "! Error reloading program from {}", pfile_);
        std::exit(1);
    }

    fmt::print_colored(fmt::GREEN, "* Reloaded program from {}\n", pfile_);
}

void fnsim::interactive_simulation::cmd_quit(fnsim::args_list)
{
    fmt::print("* Received quit.\n");
    running_ = false;
    fmt::print_colored(fmt::YELLOW, "* Ending interactive simulation.\n");
}

void fnsim::interactive_simulation::cmd_break(fnsim::args_list args)
{
    if (args.size() == 0)
        warn_incorrect_use_("break");

    if (args[0] == "show") {
        fmt::print("Showing all breakpoints");
    } else if (args[0] == "add") {
        fmt::print("Adding a breakpoint");
    } else if (args[0] == "del") {
        fmt::print("Removing a breakpoint");
    } else if (args[0] == "clear") {
        fmt::print("Clearing all breakpoints");
    } else {
        warn_incorrect_use_("break");
    }
}

void fnsim::interactive_simulation::cmd_watch(fnsim::args_list)
{
    // fmt::print_colored(fmt::BLUE, "* Watch!\n");
}

void fnsim::interactive_simulation::cmd_info(fnsim::args_list)
{
    fmt::print_colored(fmt::BLUE, "* Processor state:\n");
    print_reg_info(sim_.get_reg_info());
}

void fnsim::interactive_simulation::warn_unknown_cmd_(std::string cmd_name)
{
    fmt::print_colored(fmt::RED, "! Unknown command: {}\n", cmd_name);
    fmt::print("? Perhaps try one of the following:\n");
    cmd_help({});
}

void fnsim::interactive_simulation::warn_incorrect_use_(std::string cmd_name)
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

void fnsim::interactive_simulation::show_simulation_state_()
{
    auto [state, location] = sim_.get_state_info();
    fmt::print_colored(fmt::BLUE, "* Simulator {} @ 0x{:08X}\n", to_string(state), location);
}

static std::string get_user_cmd()
{
    fmt::print("> ");

    std::string cmd;
    getline(std::cin, cmd);

    return cmd;
}

void fnsim::interactive_simulation::init_cmd_descrs_()
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

        {'b', "break", &interactive_simulation::cmd_break, "[b]reak (show | add ADDR | del ADDR | clear)", "Show breakpoints | Add/remove breakpoint at ADDR | Clear all breakpoints"},
        {'w', "watch", &interactive_simulation::cmd_watch, "[w]atch (show | add ADDR | del ADDR | clear)", "Show watches | Add/remove watch at ADDR | Clear all watches"},
        {'i', "info", &interactive_simulation::cmd_info, "[i]nfo", "Display processor state (= value of it's registers)"}
    };
}

void fnsim::interactive_simulation::run()
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
    }
}

void fnsim::run_interactive_simulation(functional_simulation& sim, std::string pfile)
{
    interactive_simulation isim {sim, pfile};
    isim.run();
}
