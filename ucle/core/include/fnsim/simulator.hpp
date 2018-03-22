#ifndef _UCLE_CORE_SIMULATORS_FUNCTIONAL_FUNCTIONAL_HPP_
#define _UCLE_CORE_SIMULATORS_FUNCTIONAL_FUNCTIONAL_HPP_

#include <unordered_map>
#include <set>
#include <string>

#include <common/types.hpp>
#include <fnsim/components.hpp>


namespace ucle::fnsim {

    enum class simulator_state { initialized, loaded, running, stopped, exception, terminated };

    // Config parameter structs

    struct simulator_config {
        size_t          memory_size;
        address_range   memory_addr_range = {0, 0xFFFFFFFF};
        endianness      memory_layout = endianness::LE;
        mapping_type    device_mapping = mapping_type::MEMORY;
        address_range   devices_addr_range = {0, 0};
    };

    struct device_config {
        bool            is_addressable = true;
        bool            uses_interrupts = false;
        mapping_type    device_mapping = mapping_type::DEFAULT;
        address_range   addr_range = {0, 0};
        priority_t      interrupt_priority;
    };

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

            status_t load_pfile(std::string filename, address_t start_location = 0);   // TODO: Start location? What if code not PIC?

            // Breakpoints

            void add_breakpoint(address_t breakpoint)
            {
                breakpts_.insert(breakpoint);
            }

            void remove_breakpoint(address_t breakpoint)
            {
                breakpts_.erase(breakpoint);
            }

            void clear_breakpoints()
            {
                breakpts_.clear();
            }

            void clear_all_breakpoints()
            {
                breakpts_.clear();
                tmp_breakpts_.clear();
            }

            // Watches

            void add_watch(address_t location)
            {
                watches_.insert(location);
            }

            void remove_watch(address_t location)
            {
                watches_.erase(location);
            }

            void clear_watches()
            {
                watches_.clear();
            }

            // Devices

            std::variant<identifier_t, error> add_device(device_ptr dev_ptr, device_config cfg)
            {
                try {
                    return add_device_(dev_ptr, cfg);
                } catch (std::exception &e) {
                    return error::invalid_address_range;
                }
            }

            status_t remove_device(identifier_t dev_id)
            {
                try {
                    remove_device_(dev_id);
                    return success::ok;
                } catch (std::exception &e) {
                    return error::invalid_identifier;
                }
            }

            byte_t read_byte(address_t location) { return read_byte_(location); }
            half_t read_half(address_t location) { return read_half_(location); }
            word_t read_word(address_t location) { return read_word_(location); }

            // Runtime info

            // state_info
            // reg_info
            // mem_info
            // backtrace
            // frame_info (?)

        protected:
            // Key methods for a concrete implementation

            virtual address_t get_program_counter_() const = 0;
            virtual void set_program_counter_(address_t location) = 0;

            virtual status_t execute_single_() = 0;
            virtual void reset_() = 0;

            virtual identifier_t add_device_(device_ptr dev_ptr, device_config cfg) = 0;
            virtual void remove_device_(identifier_t dev_id) = 0;

            virtual byte_t read_byte_(address_t location) = 0;
            virtual half_t read_half_(address_t location) = 0;
            virtual word_t read_word_(address_t location) = 0;

            virtual void write_byte_(address_t location, byte_t value) = 0;
            virtual void write_half_(address_t location, half_t value) = 0;
            virtual void write_word_(address_t location, word_t value) = 0;

            // Utility methods

            void step_();
            status_t run_();

            bool is_breakpoint_(address_t location) const { return breakpts_.count(location) || tmp_breakpts_.count(location); }
            void clear_tmp_breakpoints_(address_t location) { tmp_breakpts_.erase(location); }

            // Fields

            simulator_state state_ = simulator_state::initialized;  /* Internal simulator state */
            std::set<address_t> breakpts_;                          /* User-defined breakpoints */
            std::set<address_t> tmp_breakpts_;                      /* Temporary internal breakpoints */
            std::set<address_t> watches_;                           /* Memory location watches */
            // ...                                                  /* Call frame info */
    };

    template <typename RegisterFile, typename AddressSpace,
              template<endianness> typename Memory, typename Config = simulator_config>
    class functional_simulator_impl : public functional_simulator {
            using memptr_type = std::shared_ptr<memory_mapped_device>;

            struct device_info {
                identifier_t    id;
                device_ptr      ptr;
                mapping_type    mapping;
            };

        public:
            template <endianness E>
            using memory_type = Memory<E>;
            using regfile_type = RegisterFile;
            using address_space_type = AddressSpace;
            using config_type = Config;

            functional_simulator_impl(config_type cfg)
                : cfg_(cfg), mem_asp_(cfg.memory_addr_range), dev_asp_(cfg.devices_addr_range)
            {
                if (cfg.memory_layout == endianness::LE)
                    mem_ptr_ = std::make_shared<memory_type<endianness::LE>>(cfg.memory_size);
                else
                    mem_ptr_ = std::make_shared<memory_type<endianness::BE>>(cfg.memory_size);
                device_config mem_cfg { true, false, mapping_type::MEMORY, cfg.memory_addr_range, 0 };
                mem_id_ = add_device_(std::dynamic_pointer_cast<memory_mapped_device>(mem_ptr_), mem_cfg);
            }

            virtual ~functional_simulator_impl() override { remove_device_(mem_id_); }

        protected:

            virtual void reset_() override
            {
                regs_.clear();
                for (auto [_, info] : devs_)
                    info.ptr->reset();
            }
            virtual identifier_t add_device_(device_ptr dev_ptr, device_config dev_cfg) override
            {
                device_info info = { ++max_cur_id_, dev_ptr, mapping_type::NONE };

                if (dev_cfg.is_addressable) {
                    info.mapping = (dev_cfg.device_mapping == mapping_type::DEFAULT) ? cfg_.device_mapping : dev_cfg.device_mapping;

                    if (info.mapping == mapping_type::MEMORY)
                        mem_asp_.register_device(std::dynamic_pointer_cast<memory_mapped_device>(dev_ptr), dev_cfg.addr_range);
                    else
                        dev_asp_.register_device(std::dynamic_pointer_cast<memory_mapped_device>(dev_ptr), dev_cfg.addr_range);
                }

                devs_[info.id] = info;

                return info.id;
            }
            virtual void remove_device_(identifier_t dev_id) override
            {
                auto dev_it = devs_.find(dev_id);
                if (dev_it == devs_.end())
                    throw invalid_identifier("Device with this ID wasn't registered.");

                auto [_, info] = *dev_it;

                if (info.mapping != mapping_type::NONE) {
                    if (info.mapping == mapping_type::MEMORY)
                        mem_asp_.unregister_device(std::dynamic_pointer_cast<memory_mapped_device>(info.ptr));
                    else
                        dev_asp_.unregister_device(std::dynamic_pointer_cast<memory_mapped_device>(info.ptr));
                }

                // return success::ok;
            }
            virtual byte_t read_byte_(address_t location) override { return mem_asp_.read_byte(location); }
            virtual half_t read_half_(address_t location) override { return mem_asp_.read_half(location); }
            virtual word_t read_word_(address_t location) override { return mem_asp_.read_word(location); }

            virtual void write_byte_(address_t location, byte_t value) override { mem_asp_.write_byte(location, value); }
            virtual void write_half_(address_t location, half_t value) override { mem_asp_.write_half(location, value); }
            virtual void write_word_(address_t location, word_t value) override { mem_asp_.write_word(location, value); }

            // Fields

            config_type cfg_;                   /* Simulator config parameters */
            regfile_type regs_;                 /* Internal register file keeping the state of all registers and flags */
            address_space_type mem_asp_;        /* Memory address space (for memory and optionally other devices) */
            address_space_type dev_asp_;        /* Device address space (if devices are memory-mapped, then it's unused) */
            memptr_type mem_ptr_;               /* Internal memory device pointer */
            identifier_t mem_id_;               /* Internal memory device ID */
            std::unordered_map<identifier_t, device_info> devs_;    /* Pointers and info about all used devices */
            identifier_t max_cur_id_;           /*  */
            // TODO: Interrupt handling

    };

}

#endif  /* _UCLE_CORE_SIMULATORS_FUNCTIONAL_FUNCTIONAL_HPP_ */
