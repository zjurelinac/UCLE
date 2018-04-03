#include <fnsim/interactive_sim.hpp>

#include <util/string.hpp>

#include <libs/fmt/format.h>

#include <algorithm>
#include <iostream>

namespace fnsim = ucle::fnsim;

void fnsim::interactive_simulation::cmd_help(fnsim::args_list)
{
    fmt::print("  Help - interactive processor simulation\n");
    fmt::print("==============================================================\n");
    std::vector<std::pair<std::string, std::string>> help = {
        {"[r]un", "Run simulation from the start"},
        {"[c]ont", "Continue paused simulation"},
        {"[s]tep", "Execute one simulation step (ie. instruction)"},
        {"[u]ntil ADDR", "Run simulation until reaching ADDR"},
        {"rese[t]", "Reset simulator - clear registers and memory contents"},
        {"[q]uit", "Quit interactive simulation"},
        {"", ""},
        {"[b]reak add ADDR", "Add a breakpoint at the ADDR"},
        {"[b]reak del ADDR", "Remove breakpoint from the ADDR"},
        {"[b]reak clear", "Remove all breakpoints"},
        {"", ""},
        {"[w]atch add ADDR", "N/A"},
        {"[w]atch del ADDR", "N/A"},
        {"[w]atch clear", "N/A"},
        {"", ""},
        {"[i]nfo", "Display processor state (= value of it's registers)"}
    };

    for (const auto& [help_cmd, help_desc] : help) {
        fmt::print_colored(fmt::CYAN, "  {:<20}", help_cmd);
        fmt::print("{} {}\n", (help_desc.size() > 0 ? '-' : ' '), help_desc);
    }

    fmt::print("\n");
}

void fnsim::interactive_simulation::cmd_run(fnsim::args_list)
{
    fmt::print_colored(fmt::BLUE, "Run!\n");
}

void fnsim::interactive_simulation::cmd_cont(fnsim::args_list)
{
    fmt::print_colored(fmt::BLUE, "Cont!\n");
}

void fnsim::interactive_simulation::cmd_step(fnsim::args_list)
{
    fmt::print_colored(fmt::BLUE, "Step!\n");
}

void fnsim::interactive_simulation::cmd_until(fnsim::args_list)
{
    fmt::print_colored(fmt::BLUE, "Until!\n");
}

void fnsim::interactive_simulation::cmd_reset(fnsim::args_list)
{
    fmt::print_colored(fmt::BLUE, "Reset!\n");
}

void fnsim::interactive_simulation::cmd_quit(fnsim::args_list)
{
    fmt::print_colored(fmt::BLUE, "Quit!\n");
    running_ = false;
}

void fnsim::interactive_simulation::cmd_break(fnsim::args_list)
{
    fmt::print_colored(fmt::BLUE, "Break!\n");
}

void fnsim::interactive_simulation::cmd_watch(fnsim::args_list)
{
    fmt::print_colored(fmt::BLUE, "Watch!\n");
}

void fnsim::interactive_simulation::cmd_info(fnsim::args_list)
{
    fmt::print_colored(fmt::BLUE, "Info!\n");
}

static std::string get_user_cmd()
{
    fmt::print("> ");

    std::string cmd;
    getline(std::cin, cmd);

    return cmd;
}

void fnsim::interactive_simulation::show_simulation_state()
{

}

// fmt colors { BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE };
void fnsim::interactive_simulation::run()
{
    fnsim::interactive_cmd_descr icmds[] = {
        {'h', "help", &interactive_simulation::cmd_help},

        {'r', "run", &interactive_simulation::cmd_run},
        {'c', "cont", &interactive_simulation::cmd_cont},
        {'s', "step", &interactive_simulation::cmd_step},
        {'u', "until", &interactive_simulation::cmd_until},
        {'t', "reset", &interactive_simulation::cmd_reset},
        {'q', "quit", &interactive_simulation::cmd_quit},

        {'b', "break", &interactive_simulation::cmd_break},
        {'w', "watch", &interactive_simulation::cmd_watch},
        {'i', "info", &interactive_simulation::cmd_info}
    };

    fmt::print_colored(fmt::YELLOW, "Starting interactive simulation...\n");

    sim_.load_pfile(pfile_);

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
        for (const auto& icmd : icmds)
            if ((cmd_name.size() == 1 && icmd.shortcut == cmd_name[0]) ||
                (cmd_name.size() > 1 && icmd.name == cmd_name)) {
                    icmd.cmd(*this, args);
                    found_cmd = true;
                    break;
                }

        if (!found_cmd) {
            fmt::print_colored(fmt::RED, "  Unknown command: {}!\n\n", cmd_name);
            cmd_help({});
        }
    }

    fmt::print_colored(fmt::YELLOW, "Ending interactive simulation.\n");
}


void fnsim::run_interactive_simulation(functional_simulation& sim, std::string pfile)
{
    interactive_simulation isim {sim, pfile};
    isim.run();
}
