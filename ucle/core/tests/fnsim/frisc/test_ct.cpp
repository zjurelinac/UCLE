#include <fnsim/device.hpp>
#include <fnsim/simulation.hpp>
#include <fnsim/processors/frisc.hpp>
#include <fnsim/devices/frisc_ct.hpp>

#include <memory>

using namespace ucle;
using namespace ucle::fnsim;

int main(int, char* argv[])
{
    using namespace ucle::literals;

    functional_simulation<false, false, false, true> sim { frisc::make_frisc_simulator({ 4096 }) };

    frisc::approximate_freq_ticker ticker { 1_MHz, 35_MHz };
    frisc::ct_chainer chainer;

    auto ct_dev_1 = std::make_shared<frisc::counter_timer>(&ticker, &chainer);
    sim.add_device(ct_dev_1, { 0x10000, frisc::counter_timer::address_space_size, device_class::addressable_device });

    auto ct_dev_2 = std::make_shared<frisc::counter_timer>(&chainer);
    sim.add_device(ct_dev_2, { 0x20000, frisc::counter_timer::address_space_size, device_class::addressable_device });

    sim.load_pfile(argv[1]);
    auto stat = sim.run();
    fmt::print("Result: {}\n", to_string(stat));

    print_reg_info(sim.get_reg_info());
    print_exec_info(sim.get_exec_info());
}
