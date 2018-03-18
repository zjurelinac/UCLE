#ifndef _UCLE_CORE_SIMULATORS_FUNCTIONAL_FUNCTIONAL_HPP_
#define _UCLE_CORE_SIMULATORS_FUNCTIONAL_FUNCTIONAL_HPP_

#include <unordered_map>
#include <set>
#include <string>

#include <common/types.hpp>
#include <simulators/functional/components.hpp>


namespace ucle {
namespace fnsim {

    // Defines all possible states of a functional simulator
    enum class simulator_state { initialized, loaded, running, stopped, exception, terminated };

    // Defines all possible functional simulator errors
    enum class simulator_error { memory_access_error, };

    // Abstract class which defines functional simulator interface
    class functional_simulator {
        public:
            virtual ~functional_simulator() {};

            // Basic functionality
            void run(address_t start_location = 0);
            void cont();
            void step();
            void until(address_t location);
            void reset();
            void quit();

            // Program loading
            void load_pfile(std::string filename, address_t start_location = 0);

            // Breakpoints
            void add_breakpoint(address_t breakpoint) { breakpts_.insert(breakpoint); }
            void remove_breakpoint(address_t breakpoint) { breakpts_.erase(breakpoint); }
            void clear_breakpoints() { breakpts_.clear(); };

            // Watches
            // TODO: Implement

            // Devices
            outcome<device_id, simulator_error> add_memory_mapped_device(device_ptr dev_ptr) { return add_mem_device_(dev_ptr); }
            outcome<device_id, simulator_error> add_port_mapped_device(device_ptr dev_ptr, address_range range) { return add_port_device_(dev_ptr); }
            outcome<success_t, simulator_error> remove_mem_device(device_id dev_id) { return remove_mem_device_(dev_id); }
            outcome<success_t, simulator_error> remove_port_device(device_id dev_id) { return remove_port_device_(dev_id); }

            // Runtime info
            // state_info
            // reg_info
            // mem_info
            // backtrace
            // frame_info (?)

        protected:
            // Key methods for a concrete implementation
            virtual void set_program_counter_(address_t location) = 0;
            virtual void execute_single_() = 0;
            virtual void reset_() = 0;

            virtual outcome<device_id, simulator_error> add_mem_device_(device_ptr dev_ptr) = 0;
            virtual outcome<device_id, simulator_error> add_port_device_(device_ptr dev_ptr) = 0;
            virtual outcome<success_t, simulator_error> remove_mem_device_(device_id dev_id) = 0;
            virtual outcome<success_t, simulator_error> remove_port_device_(device_id dev_id) = 0;

            // TODO: Rename
            // virtual void store_bytes_(byte_t* bytes, address_t location, size_t amount) = 0;

            // Utility methods
            bool is_breakpoint(address_t location) { return breakpts_.count(location) || tmp_breakpts_.count(location); }

            // Fields
            simulator_state state_ = simulator_state::initialized;
            std::set<address_t> breakpts_;          // User-defined breakpoints
            std::set<address_t> tmp_breakpts_;      // Temporary internal breakpoints
    };

    // Basic simulator config parameters
    struct simulator_config {
        size_t          memory_size;
        address_range   memory_addr_range = {0, 0xFFFFFFFF};
        endianness      memory_layout = endianness::LE;

        mapping_type    device_mapping = mapping_type::MEMORY;
        address_range   devices_addr_range = {0, 0};
    };

    // Base class for all functional simulator implementations
    template <typename RegisterFile, typename AddressSpace, typename Memory, typename Config = simulator_config>
    class functional_simulator_impl : public functional_simulator {
            using memptr_type = std::shared_ptr<Memory>;

        public:
            using config_type = Config;
            using regfile_type = RegisterFile;
            using address_space_type = AddressSpace;

            functional_simulator_impl(config_type cfg)
                : cfg_(cfg), mem_sp_(cfg.memory_addr_range), dev_sp_(cfg.devices_addr_range)
            {
                mem_ptr_ = std::make_shared(cfg.memory_size);
                mem_id_ = add_mem_device_(std::dynamic_pointer_cast<memory_mapped_device>(mem_ptr_), cfg.memory_addr_range);
            }

            virtual ~functional_simulator_impl() override { remove_mem_device_(mem_id_); }

        protected:
            // Key implementations methods - generic
            virtual void reset_() override { regs_.clear();  }

            // Key implementation methods - devices
            virtual outcome<device_id, simulator_error> add_mem_device_(device_ptr dev_ptr) override {
                // TODO: Implement
                return 1;
            }

            virtual outcome<device_id, simulator_error> add_port_device_(device_ptr dev_ptr) override {
                // TODO: Implement
                return 2;
            }

            virtual outcome<success_t, simulator_error> remove_mem_device_(device_id dev_id) override {
                mem_sp_.unregister_device(mem_devs_[dev_id]);
                mem_devs_.erase(dev_id);
                return success_t::SUCCESS;
            }

            virtual outcome<success_t, simulator_error> remove_port_device_(device_id dev_id) override {
                dev_sp_.unregister_device(port_devs_[dev_id]);
                port_devs_.erase(dev_id);
                return success_t::SUCCESS;
            }

            // Fields
            config_type cfg_;           // Simulator config parameters
            regfile_type regs_;         // Internal register file keeping the state of all registers and flags

            address_space_type mem_sp_;  // Memory address space
            address_space_type dev_sp_;  // Device address space (if devices are memory-mapped, then it's unused)

            memptr_type mem_ptr_;
            device_id mem_id_;

            std::unordered_map<device_id, device_ptr> mem_devs_;
            std::unordered_map<device_id, device_ptr> port_devs_;
        /*
            virtual void store_bytes_(byte_t* bytes, address_t location, size_t amount) override {}
        */
    };

}
}

#endif  /* _UCLE_CORE_SIMULATORS_FUNCTIONAL_FUNCTIONAL_HPP_ */
