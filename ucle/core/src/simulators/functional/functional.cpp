#include <simulators/functional/functional.hpp>

#include <cstdio>
#include <fstream>
#include <string>

using namespace ucle;

//******************************************************************************
//  functional_simulator
//******************************************************************************

// Basic simulation functionality
status_t ucle::fnsim::functional_simulator::start(address_t start_location) {
    if (state_ != simulator_state::loaded)
        return error::invalid_state;

    state_ = simulator_state::stopped;
    set_program_counter_(start_location);
    return success::ok;
}

status_t ucle::fnsim::functional_simulator::run(address_t start_location) {
    if (state_ != simulator_state::loaded)
        return error::invalid_state;

    state_ = simulator_state::running;
    set_program_counter_(start_location);

    return run_();
}

status_t ucle::fnsim::functional_simulator::cont() {
     if (state_ != simulator_state::stopped)
        return error::invalid_state;

    state_ = simulator_state::running;

    return run_();
}

status_t ucle::fnsim::functional_simulator::step() {

}

/*status_t ucle::fnsim::functional_simulator::until(address_t location) {

}

status_t ucle::fnsim::functional_simulator::reset() {

}

status_t ucle::fnsim::functional_simulator::quit() {

}

// Program loading
status_t load_pfile(std::string filename, address_t start_location = 0);

// Breakpoints
status_t add_breakpoint(address_t breakpoint); //  { breakpts_.insert(breakpoint); }
status_t remove_breakpoint(address_t breakpoint); // { breakpts_.erase(breakpoint); }
status_t clear_breakpoints();  // { breakpts_.clear(); };

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
}*/

void ucle::fnsim::functional_simulator::step_() {
    auto status = execute_single_();

    if (!is_successful(status)) {
        state_ = simulator_state::exception;
    } else {
        auto pc = get_program_counter_();

        if (is_breakpoint_(pc)) {
            state_ = simulator_state::stopped;
            clear_tmp_breakpoints_(pc);
        }
    }
}

status_t ucle::fnsim::functional_simulator::run_() {
    do { step_(); } while (state_ == simulator_state::running);

    if (state_ == simulator_state::exception)
        return error::runtime_exception;
    else
        return success::ok;
}


//******************************************************************************
//  functional_simulator_impl
//******************************************************************************

#include <iostream>

using namespace ucle;

/* Test */

/*int main() {
}*/