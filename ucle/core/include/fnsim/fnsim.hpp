#ifndef _UCLE_CORE_FNSIM_FNSIM_HPP_
#define _UCLE_CORE_FNSIM_FNSIM_HPP_

#include <common/exceptions.hpp>
#include <common/types.hpp>

#include <memory>
#include <set>
#include <utility>
#include <vector>

namespace ucle::fnsim {

    enum class byte_order { LE, BE };
    enum class device_mapping { DEFAULT, MEMORY, PORT, NONE };

    // TODO: Select possible options
    enum class device_status {};

    class device {
        // Abstract base class for all devices, both memory-mapped and isolated

        public:
            virtual void work() = 0;
            virtual void status() = 0;
            virtual void reset() = 0;
    };

    using device_ptr = std::shared_ptr<device>;

    struct device_config {
        bool            is_addressable = true;
        bool            uses_interrupts = false;
        device_mapping  mapping = device_mapping::DEFAULT;
        address_range   addr_range = {0, 0};
        priority_t      interrupt_priority;
    };

    enum class simulator_state { initialized, loaded, running, stopped, exception, terminated };

    class functional_simulator {
        public:
            virtual ~functional_simulator() {};

            // Basic simulation functionality

            status start(address_t start_location = 0);
            status run(address_t start_location = 0);
            status cont();
            status step();
            status until(address_t location);
            status reset();
            status quit();

            // Program loading

            status load_pfile(std::string filename, address_t start_location = 0);   // TODO: Start location? What if code not PIC?

            // Memory
            auto get_memory_contents(address_t location, size_t amount)
            {
                std::vector<byte_t> contents;
                while (amount--)
                    contents.push_back(get_byte_(location++));
                return contents;
            }
            void set_memory_contents(address_t location, std::vector<byte_t>& bytes)
            {
                for (auto byte : bytes)
                    set_byte_(location++, byte);
            }

            // Breakpoints

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

            std::optional<identifier_t> add_device(device_ptr dev_ptr, device_config cfg)
            {
                try {
                    return add_device_(dev_ptr, cfg);
                } catch (std::exception &e) {
                    return std::nullopt;
                }
            }

            status remove_device(identifier_t dev_id)
            {
                try {
                    remove_device_(dev_id);
                    return status::ok;
                } catch (std::exception &e) {
                    return status::invalid_identifier;
                }
            }

            // Runtime info

            // state_info
            // reg_info
            // mem_info
            // backtrace
            // frame_info (?)

        protected:
            // Key methods for a concrete implementation

            virtual address_t get_program_counter_() const = 0;
            virtual void set_program_counter_(address_t location) = 0;

            virtual status execute_single_() = 0;
            virtual void reset_() = 0;

            virtual byte_t get_byte_(address_t location) const = 0;
            virtual void set_byte_(address_t location, byte_t value) = 0;

            virtual identifier_t add_device_(device_ptr dev_ptr, device_config cfg) = 0;
            virtual void remove_device_(identifier_t dev_id) = 0;

            // Utility methods

            void step_()
            {
                auto status = execute_single_();

                if (is_error(status)) {
                    state_ = simulator_state::exception;
                } else {
                    auto pc = get_program_counter_();

                    if (is_breakpoint_(pc)) {
                        state_ = simulator_state::stopped;
                        clear_tmp_breakpoints_(pc);
                    }
                }
            }

            status run_()
            {
                do { step_(); } while (state_ == simulator_state::running);
                return state_ != simulator_state::exception ? status::ok : status::runtime_exception;
            }

            void terminate_()
            {
                state_ = simulator_state::terminated;
            }

            bool is_breakpoint_(address_t location) const { return breakpts_.count(location) || tmp_breakpts_.count(location); }
            void clear_tmp_breakpoints_(address_t location) { tmp_breakpts_.erase(location); }

            // Fields

            simulator_state state_ = simulator_state::initialized;  /* Internal simulator state */
            std::set<address_t> breakpts_;                          /* User-defined breakpoints */
            std::set<address_t> tmp_breakpts_;                      /* Temporary internal breakpoints */
            std::set<address_t> watches_;                           /* Memory location watches */
            // TODO: Annotations (ASM & C source lines)
            // TODO: Call frame info
    };

}

#endif  /* _UCLE_CORE_FNSIM_FNSIM_HPP_ */
