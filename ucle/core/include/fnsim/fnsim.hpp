#ifndef _UCLE_CORE_FNSIM_FNSIM_HPP_
#define _UCLE_CORE_FNSIM_FNSIM_HPP_

#include <memory>
#include <set>

#include <common/exceptions.hpp>
#include <common/types.hpp>

namespace ucle::fnsim {

    enum class endianness { LE, BE };
    enum class mapping_type { DEFAULT, MEMORY, PORT, NONE };

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
        mapping_type    device_mapping = mapping_type::DEFAULT;
        address_range   addr_range = {0, 0};
        priority_t      interrupt_priority;
    };

    enum class simulator_state { initialized, loaded, running, stopped, exception, terminated };

    class functional_simulator {
        public:
            virtual ~functional_simulator() {};

            // Basic simulation functionality

            status_t start(address_t start_location = 0);
            status_t run(address_t start_location = 0);
            status_t cont();
            status_t step();
            status_t until(address_t location);
            status_t reset();
            status_t quit();

            // Program loading

            status_t load_pfile(std::string filename, address_t start_location = 0);   // TODO: Start location? What if code not PIC?

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

            std::variant<identifier_t, error> add_device(device_ptr dev_ptr, device_config cfg)
            {
                try {
                    return add_device_(dev_ptr, cfg);
                } catch (std::exception &e) {
                    return error::invalid_address_range;
                }
            }

            status_t remove_device(identifier_t dev_id)
            {
                try {
                    remove_device_(dev_id);
                    return success::ok;
                } catch (std::exception &e) {
                    return error::invalid_identifier;
                }
            }

            byte_t read_byte(address_t location) { return read_byte_(location); }
            half_t read_half(address_t location) { return read_half_(location); }
            word_t read_word(address_t location) { return read_word_(location); }

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

            virtual status_t execute_single_() = 0;
            virtual void reset_() = 0;

            virtual identifier_t add_device_(device_ptr dev_ptr, device_config cfg) = 0;
            virtual void remove_device_(identifier_t dev_id) = 0;

            virtual byte_t read_byte_(address_t location) = 0;
            virtual half_t read_half_(address_t location) = 0;
            virtual word_t read_word_(address_t location) = 0;

            virtual void write_byte_(address_t location, byte_t value) = 0;
            virtual void write_half_(address_t location, half_t value) = 0;
            virtual void write_word_(address_t location, word_t value) = 0;

            // Utility methods

            void step_();
            status_t run_();

            bool is_breakpoint_(address_t location) const { return breakpts_.count(location) || tmp_breakpts_.count(location); }
            void clear_tmp_breakpoints_(address_t location) { tmp_breakpts_.erase(location); }

            // Fields

            simulator_state state_ = simulator_state::initialized;  /* Internal simulator state */
            std::set<address_t> breakpts_;                          /* User-defined breakpoints */
            std::set<address_t> tmp_breakpts_;                      /* Temporary internal breakpoints */
            std::set<address_t> watches_;                           /* Memory location watches */
            // ...                                                  /* Call frame info */
    };

}

#endif  /* _UCLE_CORE_FNSIM_FNSIM_HPP_ */
