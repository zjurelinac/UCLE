#ifndef _UCLE_CORE_FNSIM_INTERACTIVE_SIM_HPP_
#define _UCLE_CORE_FNSIM_INTERACTIVE_SIM_HPP_

#include <fnsim/base.hpp>
#include <fnsim/simulation.hpp>

#include <functional>
#include <string>
#include <vector>

namespace ucle::fnsim {

    using args_list = std::vector<std::string>;

    class interactive_simulation;

    using interactive_cmd = std::function<void(interactive_simulation&, args_list)>;

    struct interactive_cmd_descr {
        char shortcut;
        std::string name;
        interactive_cmd cmd;
        std::string use;
        std::string descr;
    };

    inline void print_icmd_descr(const interactive_cmd_descr& icmd)
    {
        if (icmd.use.size() > 16) {
            fmt::print_colored(fmt::CYAN, "  {}\n", icmd.use);
            fmt::print("  {:<16}{}\n", "", icmd.descr);
        } else {
            fmt::print_colored(fmt::CYAN, "  {:<16}", icmd.use);
            fmt::print("{}\n", icmd.descr);
        }
    }

    class interactive_simulation {
        public:
            interactive_simulation() = delete;
            interactive_simulation(functional_simulation& sim, std::string pfile)
                : sim_(sim), pfile_(pfile) { init_cmd_descrs_(); }

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

            functional_simulation& sim_;
            std::string pfile_;

            bool running_ {true};

            std::vector<interactive_cmd_descr> icmds_;
    };

    void run_interactive_simulation(functional_simulation& sim, std::string pfile);
}

#endif  /* _UCLE_CORE_FNSIM_INTERACTIVE_SIM_HPP_ */
