#include <libs/parsley/parsley.hpp>
#include <libs/fmt/format.h>

using namespace ucle;
using namespace ucle::parsley;

void indent(int level) { for (int i = 0; i < level; ++i) printf("  "); }

void print_parse_info(const parse_info& pi, unsigned depth = 1)
{
    indent(depth);
    fmt::print("<{}> {}\n", pi.symbol_name.length() > 0 ? pi.symbol_name : "unnamed", pi.contents.length() > 0 ? pi.contents : "");
    for (const auto& child : pi.children)
        print_parse_info(child, depth + 1);
}

void try_parse(const parsers::base_ptr& p, const char* input)
{
    auto res = p->parse(input);
    fmt::print("Parsing {} :: [{}]\n", input, to_string(res.status));
    print_parse_info(res.info);
}

void try_parse(symbol& s, const char* input)
{
    auto res = s.parse(input);
    // fmt::print("{} => {} {} [{}]\n", input, to_string(res.status), res.info.contents, res.info.symbol_name.length() > 0 ? res.info.symbol_name : "none");
    fmt::print("Parsing {} :: [{}]\n", input, to_string(res.status));
    print_parse_info(res.info);
}

int main() {

    auto comma   = lit(",");
    auto lparens = lit("(");
    auto rparens = lit(")");
    auto undersc = lit("_");
    auto percent = lit("%");

    auto space  = cls(" \t\r\n");
    auto spaces = space * N;

    auto sep = comma / space;
    auto eol = ~any();

    // auto bin_digit = cls({'0', '1'});
    // auto oct_digit = cls({'0', '7'});
    // auto dec_digit = cls({'0', '9'});
    // auto hex_digit = cls({{'0', '9'}, {'A', 'F'}, {'a', 'f'}});

    // auto id_start = lit("_") / cls({{'A', 'Z'}, {'a', 'z'}});
    // auto id_cont  = lit("_") / cls({{'A', 'Z'}, {'a', 'z'}, {'0', '9'}});
    // auto id = id_start >> id_cont * N;

    // auto bin_mod = percent >> cls("Bb");
    // auto oct_mod = percent >> cls("Oo");
    // auto dec_mod = percent >> cls("Dd");
    // auto hex_mod = percent >> cls("Hh");

    // auto cond_flag = lit("ULE") / lit("ULT") / lit("UGE") / lit("UGT") /
    //                  lit("SLE") / lit("SLT") / lit("SGE") / lit("SGT") /
    //                  lit("NC")  / lit("NV")  / lit("NN")  / lit("NZ")  /
    //                  lit("C")   / lit("V")   / lit("N")   / lit("Z")   /
    //                  lit("M")   / lit("P")   / lit("EQ")  / lit("NE");

    // symbol bin_num { "bin_num" }, oct_num { "oct_num" }, dec_num { "dec_num" }, hex_num { "hex_num" }, numeric_const { "numeric_const" },
    //        label { "label" },           
    //        gp_reg { "gp_reg" }, sr_reg { "sr_reg" }, any_reg { "register" },
           
    //        condition { "condition" }, jmp_target { "jmp_target" }, indirect_jmp_target { "indirect_jmp_target" },

    //        src1 { "src1" }, src2 { "src2" }, dest { "dest" },

    //        alu_opcode { "alu_opcode" }, cmp_opcode { "cmp_opcode" }, mem_opcode { "mem_opcode" },
    //        stk_opcode { "stk_opcode" }, jmp_opcode { "jmp_opcode" }, ret_opcode { "ret_opcode" },
           
    //        alu_instr { "alu_instr" }, cmp_instr { "cmp_instr" }, mem_instr { "mem_instr" },
    //        stk_instr { "stk_instr" }, jmp_instr { "jmp_instr" }, ret_instr { "ret_instr" };

    // bin_num <= bin_digit + N;
    // oct_num <= oct_digit + N;
    // dec_num <= dec_digit + N;
    // hex_num <= dec_digit >> hex_digit * N;

    // numeric_const <= ((bin_mod >> spaces >> sym(bin_num)) / (oct_mod >> spaces >> sym(oct_num)) / (dec_mod >> spaces >> sym(dec_num)) / (opt(hex_mod >> spaces) >> sym(hex_num))) >> (&sep / eol);
    // label         <= id;
    // gp_reg        <= lit("R0") / lit("R1") / lit("R2") / lit("R3") / lit("R4") / lit("R5") / lit("R6") / lit("R7") / lit("SP");
    // sr_reg        <= lit("SR");
    // any_reg       <= sym(gp_reg) / sym(sr_reg);

    // condition           <= opt(undersc >> cond_flag);
    // indirect_jmp_target <= lparens >> sym(gp_reg) >> rparens;
    // jmp_target          <= sym(numeric_const) / sym(label) / sym(indirect_jmp_target);

    // src1 <= sym(gp_reg);
    // src2 <= sym(gp_reg) / sym(numeric_const) / sym(label);
    // dest <= sym(gp_reg);

    // alu_opcode <= lit("ADD") / lit("ADC") / lit("SUB") / lit("SBC") / lit("AND") / lit("OR")  / lit("XOR") /
    //               lit("SHL") / lit("SHR") / lit("ASHR") / lit("ROTL") / lit("ROTR");
    // cmp_opcode <= lit("CMP");
    // mem_opcode <= lit("LOAD") / lit("LOADH") / lit("LOADB") / lit("STORE") / lit("STOREH") / lit("STOREB");
    // stk_opcode <= lit("PUSH") / lit("POP");
    // jmp_opcode <= lit("JP") / lit("JR") / lit("CALL");
    // ret_opcode <= lit("RET") / lit("RETI") / lit("RETN") / lit("HALT");

    // ret_instr <= sym(ret_opcode) >> sym(condition);
    // jmp_instr <= sym(jmp_opcode) >> sym(condition) >> spaces >> sym(jmp_target);

    /*** Specific tests ***/

    // try_parse(bin_num, "0100101");
    // try_parse(bin_num, "0100121");
    // try_parse(oct_num, "7100234");
    // try_parse(oct_num, "0109152");
    // try_parse(dec_num, "1264391");
    // try_parse(dec_num, "5409A52");
    // try_parse(hex_num, "0ABcd12");
    // try_parse(hex_num, "cdEF124");

    // try_parse(numeric_const, "%B 010100");
    // try_parse(numeric_const, "%B 010102");
    // try_parse(numeric_const, "%O  1234567");
    // try_parse(numeric_const, "%O 1009");
    // try_parse(numeric_const, "%D    12345");
    // try_parse(numeric_const, "%D  100C");
    // try_parse(numeric_const, "%H 1234AB");
    // try_parse(numeric_const, "0AB12ef");

    // try_parse(label, "LOOP_123");
    // try_parse(label, "0LOOP_123");

    // try_parse(any_reg, "R0");
    // try_parse(any_reg, "SP");
    // try_parse(any_reg, "SR");
    // try_parse(any_reg, "PC");

    // try_parse(ret_instr, "RET");
    // try_parse(ret_instr, "RET_NZ");

    // try_parse(jmp_instr, "JP LOOP1");
    // try_parse(jmp_instr, "JP_NZ LOOP2");
    // try_parse(jmp_instr, "CALL (R0)");
}
