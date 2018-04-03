#ifndef _UCLE_CORE_FNSIM_INTERACTIVE_SIM_HPP_
#define _UCLE_CORE_FNSIM_INTERACTIVE_SIM_HPP_

#include <fnsim/simulation.hpp>

#include <functional>
#include <string>
#include <vector>

/*
    Interactive simulator commands:
        r - run
        c - cont
        s - step
        u - until
        r - reset
        q - quit

        b add ADDR  - add_breakpoint
        del ADDR  - remove_breakpoint
        clear     - clear_breakpoints
        clear_all - clear_all_breakpoints

        w add ADDR  - add_watch
        del ADDR  - remove_watch
        clear     - clear_watches

        i - get_reg_info
*/

namespace ucle::fnsim {

    using args_list = std::vector<std::string>;

    class interactive_simulation {
        public:
            interactive_simulation() = delete;
            interactive_simulation(functional_simulation& sim) : sim_(sim) {};

            void run();

        private:
            void cmd_help(args_list);

            void cmd_run(args_list);
            void cmd_cont(args_list);
            void cmd_step(args_list);
            void cmd_until(args_list);
            void cmd_reset(args_list);
            void cmd_quit(args_list);

            void cmd_break(args_list);
            void cmd_watch(args_list);
            void cmd_info(args_list);

            functional_simulation& sim_;
            bool running {true};
    };

    using interactive_cmd = std::function<void(interactive_simulation&, args_list)>;

    struct interactive_cmd_descr {
        char shortcut;
        std::string name;
        interactive_cmd cmd;
    };

    void run_interactive_simulation(functional_simulation& sim);
}

#endif  /* _UCLE_CORE_FNSIM_INTERACTIVE_SIM_HPP_ */