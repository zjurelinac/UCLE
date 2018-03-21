#include <simulators/functional/frisc.hpp>

ucle::status_t ucle::fnsim::frisc_simulator::execute_single_() {
    return success::ok;
}

/* Test */
int main(int argc, char* argv[]) {
    ucle::fnsim::simulator_config cfg{.memory_size = 0x1000};
    ucle::fnsim::frisc_simulator fs(cfg);
    fs.load_pfile(argv[1]);
    // fs.run();
}
