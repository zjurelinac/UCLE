#pragma once

#include <fnsim/base.hpp>
#include <fnsim/cli.hpp>

#include <util/string.hpp>

#include <any>
#include <cstdlib>
#include <functional>
#include <string>
#include <vector>

namespace ucle::fnsim::cli {

    struct isim_config {
        std::string program_path;
        unsigned verbosity;
    };

    using argument_list = std::vector<std::string>;

    template <typename FunctionalSimulation>
    class interactive_simulation {

        using cmd_fn_type = std::function<void(interactive_simulation, argument_list)>;

        struct cmd_info {
            cmd_fn_type cmd;
            argument_list args;
        };

        public:
            using fnsim_type = FunctionalSimulation;

            // enum class message_type { note, info, success, warn, error };

            interactive_simulation() = delete;
            interactive_simulation(fnsim_type& fnsim, isim_config& cfg)
                : fnsim_{ fnsim }, cfg_{ cfg } {}
            virtual ~interactive_simulation() = default;

            void run();

        protected:
            // virtual void msg_(std::string text, message_type mtype) = 0;

            isim_config& config_() { return cfg_; }

        private:
            void do_help_(argument_list args);
            void do_run_(argument_list args);
            void do_start_(argument_list args);
            void do_cont_(argument_list args);
            void do_step_(argument_list args);
            void do_step_n_(argument_list args);
            void do_until_(argument_list args);
            void do_reset_(argument_list args);
            void do_quit_(argument_list args);
            void do_break_(argument_list args);
            void do_watch_(argument_list args);
            void do_info_(argument_list args);


            // void error_msg_(std::string text) { msg_(text, message_type::error); }
            // void warn_msg_(std::string text) { msg_(text, message_type::warn); }
            // void succ_msg_(std::string text) { msg_(text, message_type::success); }
            // void info_msg_(std::string text) { msg_(text, message_type::info); }
            // void note_msg_(std::string text) { if (cfg_.verbosity > 0) msg_(text, message_type::note); }

            fnsim_type fnsim_;
            isim_config cfg_;

            bool running_ { true };
    };

    template <typename FunctionalSimulation>
    void interactive_simulation<FunctionalSimulation>::run()
    {
        // note_msg_("Starting interactive simulation...");

        if (auto stat = fnsim_.load_pfile(cfg_.program_path); is_error(stat)) {
            // error_msg_(fmt::format("Error loading program from {}", cfg_.pfile));
            std::exit(1);
        }

        // note_msg_(fmt::format("Loaded program from {}", cfg_.pfile));

        while (running_) {
            // try {
            //     auto cinfo = get_cmd_();
            //     dispatch_(cinfo);
            // } catch (unknown_command& e) {
            //     report_unknown_cmd_(e.what());
            // } catch (incorrect_call& e) {
            //     report_incorrect_use_(e.what());
            // } catch (fatal_error& e) {
            //     error_(fmt::format("Fatal error: {}", e.what()));
            //     std::exit(1);
            // } catch (base_exception& e) {
            //     error_(fmt::format("Error: {}", e.what()));
            // }
        }

        // note_msg_("Ending interactive simulation.");
    }

    template <typename FunctionalSimulation>
    void interactive_simulation<FunctionalSimulation>::do_help_(argument_list args)
    {

    }

    template <typename FunctionalSimulation>
    void interactive_simulation<FunctionalSimulation>::do_run_(argument_list args)
    {
        address32_t start_location = 0;

        if (args.size() > 0) {
            if (!util::parse_int<address32_t>(args[0], &start_location))
                throw malformed_argument(fmt::format("Cannot parse argument as an address (must be an integer >= 0)"));
        }

        if (auto stat = fnsim_.start(start_location); is_error(stat))
            throw runtime_error(to_string(stat));

        if (auto stat = fnsim_.cont(start_location); is_error(stat))
            throw runtime_error(to_string(stat));


    }

    template <typename FunctionalSimulation>
    void interactive_simulation<FunctionalSimulation>::do_start_(argument_list args)
    {
        address32_t start_location = 0;

        if (args.size() > 0) {
            if (!util::parse_int<address32_t>(args[0], &start_location))
                throw malformed_argument(fmt::format("Cannot parse argument as an address (must be an integer >= 0)"));
        }

        if (auto stat = fnsim_.start(start_location); is_error(stat))
            throw runtime_error(to_string(stat));
    }

    template <typename FunctionalSimulation>
    void interactive_simulation<FunctionalSimulation>::do_cont_(argument_list args)
    {
        if (auto stat = fnsim_.cont(); is_error(stat))
            throw runtime_error(to_string(stat));
    }

    template <typename FunctionalSimulation>
    void interactive_simulation<FunctionalSimulation>::do_step_(argument_list args)
    {
        if (auto stat = fnsim_.step(); is_error(stat))
            throw runtime_error(to_string(stat));
    }

    template <typename FunctionalSimulation>
    void interactive_simulation<FunctionalSimulation>::do_step_n_(argument_list args)
    {
        if (args.size() == 0)
            throw incorrect_call("step_n");

        int num_steps;
        if (!util::parse_int<int>(args[0], &num_steps) || num_steps <= 0)
            throw malformed_argument(fmt::format("Cannot parse argument as number of steps (must be an integer > 0)"));

        // notify_(fmt::format("Executing {} steps of the simulation...", num_steps));

        if (auto stat = fnsim_.step_n(num_steps); is_error(stat))
            throw runtime_error(to_string(stat));
    }

    template <typename FunctionalSimulation>
    void interactive_simulation<FunctionalSimulation>::do_until_(argument_list args)
    {
        if (args.size() == 0)
            throw incorrect_call("until");

        address32_t end_location;
        if (!util::parse_int<address32_t>(args[0], &end_location))
            throw malformed_argument(fmt::format("Cannot parse argument as an address (must be an integer >= 0)"));

        // notify_(location_action_("Executing simulation until", end_location));

        if (auto stat = fnsim_.until(end_location); is_error(stat))
            throw runtime_error(to_string(stat));

    }

    template <typename FunctionalSimulation>
    void interactive_simulation<FunctionalSimulation>::do_reset_(argument_list args)
    {
        fnsim_.reset();

        if (auto stat = fnsim_.load_pfile(cfg_.program_path); is_error(stat))
            throw fatal_error(fmt::format("Cannot reload program from {}", cfg_.program_path));

        // note_msg_(fmt::format("Reloaded program from {}", cfg_.program_path));
    }

    template <typename FunctionalSimulation>
    void interactive_simulation<FunctionalSimulation>::do_quit_(argument_list args)
    {
        running_ = false;
    }

    template <typename FunctionalSimulation>
    void interactive_simulation<FunctionalSimulation>::do_break_(argument_list args)
    {
        if (args.size() == 0)
            throw incorrect_call("break");

        if (args[0] == "list") {
            auto breakpoints = this->get_sim_().get_breakpoints();
            // info_(fmt::format("Active breakpoints ({}):", breakpoints.size()));

            // auto i = 0u;
            // for (const auto bp : breakpoints)
            //     notify_(location_action_(fmt::format("Breakpoint {}", ++i), bp));

        } else if (args[0] == "add" || args[0] == "del") {
            if (args.size() < 2)
                throw incorrect_call("break");

            address32_t location;
            if (!util::parse_int<address32_t>(args[1], &location))
                throw malformed_argument(fmt::format("Cannot parse argument as an address (must be an integer >= 0)"));

            if (args[0] == "add") {
                fnsim_.add_breakpoint(location);
                // success_(location_action_("Added breakpoint", location));
            } else {
                fnsim_.remove_breakpoint(location);
                // success_(location_action_("Removed breakpoint", location));
            }
        } else if (args[0] == "clear") {
            fnsim_.clear_breakpoints();
            // success_("Cleared all breakpoints");
        } else {
            this->report_incorrect_use_("break");
        }
    }

    template <typename FunctionalSimulation>
    void interactive_simulation<FunctionalSimulation>::do_watch_(argument_list args)
    {

    }

    template <typename FunctionalSimulation>
    void interactive_simulation<FunctionalSimulation>::do_info_(argument_list args)
    {
        // print_reg_info(this->get_sim_().get_reg_info());

    }

}
