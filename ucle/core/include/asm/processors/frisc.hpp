#include <asm/assembler.hpp>

#include <util/const_bin_util.hpp>

#include <string>

namespace ucle::asr {

    class frisc_assembler {
        using parse_details = parsley::parse_details;
        using cbu = util::const_bin_util<address32_t>;

        public:
            frisc_assembler() { init_parser_(); }
            void assemble(std::string filename);

        private:
            void init_parser_();

            std::string read_file_(std::string filename);
            std::vector<parse_details> parse_lines_(const std::string_view contents);
            first_pass_result first_pass_(const std::vector<parse_details>& parsed_lines);
            second_pass_result second_pass_(const std::vector<line_info>& lines, const label_table& labels);

            parsley::parser_ptr parser_ { nullptr };
    };

}
