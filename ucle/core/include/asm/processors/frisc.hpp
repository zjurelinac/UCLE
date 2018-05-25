#include <asm/assembler.hpp>

#include <util/const_bin_util.hpp>

#include <string>

namespace ucle::asr {

    class frisc_assembler {
        using parse_info = parsley::parse_info;
        using cbu = util::const_bin_util<address32_t>;

        public:
            frisc_assembler() { init_parser_(); }
            void assemble(std::string filename);

        private:
            void init_parser_();

            std::string read_file_(std::string filename);
            std::vector<parse_info> parse_lines_(const std::string_view contents);
            first_pass_result first_pass_(const std::vector<parse_info>& parsed_lines);

            int parse_num_const_(parse_info num_const);

            parsley::parser_ptr parser_ { nullptr };
    };

}
