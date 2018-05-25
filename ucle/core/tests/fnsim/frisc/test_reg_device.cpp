#include <fnsim/device.hpp>
#include <fnsim/simulation.hpp>
#include <fnsim/processors/frisc.hpp>

#include <memory>

using namespace ucle;
using namespace ucle::fnsim;

class dummy_reg_device : public register_set_device<2, 32, byte_order::little_endian, address32_t> {
    public:
        void work() override
        {
            static int i = 0;
            fmt::print_colored(fmt::BLUE, ".");
            write_word_(0, ++i);
        }

        device_status status() override
        {
            return device_status::pending;
        }

        bool is_worker() const override { return true; }

    private:

        counter_t cnt_ {0};
};

int main(int, char* argv[])
{

    functional_simulation<false, false, false, true> sim { frisc::make_frisc_simulator({ 4096 }) };

    auto dummy_dev = std::make_shared<dummy_reg_device>();
    sim.add_device(dummy_dev, { 0x10000, 8, device_class::addressable_device });

    sim.load_pfile(argv[1]);
    auto stat = sim.run();
    fmt::print("Result: {}\n", to_string(stat));

    print_reg_info(sim.get_reg_info());
}
