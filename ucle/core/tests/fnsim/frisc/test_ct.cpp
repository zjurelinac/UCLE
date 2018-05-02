#include <fnsim/device.hpp>
#include <fnsim/simulation.hpp>
#include <fnsim/processors/frisc.hpp>
#include <fnsim/devices/frisc_ct.hpp>

#include <memory>

using namespace ucle;
using namespace ucle::fnsim;

int main(int, char* argv[])
{

    functional_simulation<false, false, false, true> sim { frisc::make_frisc_simulator({ 4096 }) };

    auto ct_dev = std::make_shared<frisc::counter_timer>();
    sim.add_device(ct_dev, { 0x10000, 16, device_class::addressable_device, true, frisc::frisc_int });

    sim.load_pfile(argv[1]);
    auto stat = sim.run();
    fmt::print("Result: {}\n", to_string(stat));

    print_reg_info(sim.get_reg_info());
}
