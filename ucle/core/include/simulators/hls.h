#ifndef _CORE_HLS_H_
#define _CORE_HLS_H_

#include <map>
#include <string>

#include <common/types.h>

namespace ucle {
    namespace hls {

        enum class simulator_state { initialized, loaded, running, stopped, exception, terminated };

        struct simulator_config {
            address_t memory_size;
        };

        struct device_config {
            address_t address_space_size;
        };

        class addressable_device {
            public:
            addressable_device(std::string name, device_config config) : name_(name), config_(config) {}

            template <typename T> T     get(address_t location) { assert("Not implemented!"); };
            template <typename T> void  set(address_t location, T value) { assert("Not implemented!"); };
            private:
            std::string name_;
            device_config config_;
        };

        template <> word_t addressable_device::get<word_t>(address_t location);
        template <> half_t addressable_device::get<half_t>(address_t location);
        template <> byte_t addressable_device::get<byte_t>(address_t location);

        template <> void addressable_device::set<word_t>(address_t location, word_t value);
        template <> void addressable_device::set<half_t>(address_t location, half_t value);
        template <> void addressable_device::set<byte_t>(address_t location, byte_t value);


        class address_space {
            public:
            void add_device(addressable_device device, address_range range);
            void remove_device(addressable_device device);

            template <typename T> T get(address_t location);

            private:
            std::map<address_range, addressable_device> devices_;
        };

        class memory : addressable_device {
            public:

        };

        /*template <typename State, typename Config>
        class high_level_simulator {
            public:
            high_level_simulator(Config config) : config_(config) {}
            virtual ~high_level_simulator();

            // Basic functionalities
            void run();
            void step();
            virtual void reset();

            // Program loading
            void load_pfile(std::string filename, address_t start_address = 0);

            // Breakpoints
            // void add_breakpoint( bpt );
            // void remove_breakpoint( bpt );
            // void clear_breakpoints();

            // Devices
            // add_device(dev, adr_range)
            // remove_device(dev)

            // Runtime info
            // state_info
            // reg_info
            // mem_info
            // backtrace
            // frame_info (?)

            protected:
            // Utils
            void execute_single();
            void process_interrupts();

            simulator_state state_;
            address_space adr_sp_;

            Config config_;
            State state_;
        };*/

    }

}
#endif  // _CORE_HLS_H_
