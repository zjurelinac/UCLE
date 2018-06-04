#include <common/types.hpp>

#include <libs/parsley/parsley.hpp>

#include <map>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ucle::asr {

    struct line_info {
        address32_t address;
        std::string_view contents;
        parsley::parse_details instr;
        bool is_instr;
    };

    using label_table = std::unordered_map<std::string_view, address32_t>;

    using first_pass_result = std::pair<std::vector<line_info>, label_table>;
    using second_pass_result = std::unordered_map<address32_t, word_t>;

    class parse_error : public base_exception { using base_exception::base_exception; };
    class logical_error : public base_exception { using base_exception::base_exception; };
    class semantic_error : public base_exception { using base_exception::base_exception; };
    class unimplemented_error : public base_exception { using base_exception::base_exception; };

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
