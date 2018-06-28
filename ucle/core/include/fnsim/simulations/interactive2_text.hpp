#pragma once

#include <fnsim/base.hpp>

#include <fnsim/simulations/interactive.hpp>

#include <libs/fmt/format.h>

#include <algorithm>
#include <iostream>

namespace ucle::fnsim::cli {

    template <typename FunctionalSimulation>
    class interactive_text_simulation : public interactive_simulation<FunctionalSimulation> {
        using interactive_simulation<FunctionalSimulation>::interactive_simulation;
    }
}
