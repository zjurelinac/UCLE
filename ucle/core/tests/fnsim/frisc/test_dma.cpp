#include <fnsim/device.hpp>
#include <fnsim/simulation.hpp>
#include <fnsim/processors/frisc.hpp>
#include <fnsim/devices/frisc_dma.hpp>

#include <memory>
#include <iostream>
#include <typeinfo>

using namespace ucle;
using namespace ucle::fnsim;

int main(int, char* argv[])
{
    using namespace ucle::literals;

    auto proc = frisc::make_frisc_simulator({ 4096 });
    auto proc_ptr = proc.get();
    functional_simulation<false, false, false, true> sim { std::move(proc) };

    using dma_type = frisc::dma<decltype(proc_ptr)>;

    auto dma_dev = std::make_shared<dma_type>(proc_ptr);
    sim.add_device(dma_dev, { 0x10000, dma_type::address_space_size, device_class::addressable_device });

    sim.load_pfile(argv[1]);
    auto stat = sim.run();
    fmt::print("Result: {}\n", to_string(stat));

    print_reg_info(sim.get_reg_info());
    print_exec_info(sim.get_exec_info());
}
