#ifndef _UCLE_CORE_FNSIM_INTERACTIVE_SIM_HPP_
#define _UCLE_CORE_FNSIM_INTERACTIVE_SIM_HPP_

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
            void show_correct_use_();
            void show_simulation_state_();

            functional_simulation& sim_;
            std::string pfile_;

            bool running_ {true};

            std::vector<interactive_cmd_descr> icmds_;
    };

    void run_interactive_simulation(functional_simulation& sim, std::string pfile);
}

#endif  /* _UCLE_CORE_FNSIM_INTERACTIVE_SIM_HPP_ */
