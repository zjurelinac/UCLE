#include <simulators/functional/functional.hpp>

#include <cstdio>
#include <fstream>
#include <string>


//******************************************************************************
//  functional_simulator
//******************************************************************************

void ucle::fnsim::functional_simulator::run(address_t start_location) {
    if (state_ != simulator_state::loaded && state_ != simulator_state::stopped) return;

    set_program_counter_(start_location);
    state_ = simulator_state::running;

    do { execute_single_(); } while (state_ == simulator_state::running);
}

void ucle::fnsim::functional_simulator::step() {
    execute_single_();
}

void ucle::fnsim::functional_simulator::load_pfile(std::string filename, address_t start_location) {
    address_t address;
    byte_t parts[4] = {0};

    std::ifstream pfile(filename);
    while (!pfile.eof()) {
        std::string line; std::getline(pfile, line);

        if (unsigned items = sscanf(line.c_str(), "%8X %2hhX %2hhX %2hhX %2hhX", &address, &parts[0], &parts[1], &parts[2], &parts[3]); items > 5) {
            // TODO: store_bytes_(parts, start_location + address, items - 1);
            // printf("%08zX %08X\n", address, word_from_bytes__le(word));
            // adr_sp_.set<word>(address + start_location, word_from_bytes__le(word));
        }
    }
    pfile.close();

    state_ = simulator_state::loaded;
}


//******************************************************************************
//  functional_simulator_impl
//******************************************************************************

#include <iostream>

using namespace ucle;

/* Test */

/*int main() {
}*/