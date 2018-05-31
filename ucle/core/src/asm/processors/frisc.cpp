#include <asm/processors/frisc.hpp>

#include <libs/fmt/format.h>

#include <util/string.hpp>
#include <util/string_view.hpp>

#include <fstream>
#include <iostream>

using namespace ucle;
using namespace ucle::parsley;

namespace dbg {
    void indent(int level) { for (int i = 0; i < level; ++i) printf("  "); }

    void print_parse_info(const parsley::parse_details& pi, unsigned depth = 1)
    {
        indent(depth);
        fmt::print("<{}> {}\n", pi.symbol_name.length() > 0 ? pi.symbol_name : "unnamed", pi.contents.length() > 0 ? pi.contents : "");
        for (const auto& child : pi.children)
            print_parse_info(child, depth + 1);
    }
}

std::string asr::frisc_assembler::read_file_(std::string filename)
{
    std::ifstream ifs(filename.c_str());
    std::string contents((std::istreambuf_iterator<char>(ifs)),
                         (std::istreambuf_iterator<char>()));
    return contents;
}

std::vector<parse_details> asr::frisc_assembler::parse_lines_(std::string_view contents)
{
    auto lines = util::split(contents, [](auto c){ return c == '\n'; });
    std::vector<parse_details> results;

    for (auto i = 0u; i < lines.size(); ++i) {
        auto res = parser_->parse(lines[i]);

        if (res.status == parse_status::fail)
            throw parse_error(fmt::format("Parse error at line {} :: {}", i + 1, lines[i]));

        results.push_back(res.info);
    }

    return results;
}

asr::first_pass_result asr::frisc_assembler::first_pass_(const std::vector<parse_details>& parsed_lines)
{
    std::vector<asr::line_info<>> lines;
    label_table labels;

    address32_t current_addr = 0;

    for (const auto& line : parsed_lines) {
        auto& line_instr = line["line_instr"];

        // TODO: Unduplicate
        if (line_instr.empty()) {
            lines.push_back({ current_addr, line });

            if (!line["line_label"].empty())
                labels[line["line_label"].contents] = current_addr;

            continue;
        }

        auto& exact_instr = line_instr["any_instr"][0][0];

        if (exact_instr == "org_instr") {
            address32_t new_addr = parse_num_const(exact_instr["num_const"]);

            if (new_addr != cbu::address_rounded(new_addr, 4))
                throw logical_error(fmt::format("Impossible origin (ORG {}), not rounded to a multiple of 4.", new_addr));

            if (new_addr < current_addr)
                throw logical_error(fmt::format("Impossible origin (ORG {}), lesser than current address (= {}).", new_addr, current_addr));

            current_addr = new_addr;
        }

        lines.push_back({ current_addr, line });

        if (!line["line_label"].empty())
            labels[line["line_label"].contents] = current_addr;

        if (line_instr["any_instr"][0] == "psd_instr") {

            if (exact_instr == "dsp_instr") {
                address32_t offset = parse_num_const(exact_instr["num_const"]);
                current_addr += offset;
            } else if (exact_instr == "dat_instr") {
                throw unimplemented_error("Data-def instructions not yet implemented.");
            }
        } else {
            current_addr += 4;
        }
    }

    return { lines, labels };
}

asr::second_pass_result asr::frisc_assembler::second_pass_(const std::vector<asr::line_info<>>& lines, const label_table& labels)
{
    for (auto i = 0u; i < lines.size(); ++i) {
        const auto [address, parsed] = lines[i];

        if (parsed["line_instr"].empty())
            continue;

        const auto& instr_class = parsed["line_instr"]["any_instr"][0];

        dbg::print_parse_info(instr_class);

        if (instr_class == "psd_instr") {

        }

        // try {

        // } catch () {

        // }
    }
}

void asr::frisc_assembler::assemble(std::string filename)
{
    try {
        auto contents = read_file_(filename);
        auto parsed_lines = parse_lines_(contents);
        auto [addressed_lines, labels] = first_pass_(parsed_lines);
        auto assemble_results = second_pass_(addressed_lines, labels);

        for (const auto& al : addressed_lines)
            fmt::print("@{} :: {}\n", al.address, al.parsed.contents);

        for (const auto [label, addr] : labels)
            fmt::print("* {} => {}\n", label, addr);

        // Second pass - actual assembling

    } catch (std::exception& e) {
        fmt::print_colored(stderr, fmt::RED, "{}\n", e.what());
    }
}

void asr::frisc_assembler::init_parser_()
{
    using namespace parsley;

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

    auto bin_const = bin_mod >> spaces >> bin_num      >= "bin_const";
    auto oct_const = oct_mod >> spaces >> oct_num      >= "oct_const";
    auto dec_const = dec_mod >> spaces >> dec_num      >= "dec_const";
    auto hex_const = opt(hex_mod >> spaces) >> hex_num >= "hex_const";

    auto num_const        = bin_const / oct_const / dec_const / hex_const >= "num_const";
    auto signed_num_const = opt(sign) >> num_const                        >= "signed_num_const";

    auto immediate = signed_num_const / label >= "immediate";

    auto gp_reg  = (lit("R") >> cls({'0', '7'})) / lit("SP") >= "gp_reg";
    auto sr_reg  = lit("SR")                                 >= "sr_reg";

    auto reg_off = gp_reg >> spaces_ >> sign >> spaces_ >> immediate >= "reg_off";

    auto alu_operand = gp_reg / immediate           >= "alu_operand";
    auto mem_operand = reg_off / gp_reg / immediate >= "mem_operand";
    auto mov_operand = gp_reg / sr_reg / immediate  >= "mov_operand";

    auto ind_jmp_target = lp_sep >> gp_reg >> rp_sep                >= "ind_jmp_target";
    auto jmp_target     = signed_num_const / label / ind_jmp_target >= "jmp_target";

    auto cond_flag = lit("ULE") / lit("ULT") / lit("UGE") / lit("UGT") /
                     lit("SLE") / lit("SLT") / lit("SGE") / lit("SGT") /
                     lit("NC")  / lit("NV")  / lit("NN")  / lit("NZ")  /
                     lit("C")   / lit("V")   / lit("N")   / lit("Z")   /
                     lit("M")   / lit("P")   / lit("EQ")  / lit("NE");

    auto condition = opt(undersc >> cond_flag) >= "condition";

    auto alu_opcode = lit("ADD") / lit("ADC") / lit("SUB") / lit("SBC") / lit("AND") / lit("OR")  / lit("XOR") /
                      lit("SHL") / lit("SHR") / lit("ASHR") / lit("ROTL") / lit("ROTR")                        >= "alu_opcode";
    auto cmp_opcode = lit("CMP")                                                                               >= "cmp_opcode";
    auto mov_opcode = lit("MOVE")                                                                              >= "mov_opcode";
    auto mem_opcode = lit("LOADH") / lit("LOADB") / lit("LOAD") / lit("STOREH") / lit("STOREB") / lit("STORE") >= "mem_opcode";
    auto stk_opcode = lit("PUSH") / lit("POP")                                                                 >= "stk_opcode";
    auto jmp_opcode = lit("JP") / lit("JR") / lit("CALL")                                                      >= "jmp_opcode";
    auto ret_opcode = lit("RETI") / lit("RETN") / lit("RET") / lit("HALT")                                     >= "ret_opcode";

    auto org_opcode = lit("ORG")                        >= "org_opcode";
    auto equ_opcode = lit("EQU")                        >= "equ_opcode";
    auto dsp_opcode = lit("DS")                         >= "dsp_opcode";
    auto dat_opcode = lit("DW") / lit("DH") / lit("DB") >= "dat_opcode";

    auto src1 = gp_reg          >= "src1";
    auto src2 = alu_operand     >= "src2";
    auto dest = gp_reg          >= "dest";
    auto msrc = mov_operand     >= "msrc";
    auto mdst = gp_reg / sr_reg >= "mdst";

    auto reg  = gp_reg                          >= "reg";
    auto addr = lp_sep >> mem_operand >> rp_sep >= "addr";

    // auto def_item =
    // auto def_list =

    auto alu_instr = alu_opcode >> spaces >> src1 >> cm_sep >> src2 >> cm_sep >> dest   >= "alu_instr";
    auto cmp_instr = cmp_opcode >> spaces >> src1 >> cm_sep >> src2                     >= "cmp_instr";
    auto mov_instr = mov_opcode >> spaces >> msrc >> cm_sep >> mdst                     >= "mov_instr";
    auto mem_instr = mem_opcode >> spaces >> reg  >> cm_sep >> addr                     >= "mem_instr";
    auto stk_instr = stk_opcode >> spaces >> reg                                        >= "stk_instr";
    auto jmp_instr = jmp_opcode >> condition >> spaces >> jmp_target                    >= "jmp_instr";
    auto ret_instr = ret_opcode >> condition                                            >= "ret_instr";

    // TODO: Pseudo instructions

    auto equ_instr = equ_opcode >> spaces >> num_const >= "equ_instr";
    auto org_instr = org_opcode >> spaces >> num_const >= "org_instr";
    auto dsp_instr = dsp_opcode >> spaces >> num_const >= "dsp_instr";
    // auto dat_instr = dat_opcode >> spaces >>

    auto reg_instr = alu_instr / mem_instr / mov_instr / stk_instr / jmp_instr / ret_instr >= "reg_instr";
    auto psd_instr = equ_instr / org_instr / dsp_instr                                     >= "psd_instr";
    auto any_instr = psd_instr / reg_instr                                                 >= "any_instr";

    auto line_label   = label / eps()                            >= "line_label";
    auto line_comment = (semicol >> (~eol >> any()) * N) / eps() >= "line_comment";
    auto line_instr   = any_instr / eps()                        >= "line_instr";

    auto line  = line_label >> spaces_ >> line_instr >> spaces_ >> line_comment >> spaces_ >> eof >= "line";
    auto lines = line * N  >= "lines";

    // parser_ = lines >> eof >= "file";
    parser_ = line;
}

int asr::parse_num_const(parse_details num_const)
{
    if (num_const != "num_const")
        throw parse_error("Cannot parse num_const - incorrect symbol type.");

    int value = 0;

    if (num_const[0] == "hex_const")
        util::parse_int(std::string { num_const[0]["hex_num"].contents }, &value, 16);
    else if (num_const[0] == "dec_const")
        util::parse_int(std::string { num_const[0]["dec_num"].contents }, &value, 10);
    else if (num_const[0] == "oct_const")
        util::parse_int(std::string { num_const[0]["oct_num"].contents }, &value, 8);
    else if (num_const[0] == "bin_const")
        util::parse_int(std::string { num_const[0]["bin_num"].contents }, &value, 2);
    else
        fmt::print("Unknown :: {}\n", num_const[0].symbol_name);

    return value;
}

int main(int, char* argv[]) {
    asr::frisc_assembler fasm;
    fasm.assemble(argv[1]);
}
