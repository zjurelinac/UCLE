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

class switch_device : public frisc::base_io_device {
    public:
        byte_t read() override {
            if (cnt_++ == 12) {
                cnt_ = 0;
                ++value_;
            }

            return value_;
        }

    private:
        byte_t value_ { 0 };
        int cnt_ { 0 };
};

class uart_rx_device : public frisc::base_io_device {
    public:
        bool ready() override {
            if (cnt_++ != 50)
                return false;

            cnt_ = 0;
            ++value_;
            return true;
        }

        byte_t read() override {
            fmt::print_colored(fmt::BLUE, "Receiving from UART: <{}> [0x{:02X}]\n", static_cast<char>(value_), value_);
            return value_;
        }

    private:
        byte_t value_ { 0 };
        int cnt_ { 0 };
};

class uart_tx_device : public frisc::base_io_device {
    public:
        bool ready() override {
            if (cnt_++ != 50)
                return false;

            cnt_ = 0;
            return true;
        }

        void write(byte_t value) override {
            fmt::print_colored(fmt::BLUE, "Sending to UART: {} [0x{:02X}]\n", static_cast<char>(value), value);
        }

    private:
        int cnt_ { 0 };
};

int main(int, char* argv[])
{
    using namespace ucle::literals;

    functional_simulation<false, false, false, true> sim { frisc::make_frisc_simulator({ 4096 }) };

    led_device led;
    switch_device sw;
    uart_rx_device uart_rx;
    uart_tx_device uart_tx;

    auto gpio_dev_1 = std::make_shared<frisc::gpio>(&led);
    sim.add_device(gpio_dev_1, { 0x10000, frisc::gpio::address_space_size, device_class::addressable_device });

    auto gpio_dev_2 = std::make_shared<frisc::gpio>(&sw);
    sim.add_device(gpio_dev_2, { 0x20000, frisc::gpio::address_space_size, device_class::addressable_device });

    auto gpio_dev_3 = std::make_shared<frisc::gpio>(&uart_rx);
    sim.add_device(gpio_dev_3, { 0x30000, frisc::gpio::address_space_size, device_class::addressable_device });

    auto gpio_dev_4 = std::make_shared<frisc::gpio>(&uart_tx);
    sim.add_device(gpio_dev_4, { 0x40000, frisc::gpio::address_space_size, device_class::addressable_device });

    sim.load_pfile(argv[1]);
    auto stat = sim.run();
    fmt::print("Result: {}\n", to_string(stat));

    print_reg_info(sim.get_reg_info());
    print_exec_info(sim.get_exec_info());
}
