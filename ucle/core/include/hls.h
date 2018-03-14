#ifndef _CORE_HLS_H_
#define _CORE_HLS_H_

#include <util/base.h>

namespace ucle {
    namespace hls {

        enum class simulator_state { initialized, loaded, running, stopped, exception, terminated };

        struct simulator_config {
            int memory_size;
        };


        class address_space {

        };


        class addressable_device {

        };


        class memory : addressable_device {

        };


        class high_level_simulator {
        public:
            high_level_simulator(simulator_config);
            virtual ~high_level_simulator();

            // Basic functionalities
            virtual void load(/* params */);
            virtual void run();
            virtual void step();
            virtual void reset();

            // Breakpoints
            void add_breakpoint(/* bpt */);
            void remove_breakpoint(/* bpt */);
            void clear_breakpoints();

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

            simulator_state _state;
            address_space _adr_sp;
        };

    }

}
#endif  // _CORE_HLS_H_
