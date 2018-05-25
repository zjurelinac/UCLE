#ifndef _UCLE_CORE_FNSIM_SIMULATIONS_INTERACTIVE_HPP_
#define _UCLE_CORE_FNSIM_SIMULATIONS_INTERACTIVE_HPP_

#include <fnsim/base.hpp>
#include <fnsim/cli.hpp>

#include <util/string.hpp>

#include <any>
#include <cstdlib>
#include <functional>
#include <string>
#include <vector>

namespace ucle::fnsim::cli {

    template <typename FunctionalSimulation>
    class interactive_simulation;

    using argument_list = std::vector<std::any>;

    struct interactive_cmd_info {
        std::string name;
        argument_list args;
    };

    template <typename FunctionalSimulation>
    using interactive_cmd_fn = std::function<void(interactive_simulation<FunctionalSimulation>&, argument_list)>;

    template <typename FunctionalSimulation>
    struct interactive_cmd_descr {
        using simulation_type = FunctionalSimulation;

        char shortcut;
        std::string full_name;
        interactive_cmd_fn<simulation_type> fn;
    };

    template <typename FunctionalSimulation>
    class interactive_simulation {
        public:
            using simulation_type = FunctionalSimulation;
            using descr_type = interactive_cmd_descr<simulation_type>;

            interactive_simulation() = delete;
            interactive_simulation(simulation_type& sim, config& cfg)
                : sim_{sim}, cfg_{cfg} { init_descriptors_(); }

            void run();

        protected:
            virtual interactive_cmd_info get_cmd_()  = 0;

            virtual void do_help_(argument_list)   = 0;
            virtual void do_run_(argument_list)    = 0;
            virtual void do_start_(argument_list)  = 0;
            virtual void do_cont_(argument_list)   = 0;
            virtual void do_step_(argument_list)   = 0;
            virtual void do_step_n_(argument_list) = 0;
            virtual void do_until_(argument_list)  = 0;
            virtual void do_reset_(argument_list)  = 0;
            virtual void do_quit_(argument_list)   = 0;
            virtual void do_break_(argument_list)  = 0;
            virtual void do_watch_(argument_list)  = 0;
            virtual void do_info_(argument_list)   = 0;

            virtual void info_(std::string) {}
            virtual void warn_(std::string) {}
            virtual void error_(std::string) {}
            virtual void success_(std::string) {}
            virtual void notify_(std::string) {}

            virtual void dispatch_(interactive_cmd_info& cinfo);

            virtual void report_unknown_cmd_(std::string cmd) { error_(fmt::format("Unknown command: {}. See help for available commands.", cmd)); }
            virtual void report_incorrect_use_(std::string cmd) { error_(fmt::format("Incorrect command call of {}. See help for correct usage options.", cmd)); }

            void quit_() { running_ = false; }
            void reload_()
            {
                sim_.reset();

                if (auto stat = sim_.load_pfile(cfg_.pfile); is_error(stat))
                    throw fatal_error(fmt::format("Cannot reload program from {}", cfg_.pfile));

                if (cfg_.verbosity > 0)
                    success_(fmt::format("Reloaded program from {}", cfg_.pfile));
            }

            simulation_type& get_sim_() { return sim_; }
            config& get_cfg_() { return cfg_; }

        private:
            void init_descriptors_()
            {
                descrs_ = {
                    { 'h', "help", &interactive_simulation::do_help_ },
                    { 'r', "run", &interactive_simulation::do_run_ },
                    { 'a', "start", &interactive_simulation::do_start_ },
                    { 'c', "cont", &interactive_simulation::do_cont_ },
                    { 's', "step", &interactive_simulation::do_step_ },
                    { 'n', "step_n", &interactive_simulation::do_step_n_ },
                    { 'u', "until", &interactive_simulation::do_until_ },
                    { 't', "reset", &interactive_simulation::do_reset_ },
                    { 'q', "quit", &interactive_simulation::do_quit_ },
                    { 'b', "break", &interactive_simulation::do_break_ },
                    { 'w', "watch", &interactive_simulation::do_watch_ },
                    { 'i', "info", &interactive_simulation::do_info_ }
                };
            }

            simulation_type& sim_;
            config& cfg_;
            std::vector<descr_type> descrs_;

            bool running_ {true};
    };

    template <typename FunctionalSimulation>
    void interactive_simulation<FunctionalSimulation>::run()
    {
        if (cfg_.verbosity > 0)
            info_("Starting interactive simulation...");

        if (auto stat = sim_.load_pfile(cfg_.pfile); is_error(stat)) {
            error_(fmt::format("Error loading program from {}", cfg_.pfile));
            std::exit(1);
        }

        if (cfg_.verbosity > 0)
            success_(fmt::format("Loaded program from {}", cfg_.pfile));

        while (running_) {
            try {
                auto cinfo = get_cmd_();
                dispatch_(cinfo);
            } catch (unknown_command& e) {
                report_unknown_cmd_(e.what());
            } catch (incorrect_call& e) {
                report_incorrect_use_(e.what());
            } catch (fatal_error& e) {
                error_(fmt::format("Fatal error: {}", e.what()));
                std::exit(1);
            } catch (base_exception& e) {
                error_(fmt::format("Error: {}", e.what()));
            }
        }

        if (cfg_.verbosity > 0)
            info_("Ending interactive simulation.");
    }

    template <typename FunctionalSimulation>
    void interactive_simulation<FunctionalSimulation>::dispatch_(interactive_cmd_info& cinfo)
    {
        for (const auto& descr : descrs_)
            if ((cinfo.name.size() == 1 && cinfo.name[0] == descr.shortcut) || cinfo.name == descr.full_name) {
                descr.fn(*this, cinfo.args);
                return;
            }

        throw unknown_command(cinfo.name);
    }

}

#endif  /* _UCLE_CORE_FNSIM_SIMULATIONS_INTERACTIVE_HPP_ */
