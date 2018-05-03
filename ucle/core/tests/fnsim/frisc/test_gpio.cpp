#include <fnsim/device.hpp>
#include <fnsim/simulation.hpp>
#include <fnsim/processors/frisc.hpp>
#include <fnsim/devices/frisc_gpio.hpp>

#include <memory>

using namespace ucle;
using namespace ucle::fnsim;

class led_device : public frisc::base_io_device {
    public:
        void write(byte_t value) override
        {
            bitfield<8> b = value;
            for (int i = 7; i >= 0; --i)
                if (b[i])
                    fmt::print_colored(fmt::YELLOW, "o");
                else
                    fmt::print_colored(fmt::BLACK, ".");
            fmt::print("\n");
        }
};

int main(int, char* argv[])
{
    using namespace ucle::literals;

    functional_simulation<false, false, false, true> sim { frisc::make_frisc_simulator({ 4096 }) };

    led_device led;

    auto gpio_dev_1 = std::make_shared<frisc::gpio>(&led);
    sim.add_device(gpio_dev_1, { 0x10000, 16, device_class::addressable_device });

    sim.load_pfile(argv[1]);
    auto stat = sim.run();
    fmt::print("Result: {}\n", to_string(stat));

    print_reg_info(sim.get_reg_info());
}
