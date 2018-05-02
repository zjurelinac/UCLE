#ifndef _UCLE_CORE_FNSIM_SIMULATIONS_INTERACTIVE_JSON_HPP_
#define _UCLE_CORE_FNSIM_SIMULATIONS_INTERACTIVE_JSON_HPP_

#include <fnsim/base.hpp>

#include <fnsim/simulations/interactive.hpp>

#include <libs/fmt/format.h>
#include <libs/nlohmann/json.hpp>

#include <iostream>

namespace ucle::fnsim::cli {

    template <typename FunctionalSimulation>
    class interactive_json_simulation : public interactive_simulation<FunctionalSimulation> {
        using interactive_simulation<FunctionalSimulation>::interactive_simulation;
        using json = nlohmann::json;

        protected:
            interactive_cmd_info get_cmd_() override
            {
                std::string input_str;
                std::getline(std::cin, input_str);

                try {
                    auto input = json::parse(input_str);

                    if (input.count("command") == 0 || !input["command"].is_string() || input.count("args") == 0 || !input["args"].is_array())
                        throw json_parse_error("Malformed JSON command - not all fields present!");

                    interactive_cmd_info cinfo = {input["command"], {}};
                    for (const auto& arg : input["args"]) {
                        if (arg.is_string())
                            cinfo.args.emplace_back(arg.get<std::string>());
                        else if (arg.is_number())
                            cinfo.args.emplace_back(arg.get<int>());
                        else if (arg.is_boolean())
                            cinfo.args.emplace_back(arg.get<bool>());
                        else
                            throw json_parse_error("Malformed JSON command - unsupported argument type!");
                    }

                    return cinfo;
                } catch (nlohmann::detail::parse_error& e) {
                    throw json_parse_error("Malformed JSON command - cannot parse it!");
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

            void info_(std::string msg) override       { status_report_("info", msg); }
            void warn_(std::string msg) override       { status_report_("warning", msg); }
            void error_(std::string msg) override      { status_report_("error", msg); }
            void success_(std::string msg) override    { status_report_("success", msg); }
            void notify_(std::string msg) override     { status_report_("notify", msg); }

        private:
            void status_report_(std::string status, std::string msg)
            {
                json j = { { "type", "status" }, { "status", status }, { "message", msg } };
                std::cout << j << std::endl;
            }

            void show_simulation_state_()
            {
                auto [state, location, annotation] = this->get_sim_().get_state_info();
                json j = { { "type", "state_info" }, { "state", to_string(state) }, { "location", location }, { "annotation", annotation } };
                std::cout << j << std::endl;
            }

            void show_simulation_state_(std::string msg)
            {
                auto [state, location, annotation] = this->get_sim_().get_state_info();
                json j = { { "type", "state_info" }, { "state", to_string(state) }, { "location", location }, { "annotation", annotation }, { "message", msg } };
                std::cout << j << std::endl;
            }
    };

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_help_(argument_list)
    {
        notify_("Help command unsupported in this simulation mode.");
    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_run_(argument_list args)
    {
        address32_t start_location = 0;

        if (args.size() > 0)
            start_location = std::any_cast<int>(args[0]);

        if (auto stat = this->get_sim_().start(start_location); is_error(stat))
            throw runtime_error(to_string(stat));

        if (auto stat = this->get_sim_().cont(); is_error(stat))
            throw runtime_error(to_string(stat));

        show_simulation_state_("Ran the simulation.");
    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_start_(argument_list args)
    {
        address32_t start_location = 0;

        if (args.size() > 0)
            start_location = std::any_cast<int>(args[0]);

        if (auto stat = this->get_sim_().start(start_location); is_error(stat))
            throw runtime_error(to_string(stat));

        show_simulation_state_("Program successfuly started");
    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_cont_(argument_list)
    {
        if (auto stat = this->get_sim_().cont(); is_error(stat))
            throw runtime_error(to_string(stat));

        show_simulation_state_("Continued the simulation.");
    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_step_(argument_list)
    {
        if (auto stat = this->get_sim_().step(); is_error(stat))
            throw runtime_error(to_string(stat));

        show_simulation_state_("Executed one simulation step.");
    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_step_n_(argument_list args)
    {
        if (args.size() == 0)
            throw incorrect_call("step_n");

        auto num_steps = std::any_cast<int>(args[0]);

        if (auto stat = this->get_sim_().step_n(num_steps); is_error(stat))
            throw runtime_error(to_string(stat));

        show_simulation_state_(fmt::format("Executed {} steps of the simulation.", num_steps));
    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_until_(argument_list args)
    {
        if (args.size() == 0)
            throw incorrect_call("until");

        address32_t end_location = std::any_cast<int>(args[0]);

        if (auto stat = this->get_sim_().until(end_location); is_error(stat))
            throw runtime_error(to_string(stat));

        show_simulation_state_(fmt::format("Ran simulation until address {}.", end_location));
    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_reset_(argument_list)
    {
        this->reload_();
        show_simulation_state_("Simulation successfully resetted.");
    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_quit_(argument_list)
    {
        this->quit_();
        success_("Simulation terminated.");
    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_break_(argument_list args)
    {
        if (args.size() == 0)
            throw incorrect_call("break");

        auto subcmd = std::any_cast<std::string>(args[0]);

        if (subcmd == "list") {
            auto breakpoints = this->get_sim_().get_breakpoints();
            json j = { { "type", "breakpoint_info" }, { "breakpoints", breakpoints } };
            std::cout << j << std::endl;
        } else if (subcmd == "add") {
            if (args.size() < 2)
                throw incorrect_call("break");

            address32_t location = std::any_cast<int>(args[1]);

            this->get_sim_().add_breakpoint(location);
            success_(fmt::format("Added breakpoint @ 0x{:08X}", location));

        } else if (subcmd == "del") {
            if (args.size() < 2)
                throw incorrect_call("break");

            address32_t location = std::any_cast<int>(args[1]);

            this->get_sim_().remove_breakpoint(location);
            success_(fmt::format("Removed breakpoint @ 0x{:08X}", location));

        } else if (subcmd == "clear") {
            this->get_sim_().clear_breakpoints();
            success_("Cleared all breakpoints");
        } else {
            this->report_incorrect_use_("break");
        }
    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_watch_(argument_list)
    {
        error_("Watch command unsupported as of yet.");
    }

    template <typename FunctionalSimulation>
    void interactive_json_simulation<FunctionalSimulation>::do_info_(argument_list)
    {
        json j = { { "type", "register_info" }, { "registers", this->get_sim_().get_reg_info() } };
        std::cout << j << std::endl;
    }

    template <typename FunctionalSimulation>
    void run_interactive_json_simulation(FunctionalSimulation& sim, config& cfg)
    {
        interactive_json_simulation<FunctionalSimulation> tsim {sim, cfg};
        tsim.run();
    }
}

#endif  /* _UCLE_CORE_FNSIM_SIMULATIONS_INTERACTIVE_JSON_HPP_ */
