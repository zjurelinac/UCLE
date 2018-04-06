#ifndef _UCLE_CORE_FNSIM_SIMULATION_HPP_
#define _UCLE_CORE_FNSIM_SIMULATION_HPP_

#include <common/structures.hpp>
#include <common/types.hpp>

#include <fnsim/base.hpp>
#include <fnsim/device.hpp>
#include <fnsim/exceptions.hpp>
#include <fnsim/processor.hpp>

#include <memory>
#include <set>
#include <unordered_map>
#include <utility>

namespace ucle::fnsim {

    class functional_simulation {
        public:

            functional_simulation() = delete;
            functional_simulation(functional_processor_simulator_ptr fnsim_ptr) : fnsim_(std::move(fnsim_ptr)) {}

            // Basic simulation functionality

            status start(address_t start_location = 0) noexcept;
            status run(address_t start_location = 0) noexcept;
            status cont() noexcept;
            status step() noexcept;
            status step_n(size_t num_steps) noexcept;
            status until(address_t location) noexcept;
            status reset() noexcept;
            status quit() noexcept;

            // Program loading

            status load_pfile(std::string filename, address_t start_location = 0) noexcept;   // TODO: Start location? What if code not PIC?

            // Memory
            auto get_memory_contents(address_t location, size_t amount)
            {
                small_byte_vector bytes;
                while (amount--)
                    bytes.push_back(fnsim_->get_byte(location++));
                return bytes;
            }
            void set_memory_contents(address_t location, small_byte_vector& bytes)
            {
                for (auto i = 0u; i < bytes.size(); ++i)
                    fnsim_->set_byte(location++, bytes[i]);
            }

            // Breakpoints

            auto get_breakpoints()
            {
                return breakpts_;
            }

            void add_breakpoint(address_t breakpoint)
            {
                breakpts_.insert(breakpoint);
            }

            void remove_breakpoint(address_t breakpoint)
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

            void add_watch(address_t location)
            {
                watches_.insert(location);
            }

            void remove_watch(address_t location)
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

            std::string get_asm_annotation_(address_t location)
            {
                return asm_annotations_[location];
            }

            void set_asm_annotation_(address_t location, std::string annotation)
            {
                asm_annotations_[location] = annotation;
            }

            bool is_breakpoint_(address_t location) const { return breakpts_.count(location) || tmp_breakpts_.count(location); }
            void clear_tmp_breakpoints_(address_t location) { tmp_breakpts_.erase(location); }

            functional_processor_simulator_ptr          fnsim_;
            std::set<address_t>                         breakpts_;
            std::set<address_t>                         tmp_breakpts_;
            std::set<address_t>                         watches_;
            std::unordered_map<address_t, std::string>  asm_annotations_;
    };

    void print_reg_info(reg_info ri);

}

#endif  /* _UCLE_CORE_FNSIM_SIMULATION_HPP_ */
