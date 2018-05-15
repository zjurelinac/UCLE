#include <common/types.hpp>

#include <libs/parsley/parsley.hpp>

#include <string_view>

namespace ucle::asr {
    
    template <typename AddressType = address32_t>
    struct line_info {
        using address_type = AddressType;

        std::string_view contents;
        address_type address { 0 };
    };

    // class assembler {
    //     public:
    //         void assemble(std::string filename);
    //     protected:
    //         virtual build_instr_parser_() = 0;
    //         virtual build_line_parser_() = 0;
            
    //         void build_parser_();

    //     //     virtual void parse_(std::string_view) = 0;
    // };

}