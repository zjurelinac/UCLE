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

    frisc::ct_chainer chainer;

    auto ct_dev_1 = std::make_shared<frisc::counter_timer>(nullptr, [&] { chainer.zc_notify(); });
    sim.add_device(ct_dev_1, { 0x10000, 16, device_class::addressable_device });

    auto ct_dev_2 = std::make_shared<frisc::counter_timer>([&] { return chainer.should_tick(); });
    sim.add_device(ct_dev_2, { 0x20000, 16, device_class::addressable_device });

    sim.load_pfile(argv[1]);
    auto stat = sim.run();
    fmt::print("Result: {}\n", to_string(stat));

    print_reg_info(sim.get_reg_info());
}
