#ifndef _UCLE_CORE_FNSIM_SIMULATION_HPP_
#define _UCLE_CORE_FNSIM_SIMULATION_HPP_

#include <common/structures.hpp>
#include <common/types.hpp>

#include <fnsim/base.hpp>
#include <fnsim/device.hpp>
#include <fnsim/exceptions.hpp>
#include <fnsim/processor.hpp>

#include <libs/fmt/format.h>

#include <util/string.hpp>

#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ucle::fnsim {

    template <typename AddressType = address_t>
    class functional_simulation {
        public:
            using address_type = AddressType;

            functional_simulation() = delete;
            functional_simulation(functional_processor_simulator_ptr fnsim_ptr) : fnsim_(std::move(fnsim_ptr)) {}

            // Basic simulation functionality

            status start(address_type start_location = 0) noexcept;
            status run(address_type start_location = 0) noexcept;
            status cont() noexcept;
            status step() noexcept;
            status step_n(size_t num_steps) noexcept;
            status until(address_type location) noexcept;
            status reset() noexcept;
            status quit() noexcept;

            // Program loading

            status load_pfile(std::string filename, address_type start_location = 0) noexcept;   // TODO: Start location? What if code not PIC?

            // Memory
            auto get_memory_contents(address_type location, size_t amount)
            {
                small_byte_vector bytes;
                while (amount--)
                    bytes.push_back(fnsim_->get_byte(location++));
                return bytes;
            }
            void set_memory_contents(address_type location, small_byte_vector& bytes)
            {
                for (auto i = 0u; i < bytes.size(); ++i)
                    fnsim_->set_byte(location++, bytes[i]);
            }

            // Breakpoints

            auto get_breakpoints()
            {
                return breakpts_;
            }

            void add_breakpoint(address_type breakpoint)
            {
                breakpts_.insert(breakpoint);
            }

            void remove_breakpoint(address_type breakpoint)
            {
                breakpts_.erase(breakpoint);
            }

            void clear_breakpoints()
            {
                breakpts_.clear();
            }

            void clear_all_breakpoints()
            {
                breakpts_.clear();
                tmp_breakpts_.clear();
            }

            // Watches

            auto get_watches()
            {
                return watches_;
            }

            void add_watch(address_type location)
            {
                watches_.insert(location);
            }

            void remove_watch(address_type location)
            {
                watches_.erase(location);
            }

            void clear_watches()
            {
                watches_.clear();
            }

            // Devices

            std::optional<identifier_t> add_device(device_ptr dev_ptr, device_config cfg) noexcept
            {
                try {
                    return fnsim_->add_device(std::move(dev_ptr), cfg);
                } catch (std::exception &e) {
                    return std::nullopt;
                }
            }

            status remove_device(identifier_t dev_id) noexcept
            {
                try {
                    fnsim_->remove_device(dev_id);
                    return status::ok;
                } catch (std::exception &e) {
                    return status::invalid_identifier;
                }
            }

            // Runtime info

            reg_info get_reg_info()
            {
                return fnsim_->get_reg_info();
            }

            state_info get_state_info()
            {
                return { 
                    fnsim_->get_state(),
                    fnsim_->get_program_counter(),
                    get_asm_annotation_(fnsim_->get_program_counter())
                };
            }

            // mem_info
            // backtrace
            // frame_info (?)

        private:

            void step_()
            {
                try {
                    auto status = fnsim_->execute_single();

                    if (is_error(status)) {
                        fnsim_->set_state(simulator_state::exception);
                    } else {
                        auto pc = fnsim_->get_program_counter();

                        if (is_breakpoint_(pc)) {
                            fnsim_->set_state(simulator_state::stopped);
                            clear_tmp_breakpoints_(pc);
                        }
                    }
                } catch (std::exception& e) {
                    fnsim_->set_state(simulator_state::exception);
                }
            }

            status run_()
            {
                do { step_(); } while (fnsim_->get_state() == simulator_state::running);
                return fnsim_->get_state() != simulator_state::exception ? status::ok : status::runtime_exception;
            }

            std::string get_asm_annotation_(address_type location)
            {
                return asm_annotations_[location];
            }

            void set_asm_annotation_(address_type location, std::string annotation)
            {
                asm_annotations_[location] = annotation;
            }

            bool is_breakpoint_(address_type location) const { return breakpts_.count(location) || tmp_breakpts_.count(location); }
            void clear_tmp_breakpoints_(address_type location) { tmp_breakpts_.erase(location); }

            functional_processor_simulator_ptr fnsim_;
            std::set<address_type> breakpts_;
            std::set<address_type> tmp_breakpts_;
            std::set<address_type> watches_;
            std::unordered_map<address_type, std::string> asm_annotations_;
    };

    template <typename AddressType>
    inline status functional_simulation<AddressType>::start(address_type start_location) noexcept {
        if (fnsim_->get_state() != simulator_state::loaded)
            return status::invalid_state;

        fnsim_->set_state(simulator_state::stopped);
        fnsim_->set_program_counter(start_location);

        return status::ok;
    }

    template <typename AddressType>
    inline status functional_simulation<AddressType>::run(address_type start_location) noexcept {
        if (fnsim_->get_state() != simulator_state::loaded)
            return status::invalid_state;

        fnsim_->set_state(simulator_state::running);
        fnsim_->set_program_counter(start_location);

        return run_();
    }

    template <typename AddressType>
    inline status functional_simulation<AddressType>::cont() noexcept {
        if (fnsim_->get_state() != simulator_state::stopped)
            return status::invalid_state;

        fnsim_->set_state(simulator_state::running);

        return run_();
    }

    template <typename AddressType>
    inline status functional_simulation<AddressType>::step() noexcept {
        if (fnsim_->get_state() != simulator_state::stopped)
            return status::invalid_state;

        fnsim_->set_state(simulator_state::running);
        step_();

        if (fnsim_->get_state() == simulator_state::running)
            fnsim_->set_state(simulator_state::stopped);

        return fnsim_->get_state() != simulator_state::exception ? status::ok : status::runtime_exception;
    }

    template <typename AddressType>
    inline status functional_simulation<AddressType>::step_n(size_t num_steps) noexcept
    {
        if (fnsim_->get_state() != simulator_state::stopped)
            return status::invalid_state;

        fnsim_->set_state(simulator_state::running);

        do { step_(); } while (fnsim_->get_state() == simulator_state::running && num_steps-- > 0);

        if (fnsim_->get_state() == simulator_state::running)
            fnsim_->set_state(simulator_state::stopped);

        return fnsim_->get_state() != simulator_state::exception ? status::ok : status::runtime_exception;
    }

    template <typename AddressType>
    inline status functional_simulation<AddressType>::until(address_type location) noexcept {
        if (fnsim_->get_state() != simulator_state::stopped)
            return status::invalid_state;

        fnsim_->set_state(simulator_state::running);

        tmp_breakpts_.insert(location);
        return run_();
    }

    template <typename AddressType>
    inline status functional_simulation<AddressType>::reset() noexcept {
        fnsim_->reset();
        fnsim_->set_state(simulator_state::initialized);
        return status::ok;
    }

    template <typename AddressType>
    inline status functional_simulation<AddressType>::quit() noexcept {
        fnsim_->set_state(simulator_state::terminated);
        return status::ok;
    }

    template <typename AddressType>
    inline status functional_simulation<AddressType>::load_pfile(std::string filename, address_type start_location) noexcept {
        constexpr int pf_line_bound = 21;

        std::ifstream pfile(filename);

        if (pfile.bad())
            return status::filesystem_error;

        try {
            while (!pfile.eof()) {
                std::string line;
                std::getline(pfile, line);

                std::string code = util::trim_copy(line.substr(0, pf_line_bound));
                std::string annotation = line.size() > pf_line_bound ? util::trim_copy(line.substr(pf_line_bound)) : "";

                if (code.size() == 0) continue;

                std::istringstream iss(code);
                address_t address;
                unsigned byte;

                iss >> std::hex >> address;
                address += start_location;

                set_asm_annotation_(address, annotation);

                small_byte_vector bytes;
                while (iss >> byte)
                    bytes.push_back(static_cast<byte_t>(byte));

                set_memory_contents(address, bytes);
            }
        } catch (std::exception& e) {
            fnsim_->set_state(simulator_state::exception);
            return status::invalid_program;
        }

        pfile.close();
        fnsim_->set_state(simulator_state::loaded);

        return status::ok;
    }

    void print_reg_info(reg_info ri)
    {
        for (const auto [name, value] : ri)
            fmt::print("{} = {} = {}\n", name, fnsim::to_xstring(value), fnsim::to_string(value));
    }
}

#endif  /* _UCLE_CORE_FNSIM_SIMULATION_HPP_ */
