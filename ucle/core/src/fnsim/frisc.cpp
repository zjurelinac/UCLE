#include <fnsim/frisc.hpp>

ucle::status_t ucle::fnsim::frisc_simulator::execute_single_() {
    return success::ok;
}

/* Test */
int main(int, char* argv[]) {
    ucle::fnsim::simulator_config cfg{.memory_size = 0x1000};
    ucle::fnsim::frisc_simulator fs(cfg);
    fs.load_pfile(argv[1]);
    // fs.run();
}
