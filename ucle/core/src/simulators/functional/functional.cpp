#include <simulators/functional/functional.hpp>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

using namespace ucle;


// Basic simulation functionality

status_t ucle::fnsim::functional_simulator::start(address_t start_location) {
    if (state_ != simulator_state::loaded) return error::invalid_state;

    state_ = simulator_state::stopped;
    set_program_counter_(start_location);

    return success::ok;
}

status_t ucle::fnsim::functional_simulator::run(address_t start_location) {
    if (state_ != simulator_state::loaded) return error::invalid_state;

    state_ = simulator_state::running;
    set_program_counter_(start_location);

    return run_();
}

status_t ucle::fnsim::functional_simulator::cont() {
     if (state_ != simulator_state::stopped) return error::invalid_state;

    state_ = simulator_state::running;

    return run_();
}

status_t ucle::fnsim::functional_simulator::step() {
    if (state_ != simulator_state::stopped) return error::invalid_state;

    state_ = simulator_state::running;
    step_();

    if (state_ == simulator_state::exception) return error::runtime_exception;
    else                                      return success::ok;
}

status_t ucle::fnsim::functional_simulator::until(address_t location) {
     if (state_ != simulator_state::stopped) return error::invalid_state;

    state_ = simulator_state::running;

    tmp_breakpts_.insert(location);
    return run_();
}

status_t ucle::fnsim::functional_simulator::reset() {
    reset_();
    state_ = simulator_state::initialized;
    return success::ok;
}

status_t ucle::fnsim::functional_simulator::quit() {
    state_ = simulator_state::terminated;
    return success::ok;
}

// Program loading

status_t ucle::fnsim::functional_simulator::load_pfile(std::string filename, address_t start_location) {
    std::ifstream pfile(filename);

    if (pfile.bad())
        return error::filesystem_error;

    while (!pfile.eof()) {
        std::string line;
        std::getline(pfile, line);

        std::istringstream iss(line);

        address_t address;
        iss >> std::hex >> address;

        address += start_location;
        std::cout << address << "\n";

        byte_t byte;
        while (iss >> (uint32_t&) byte) {
            printf("%08u %02hhX\n", address, byte);
            write_byte_(address++, byte);
        }
    }
    pfile.close();

    state_ = simulator_state::loaded;
    return success::ok;
}

// Internals

void ucle::fnsim::functional_simulator::step_() {
    auto status = execute_single_();

    if (is_error(status)) {
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
