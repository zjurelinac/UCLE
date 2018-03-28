#include <fnsim/fnsim.hpp>

#include <common/types.hpp>
#include <common/structures.hpp>

#include <util/string.hpp>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

ucle::status ucle::fnsim::functional_simulator::start(address_t start_location) {
    if (state_ != simulator_state::loaded)
        return status::invalid_state;

    state_ = simulator_state::stopped;
    set_program_counter_(start_location);

    return status::ok;
}

ucle::status ucle::fnsim::functional_simulator::run(address_t start_location) {
    if (state_ != simulator_state::loaded)
        return status::invalid_state;

    state_ = simulator_state::running;
    set_program_counter_(start_location);

    return run_();
}

ucle::status ucle::fnsim::functional_simulator::cont() {
     if (state_ != simulator_state::stopped)
        return status::invalid_state;

    state_ = simulator_state::running;

    return run_();
}

ucle::status ucle::fnsim::functional_simulator::step() {
    if (state_ != simulator_state::stopped)
        return status::invalid_state;

    state_ = simulator_state::running;
    step_();

    return state_ != simulator_state::exception ? status::ok : status::runtime_exception;
}

ucle::status ucle::fnsim::functional_simulator::until(address_t location) {
     if (state_ != simulator_state::stopped)
        return status::invalid_state;

    state_ = simulator_state::running;

    tmp_breakpts_.insert(location);
    return run_();
}

ucle::status ucle::fnsim::functional_simulator::reset() {
    reset_();
    state_ = simulator_state::initialized;
    return status::ok;
}

ucle::status ucle::fnsim::functional_simulator::quit() {
    state_ = simulator_state::terminated;
    return status::ok;
}

ucle::status ucle::fnsim::functional_simulator::load_pfile(std::string filename, address_t start_location) {
    constexpr int pf_line_bound = 21;

    std::ifstream pfile(filename);

    if (pfile.bad())
        return status::filesystem_error;

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

        small_byte_vector bytes;
        while (iss >> byte)
            bytes.push_back(static_cast<byte_t>(byte));

        set_memory_contents(address, bytes);
    }

    pfile.close();
    state_ = simulator_state::loaded;

    return status::ok;
}
