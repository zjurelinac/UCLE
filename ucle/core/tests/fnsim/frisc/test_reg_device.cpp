#include <fnsim/device.hpp>
#include <fnsim/simulation.hpp>
#include <fnsim/processors/frisc.hpp>

#include <memory>

using namespace ucle::fnsim;

class dummy_reg_device : public register_set_device<2, 32> {
    void work() override {}
    void status() override {}
};

int main(int, char* argv[])
{

    functional_simulation<false, false, false, true> sim { make_frisc_simulator({ 4096 }) };

    sim.add_device(std::make_unique<dummy_reg_device>(), { 0x10000, 8, device_class::addressable_device });

    sim.load_pfile(argv[1]);
    auto stat = sim.run();
    fmt::print("Result: {}\n", to_string(stat));

    print_reg_info(sim.get_reg_info());
}
