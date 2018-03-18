#include <simulators/functional/frisc.hpp>

void ucle::fnsim::frisc_simulator::execute_single_() {

}

/* Test */
int main() {
    ucle::fnsim::simulator_config cfg{.memory_size = 0x1000};
    ucle::fnsim::frisc_simulator fs(cfg);
}
