#ifndef _UCLE_CORE_SIMULATORS_FUNCTIONAL_FUNCTIONAL_HPP_
#define _UCLE_CORE_SIMULATORS_FUNCTIONAL_FUNCTIONAL_HPP_

#include <common/types.hpp>

#include <fnsim/fnsim.hpp>
#include <fnsim/address_space.hpp>
#include <fnsim/basic_devices.hpp>

#include <set>
#include <string>
#include <unordered_map>

namespace ucle::fnsim {

    struct simulator_config {
        size_t          memory_size;
        address_range   memory_addr_range = {0, 0xFFFFFFFF};
        byte_order      endianness = byte_order::LE;
        mapping_type    device_mapping = mapping_type::MEMORY;
        address_range   devices_addr_range = {0, 0};
    };

    template <typename RegisterFile, typename AddressSpace,
              template<byte_order> typename Memory, typename Config = simulator_config>
    class functional_simulator_impl : public functional_simulator {
            // using memory_ptr = std::shared_ptr<mapped_device>;

            struct device_info {
                identifier_t    id;
                device_ptr      ptr;
                mapping_type    mapping;
            };

        public:
            template <byte_order E>
            using memory_type = Memory<E>;

            using regfile_type = RegisterFile;
            using config_type = Config;
            using address_space_type = AddressSpace;
            using mapped_device_type = typename AddressSpace::mapped_device_type;
            using mapped_device_ptr = typename AddressSpace::mapped_device_ptr;

            functional_simulator_impl(config_type cfg)
                : cfg_(cfg), mem_asp_(cfg.memory_addr_range), dev_asp_(cfg.devices_addr_range)
            {
                if (cfg.endianness == byte_order::LE)
                    mem_ptr_ = std::make_shared<memory_type<byte_order::LE>>(cfg.memory_size);
                else
                    mem_ptr_ = std::make_shared<memory_type<byte_order::BE>>(cfg.memory_size);

                device_config mem_cfg { true, false, mapping_type::MEMORY, cfg.memory_addr_range, 0 };
                mem_id_ = add_device_(std::dynamic_pointer_cast<mapped_device_type>(mem_ptr_), mem_cfg);
            }

            virtual ~functional_simulator_impl() override { remove_device_(mem_id_); }

        protected:

            virtual void reset_() override
            {
                regs_.clear();
                for (auto [_, info] : devs_)
                    info.ptr->reset();
            }

            virtual byte_t get_byte_(address_t location) const override
            {
                return read_byte_(location);
            }
            virtual void set_byte_(address_t location, byte_t value) override
            {
                return write_byte_(location, value);
            }

            virtual identifier_t add_device_(device_ptr dev_ptr, device_config dev_cfg) override
            {
                device_info info = { ++max_cur_id_, dev_ptr, mapping_type::NONE };

                if (dev_cfg.is_addressable) {
                    info.mapping = (dev_cfg.device_mapping == mapping_type::DEFAULT) ? cfg_.device_mapping : dev_cfg.device_mapping;

                    if (info.mapping == mapping_type::MEMORY)
                        mem_asp_.register_device(std::dynamic_pointer_cast<mapped_device_type>(dev_ptr), dev_cfg.addr_range);
                    else
                        dev_asp_.register_device(std::dynamic_pointer_cast<mapped_device_type>(dev_ptr), dev_cfg.addr_range);
                }

                devs_[info.id] = info;

                return info.id;
            }
            virtual void remove_device_(identifier_t dev_id) override
            {
                auto dev_it = devs_.find(dev_id);
                if (dev_it == devs_.end())
                    throw invalid_identifier("Device with this ID wasn't registered.");

                const auto &info = dev_it->second;

                if (info.mapping != mapping_type::NONE) {
                    if (info.mapping == mapping_type::MEMORY)
                        mem_asp_.unregister_device(std::dynamic_pointer_cast<mapped_device_type>(info.ptr));
                    else
                        dev_asp_.unregister_device(std::dynamic_pointer_cast<mapped_device_type>(info.ptr));
                }
            }

            byte_t read_byte_(address_t location) const { return mem_asp_.read_byte(location); }
            half_t read_half_(address_t location) const { return mem_asp_.read_half(location & ~0b1); }
            word_t read_word_(address_t location) const { return mem_asp_.read_word(location & ~0b11); }

            void write_byte_(address_t location, byte_t value) { mem_asp_.write_byte(location, value); }
            void write_half_(address_t location, half_t value) { mem_asp_.write_half(location & ~0b1, value); }
            void write_word_(address_t location, word_t value) { mem_asp_.write_word(location & ~0b11, value); }

            // Fields

            config_type cfg_;                   /* Simulator config parameters */
            regfile_type regs_;                 /* Internal register file keeping the state of all registers and flags */
            address_space_type mem_asp_;        /* Memory address space (for memory and optionally other devices) */
            address_space_type dev_asp_;        /* Device address space (if devices are memory-mapped, then it's unused) */
            mapped_device_ptr mem_ptr_;         /* Internal memory device pointer */
            identifier_t mem_id_;               /* Internal memory device ID */
            std::unordered_map<identifier_t, device_info> devs_;    /* Pointers and info about all used devices */
            identifier_t max_cur_id_;           /*  */
            // TODO: Interrupt handling

    };

}

#endif  /* _UCLE_CORE_SIMULATORS_FUNCTIONAL_FUNCTIONAL_HPP_ */
