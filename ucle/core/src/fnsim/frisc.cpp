#include <fnsim/frisc.hpp>

#include <iostream>

ucle::status_t ucle::fnsim::frisc_simulator::execute_single_() {
    if (regs_.PC > 20)
        return error::invalid_state;

    regs_.IR = read_word(regs_.PC);
    std::cout << regs_.PC.get() << ":" << regs_.IR[{31, 27}] << "\n";

    regs_.PC += 4;

    return success::ok;
}

/* Test */
int main(int, char* argv[]) {
    ucle::fnsim::simulator_config cfg{.memory_size = 0x1000};
    ucle::fnsim::frisc_simulator fs(cfg);
    fs.load_pfile(argv[1]);
    fs.run();
}
