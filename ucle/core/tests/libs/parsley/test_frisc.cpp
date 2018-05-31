#include <libs/parsley/parsley.hpp>
#include <libs/fmt/format.h>

using namespace ucle;
using namespace ucle::parsley;

void indent(int level) { for (int i = 0; i < level; ++i) printf("  "); }

void print_parse_info(const parse_details& pi, unsigned depth = 1)
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
    print_parse_info(res.details);
}

int main() {

    auto comma   = lit(",");
    auto semicol = lit(";");
    auto lparens = lit("(");
    auto rparens = lit(")");
    auto undersc = lit("_");
    auto percent = lit("%");
    auto plus    = lit("+");
    auto minus   = lit("-");

    auto sign    = plus / minus >= "sign";

    auto space   = cls(" \t");
    auto spaces  = space + N;
    auto spaces_ = space * N;

    auto cm_sep = spaces_ >> comma >> spaces_;
    auto lp_sep = lparens >> spaces_;
    auto rp_sep = spaces_ >> rparens;

    auto eol = lit("\r\n") / lit("\r") / lit("\n");
    auto eof = ~any();

    auto bin_digit = cls({'0', '1'});
    auto oct_digit = cls({'0', '7'});
    auto dec_digit = cls({'0', '9'});
    auto hex_digit = cls({{'0', '9'}, {'A', 'F'}, {'a', 'f'}});

    auto id_start = lit("_") / cls({{'A', 'Z'}, {'a', 'z'}});
    auto id_cont  = lit("_") / cls({{'A', 'Z'}, {'a', 'z'}, {'0', '9'}});
    auto id = id_start >> id_cont * N;

    auto bin_mod = percent >> cls("Bb");
    auto oct_mod = percent >> cls("Oo");
    auto dec_mod = percent >> cls("Dd");
    auto hex_mod = percent >> cls("Hh");

    auto bin_num = bin_digit + N              >= "bin_num";
    auto oct_num = oct_digit + N              >= "oct_num";
    auto dec_num = dec_digit + N              >= "dec_num";
    auto hex_num = dec_digit >> hex_digit * N >= "hex_num";

    auto label = id >= "label";

    auto bin_const = bin_mod >> spaces >> bin_num;
    auto oct_const = oct_mod >> spaces >> oct_num;
    auto dec_const = dec_mod >> spaces >> dec_num;
    auto hex_const = opt(hex_mod >> spaces) >> hex_num;

    auto num_const        = bin_const / oct_const / dec_const / hex_const >= "num_const";
    auto signed_num_const = opt(sign) >> num_const                        >= "signed_num_const";

    auto immediate = signed_num_const / label;

    auto gp_reg  = (ilit("R") >> cls({'0', '7'})) / ilit("SP") >= "gp_reg";
    auto sr_reg  = ilit("SR")                                  >= "sr_reg";

    auto reg_off = gp_reg >> spaces_ >> sign >> spaces_ >> immediate >= "reg_off";

    auto alu_operand = gp_reg / immediate;
    auto mem_operand = reg_off / gp_reg / immediate;
    auto mov_operand = gp_reg / sr_reg / immediate;

    auto ind_jmp_target = lp_sep >> gp_reg >> rp_sep;
    auto jmp_target     = signed_num_const / label / ind_jmp_target;

    auto addr = lp_sep >> mem_operand >> rp_sep;

    auto cond_flag = ilit("ULE") / ilit("ULT") / ilit("UGE") / ilit("UGT") /
                     ilit("SLE") / ilit("SLT") / ilit("SGE") / ilit("SGT") /
                     ilit("NC")  / ilit("NV")  / ilit("NN")  / ilit("NZ")  /
                     ilit("C")   / ilit("V")   / ilit("N")   / ilit("Z")   /
                     ilit("M")   / ilit("P")   / ilit("EQ")  / ilit("NE");

    auto condition = opt(undersc >> cond_flag) >= "condition";

    auto def_item = (num_const >> cm_sep) / num_const;
    auto def_list = def_item + N >= "def_list";

    auto alu_opcode = ilit("ADD") / ilit("ADC") / ilit("SUB") / ilit("SBC") / ilit("AND") / ilit("OR")  / ilit("XOR") /
                      ilit("SHL") / ilit("SHR") / ilit("ASHR") / ilit("ROTL") / ilit("ROTR")                         >= "alu_opcode";
    auto cmp_opcode = ilit("CMP")                                                                                    >= "cmp_opcode";
    auto mov_opcode = ilit("MOVE")                                                                                   >= "mov_opcode";
    auto mem_opcode = ilit("LOADH") / ilit("LOADB") / ilit("LOAD") / ilit("STOREH") / ilit("STOREB") / ilit("STORE") >= "mem_opcode";
    auto stk_opcode = ilit("PUSH") / ilit("POP")                                                                     >= "stk_opcode";
    auto jmp_opcode = ilit("JP") / ilit("JR") / ilit("CALL")                                                         >= "jmp_opcode";
    auto ret_opcode = ilit("RETI") / ilit("RETN") / ilit("RET") / ilit("HALT")                                       >= "ret_opcode";

    auto org_opcode = ilit("ORG")                          >= "org_opcode";
    auto equ_opcode = ilit("EQU")                          >= "equ_opcode";
    auto dsp_opcode = ilit("DS")                           >= "dsp_opcode";
    auto dat_opcode = ilit("DW") / ilit("DH") / ilit("DB") >= "dat_opcode";

    auto alu_instr = alu_opcode >> spaces >> gp_reg >> cm_sep >> alu_operand >> cm_sep >> gp_reg >= "alu_instr";
    auto cmp_instr = cmp_opcode >> spaces >> gp_reg >> cm_sep >> alu_operand                     >= "cmp_instr";
    auto mov_instr = mov_opcode >> spaces >> mov_operand >> cm_sep >> (gp_reg / sr_reg)          >= "mov_instr";
    auto mem_instr = mem_opcode >> spaces >> gp_reg >> cm_sep >> addr                            >= "mem_instr";
    auto stk_instr = stk_opcode >> spaces >> gp_reg                                              >= "stk_instr";
    auto jmp_instr = jmp_opcode >> condition >> spaces >> jmp_target                             >= "jmp_instr";
    auto ret_instr = ret_opcode >> condition                                                     >= "ret_instr";

    auto equ_instr = equ_opcode >> spaces >> immediate >= "equ_instr";
    auto org_instr = org_opcode >> spaces >> immediate >= "org_instr";
    auto dsp_instr = dsp_opcode >> spaces >> immediate >= "dsp_instr";
    auto dat_instr = dat_opcode >> spaces >> def_list  >= "dat_instr";

    auto reg_instr = alu_instr / mem_instr / mov_instr / stk_instr / jmp_instr / ret_instr >= "reg_instr";
    auto psd_instr = equ_instr / org_instr / dsp_instr / dat_instr                         >= "psd_instr";
    auto any_instr = psd_instr / reg_instr;

    auto line_label   = label / eps()                            >= "line_label";
    auto line_comment = (semicol >> (~eol >> any()) * N) / eps() >= "line_comment";
    auto line_instr   = any_instr / eps()                        >= "line_instr";

    auto line = line_label >> spaces_ >> line_instr >> spaces_ >> line_comment >> spaces_ >> eol >= "line";

    auto lines = line * N     >= "lines";
    auto file  = lines >> eof >= "file";

    /*** Specific tests ***/

    //// Basic parts

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

    //// Instructions

    // try_parse(alu_instr, "ADD R0, 124, R0");
    // try_parse(alu_instr, "ADD R1, R2, R3");

    // try_parse(cmp_instr, "CMP R0, 7");
    // try_parse(cmp_instr, "CMP R0, SP");

    // try_parse(mov_instr, "MOVE R1, R2");
    // try_parse(mov_instr, "MOVE 1000, SP");
    // try_parse(mov_instr, "MOVE -1, SP");
    // try_parse(mov_instr, "MOVE SR, R0");
    // try_parse(mov_instr, "MOVE R0, SR");

    // try_parse(mem_instr, "LOAD  R0, (LABEL)");
    // try_parse(mem_instr, "LOADH R1, (1234)");
    // try_parse(mem_instr, "LOADB R2, (SP)");

    // try_parse(mem_instr, "STORE R3, (R7 + 12)");
    // try_parse(mem_instr, "STORE R3, (R7 + %D 12)");
    // try_parse(mem_instr, "STORE R3, (R7-4)");
    // try_parse(mem_instr, "STORE R3, (R7- %O 4)");

    // try_parse(stk_instr, "PUSH R0");
    // try_parse(stk_instr, "POP SP");

    // try_parse(jmp_instr, "JP LOOP1");
    // try_parse(jmp_instr, "JP_NZ LOOP2");
    // try_parse(jmp_instr, "CALL (R0)");

    // try_parse(ret_instr, "RETI");
    // try_parse(ret_instr, "RET_NZ");

    //// Whole lines

    // try_parse(line, "\tPUSH R0\n");
    // try_parse(line, "\tPUSH R0\n");
    // try_parse(line, "LABEL RET\n");
    // try_parse(line, "\n");
    // try_parse(line, "LABEL ; comment\n");
    // try_parse(line, "; comment\r\n");
    // try_parse(line, "   ; comment\r\n");
    // try_parse(line, " RETN ; comment\r\n");

    //// Whole file

auto file_test = R"(
    MOVE 1000, SP
    ADD R0, 0, R1
    XOR R2, R2, R2
    LOAD R3, (1000)
    LOAD R4, (LABEL)
    LOAD R5, (SP+4)
    LOAD R6, (SP-8)
    LOAD R7, (SP)
    CALL F
    JP (R0)

    dw 20, 30, 40
)";

/*
F   PUSH R0
    ADD R1, R2, R3
    POP R0
    RET
*/

    try_parse(file, file_test);

    // /// Visitors

    // visitor v;

    // v["line"] = [](auto& v) { fmt::print("A line!\n"); return nullptr; };

    // v.visit(file->parse(file_test));
}
