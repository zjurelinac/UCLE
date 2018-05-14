#include <asm/assembler.hpp>

namespace ucle::asr {
    
    class frisc_assembler : public assembler {
        public:
            // frisc_assembler() { init_parser_(); }
            void assemble(std::string filename);

        protected:
            // void parse_(std::string_view) override;

        private:
            // void init_parser_();

            // parser_ptr instr_parser_ { nullptr };

            // parser_ptr line_parser_ { nullptr };
            // parser_ptr file_parser_ { nullptr };
    };

};