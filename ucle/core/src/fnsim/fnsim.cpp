#include <fnsim/fnsim.hpp>
#include <util/string.hpp>

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
    constexpr int pf_line_bound = 21;

    std::ifstream pfile(filename);

    if (pfile.bad())
        return error::filesystem_error;

    while (!pfile.eof()) {
        std::string line;
        std::getline(pfile, line);

        std::string code = util::trim_copy(line.substr(0, pf_line_bound));
        std::string annotation = line.size() > pf_line_bound ? util::trim_copy(line.substr(pf_line_bound)) : "";

        if (code.size() == 0) continue;

        std::istringstream iss(code);
        address_t address;
        unsigned byte;

        iss >> std::hex >> address;
        address += start_location;

        // Store the annotation

        while (iss >> byte) {
            // printf("%08X %02X\n", address, byte);
            write_byte_(address++, byte);
        }
    }

    pfile.close();
    state_ = simulator_state::loaded;

    return success::ok;
}
