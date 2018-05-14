#include <assemblers/frisc.hpp>

using namespace ucle::asr = asr;

void asr::frisc_assembler::init_parser_()
{

}

void asr::frisc_assembler::assemble(std::string filename)
{
    std::ifstream ifs(filename.c_str());
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));
    
    std::cout << content;
}

int main(int, char* argv[]) {
    asr::frisc_assembler fasm;
    fasm.assemble(argv[1]);
}
