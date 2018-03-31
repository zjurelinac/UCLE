#include <fnsim/simulation.hpp>

#include <common/types.hpp>
#include <common/structures.hpp>

#include <fnsim/base.hpp>

#include <util/string.hpp>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

ucle::fnsim::status ucle::fnsim::functional_simulation::start(address_t start_location) noexcept {
    if (fnsim_->get_state() != simulator_state::loaded)
        return status::invalid_state;

    fnsim_->set_state(simulator_state::stopped);
    fnsim_->set_program_counter(start_location);

    return status::ok;
}

ucle::fnsim::status ucle::fnsim::functional_simulation::run(address_t start_location) noexcept {
    if (fnsim_->get_state() != simulator_state::loaded)
        return status::invalid_state;

    fnsim_->set_state(simulator_state::running);
    fnsim_->set_program_counter(start_location);

    return run_();
}

ucle::fnsim::status ucle::fnsim::functional_simulation::cont() noexcept {
     if (fnsim_->get_state() != simulator_state::stopped)
        return status::invalid_state;

    fnsim_->set_state(simulator_state::running);

    return run_();
}

ucle::fnsim::status ucle::fnsim::functional_simulation::step() noexcept {
    if (fnsim_->get_state() != simulator_state::stopped)
        return status::invalid_state;

    fnsim_->set_state(simulator_state::running);
    step_();

    return fnsim_->get_state() != simulator_state::exception ? status::ok : status::runtime_exception;
}

ucle::fnsim::status ucle::fnsim::functional_simulation::until(address_t location) noexcept {
     if (fnsim_->get_state() != simulator_state::stopped)
        return status::invalid_state;

    fnsim_->set_state(simulator_state::running);

    tmp_breakpts_.insert(location);
    return run_();
}

ucle::fnsim::status ucle::fnsim::functional_simulation::reset() noexcept {
    fnsim_->reset();
    fnsim_->set_state(simulator_state::initialized);
    return status::ok;
}

ucle::fnsim::status ucle::fnsim::functional_simulation::quit() noexcept {
    fnsim_->set_state(simulator_state::terminated);
    return status::ok;
}

ucle::fnsim::status ucle::fnsim::functional_simulation::load_pfile(std::string filename, address_t start_location) noexcept {
    constexpr int pf_line_bound = 21;

    std::ifstream pfile(filename);

    if (pfile.bad())
        return status::filesystem_error;

    try {
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
    } catch (std::exception& e) {
        fnsim_->set_state(simulator_state::exception);
        return status::invalid_program;
    }

    pfile.close();
    fnsim_->set_state(simulator_state::loaded);

    return status::ok;
}