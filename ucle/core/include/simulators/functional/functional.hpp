#ifndef _UCLE_CORE_SIMULATORS_FUNCTIONAL_FUNCTIONAL_HPP_
#define _UCLE_CORE_SIMULATORS_FUNCTIONAL_FUNCTIONAL_HPP_

#include <unordered_map>
#include <set>
#include <string>

#include <common/types.hpp>
#include <simulators/functional/components.hpp>


namespace ucle::fnsim {

    // Defines all possible states of a functional simulator
    enum class simulator_state { initialized, loaded, running, stopped, exception, terminated };

    // Abstract class which defines functional simulator interface
    class functional_simulator {
        public:
            virtual ~functional_simulator() {};

            // Basic simulation functionality
            status_t start(address_t start_location = 0);
            status_t run(address_t start_location = 0);
            status_t cont();
            status_t step();
            status_t until(address_t location);
            status_t reset();
            status_t quit();

            // Program loading
            status_t load_pfile(std::string filename, address_t start_location = 0);

            // Breakpoints
            status_t add_breakpoint(address_t breakpoint); //  { breakpts_.insert(breakpoint); }
            status_t remove_breakpoint(address_t breakpoint); // { breakpts_.erase(breakpoint); }
            status_t clear_breakpoints();  // { breakpts_.clear(); };

            // Watches
            // TODO: Implement

            // Devices
            outcome<device_id, error> add_memory_mapped_device(device_ptr dev_ptr, address_range range) { return add_mem_device_(dev_ptr, range); }
            outcome<device_id, error> add_port_mapped_device(device_ptr dev_ptr, address_range range) { return add_port_device_(dev_ptr, range); }
            status_t remove_mem_device(device_id dev_id) { return remove_mem_device_(dev_id); }
            status_t remove_port_device(device_id dev_id) { return remove_port_device_(dev_id); }

            // Runtime info
            // state_info
            // reg_info
            // mem_info
            // backtrace
            // frame_info (?)

        protected:
            // Key methods for a concrete implementation
            virtual address_t get_program_counter_() = 0;
            virtual void set_program_counter_(address_t location) = 0;
            virtual status_t execute_single_() = 0;
            virtual void reset_() = 0;

            virtual outcome<device_id, error> add_mem_device_(device_ptr dev_ptr, address_range range) = 0;
            virtual outcome<device_id, error> add_port_device_(device_ptr dev_ptr, address_range range) = 0;
            virtual status_t remove_mem_device_(device_id dev_id) = 0;
            virtual status_t remove_port_device_(device_id dev_id) = 0;

            // TODO: Rename
            // virtual void store_bytes_(byte_t* bytes, address_t location, size_t amount) = 0;

            // Utility methods
            void step_();
            status_t run_();

            bool is_breakpoint_(address_t location) { return breakpts_.count(location) || tmp_breakpts_.count(location); }
            void clear_tmp_breakpoints_(address_t location) { tmp_breakpts_.erase(location); }

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
            using regfile_type = RegisterFile;
            using address_space_type = AddressSpace;
            using memory_type = Memory;
            using config_type = Config;

            functional_simulator_impl(config_type cfg)
                    : cfg_(cfg), mem_sp_(cfg.memory_addr_range), dev_sp_(cfg.devices_addr_range) {
                mem_ptr_ = std::make_shared(cfg.memory_size);
                mem_id_ = add_mem_device_(std::dynamic_pointer_cast<memory_mapped_device>(mem_ptr_), cfg.memory_addr_range);
            }

            virtual ~functional_simulator_impl() override { remove_mem_device_(mem_id_); }

        protected:
            // Key implementations methods - generic
            virtual void reset_() override {
                regs_.clear();
                for (const auto& [_, dev_ptr]: mem_devs_) dev_ptr->reset();
                for (const auto& [_, dev_ptr]: port_devs_) dev_ptr->reset();
            }

            // Key implementation methods - devices
            virtual outcome<device_id, error> add_mem_device_(device_ptr dev_ptr, address_range range) override {
                auto res = mem_sp_.register_device(dev_ptr, range);
                if (!is_successful(res)) return get_error(res);

                device_id dev_id = mem_sp_.size() + 1;
                mem_sp_[dev_id] = dev_ptr;

                return dev_id;
            }

            virtual outcome<device_id, error> add_port_device_(device_ptr dev_ptr, address_range range) override {
                auto res = dev_sp_.register_device(dev_ptr, range);
                if (!is_successful(res)) return get_error(res);

                device_id dev_id = dev_sp_.size() + 1;
                dev_sp_[dev_id] = dev_ptr;

                return dev_id;
            }

            virtual status_t remove_mem_device_(device_id dev_id) override {
                mem_sp_.unregister_device(mem_devs_[dev_id]);
                mem_devs_.erase(dev_id);
                return success::ok;
            }

            virtual status_t remove_port_device_(device_id dev_id) override {
                dev_sp_.unregister_device(port_devs_[dev_id]);
                port_devs_.erase(dev_id);
                return success::ok;
            }

            // Fields
            config_type cfg_;           // Simulator config parameters
            regfile_type regs_;         // Internal register file keeping the state of all registers and flags

            address_space_type mem_sp_; // Memory address space
            address_space_type dev_sp_; // Device address space (if devices are memory-mapped, then it's unused)

            memptr_type mem_ptr_;       // Internal memory device pointer
            device_id mem_id_;          // Internal memory device ID

            std::unordered_map<device_id, device_ptr> mem_devs_;
            std::unordered_map<device_id, device_ptr> port_devs_;
        /*
            virtual void store_bytes_(byte_t* bytes, address_t location, size_t amount) override {}
        */
    };

}

#endif  /* _UCLE_CORE_SIMULATORS_FUNCTIONAL_FUNCTIONAL_HPP_ */
