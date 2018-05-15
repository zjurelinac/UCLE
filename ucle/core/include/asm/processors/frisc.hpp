#include <asm/assembler.hpp>

#include <string>

namespace ucle::asr {

    class frisc_assembler {
        public:
            frisc_assembler() { init_parser_(); }
            void assemble(std::string filename);

        protected:


        private:
            void init_parser_();

            parsley::parser_ptr parser_ { nullptr };
    };

}
