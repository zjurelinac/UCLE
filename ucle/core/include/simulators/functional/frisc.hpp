#include <simulators/functional/components.hpp>
#include <simulators/functional/functional.hpp>

namespace ucle {
namespace fnsim {

    struct frisc_register_file : register_file {
        reg<32> R[8];
        reg<32>& SP = R[7];

        reg<32> PC;
        reg<32> SR;  // TODO: A flags register!

        void clear() override { PC = 0; SR = 0; R = {0}; }
    };

    class frisc_simulator : public functional_simulator_impl<frisc_register_file, address_space<>, memory> {
        using functional_simulator_impl<frisc_register_file, address_space<>, memory>::functional_simulator_impl;

        protected:
            virtual void set_program_counter_(address_t location) override { registers_.PC = location; };
            virtual void execute_single_() override;

    };

}
}