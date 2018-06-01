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

static int data_instr_size(std::string_view dat_opcode) {
    if (util::iequals(dat_opcode, "DW")) return 4;
    if (util::iequals(dat_opcode, "DH")) return 2;
    if (util::iequals(dat_opcode, "DB")) return 1;
    // Should be unreachable
    return 0;
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

        results.push_back(res.details);
    }

    return results;
}

asr::first_pass_result asr::frisc_assembler::first_pass_(const std::vector<parse_details>& parsed_lines)
{
    std::vector<asr::instr_info<>> instrs;
    label_table labels;

    address32_t current_addr = 0;

    for (const auto& line : parsed_lines) {
        auto& line_instr = line["line_instr"];

        if (line_instr.empty()) {
            if (!line["line_label"].empty())
                labels[line["line_label"].contents] = current_addr;
            continue;
        }

        auto& instr_class = line_instr[0];

        if (instr_class[0] == "equ_instr") {
            labels[line["line_label"].contents] = parse_num_const(instr_class[0]["signed_num_const"]);
        } else if (instr_class[0] == "org_instr") {
            address32_t new_addr = parse_num_const(instr_class[0]["signed_num_const"]);

            if (new_addr != cbu::address_rounded(new_addr, 4))
                throw logical_error(fmt::format("Impossible origin (ORG {}), not rounded to a multiple of 4.", new_addr));

            if (new_addr < current_addr)
                throw logical_error(fmt::format("Impossible origin (ORG {}), lesser than current address (= {}).", new_addr, current_addr));

            current_addr = new_addr;
            continue;
        }

        if (instr_class == "reg_instr") {
            instrs.push_back({ current_addr, instr_class[0] });
            current_addr += 4;
        } else if (instr_class[0] == "dsp_instr") {
            address32_t offset = parse_num_const(instr_class[0]["signed_num_const"]);
            current_addr += offset;
        } else if (instr_class[0] == "dat_instr") {
            instrs.push_back({ current_addr, instr_class[0] });

            auto& dat_instr = instr_class[0];
            auto& def_list = dat_instr["def_list"];
            auto data_size = data_instr_size(dat_instr["dat_opcode"].contents);
            auto data_count = def_list.children.size();

            current_addr += cbu::address_rounded(data_count * data_size + sizeof(word_t) - 1, sizeof(word_t));
        } // else do nothing
    }

    return { instrs, labels };
}

asr::second_pass_result asr::frisc_assembler::second_pass_(const std::vector<asr::instr_info<>>& instrs, const label_table& labels)
{
    for (auto i = 0u; i < instrs.size(); ++i) {
        const auto [address, instr] = instrs[i];

        dbg::print_parse_info(instr);

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
            fmt::print("@{} :: {}\n", al.address, al.instr.contents);

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

    auto def_item = (signed_num_const >> cm_sep) / signed_num_const;
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

    auto line = line_label >> spaces_ >> line_instr >> spaces_ >> line_comment >> spaces_ >> eof >= "line";

    parser_ = line;
}


int asr::parse_num_const(parse_details num_const)
{
    if (num_const == "signed_num_const") {
        auto sign = num_const.has("sign") && num_const["sign"] == "-" ? -1 : 1;
        return parse_num_const(num_const["num_const"]) * sign;
    }

    if (num_const != "num_const")
        throw parse_error("Cannot parse num_const - incorrect symbol type.");

    long long value = 0;

    if (num_const[0] == "hex_num")
        util::parse_int(std::string { num_const["hex_num"].contents }, &value, 16);
    else if (num_const[0] == "dec_num")
        util::parse_int(std::string { num_const["dec_num"].contents }, &value, 10);
    else if (num_const[0] == "oct_num")
        util::parse_int(std::string { num_const["oct_num"].contents }, &value, 8);
    else if (num_const[0] == "bin_num")
        util::parse_int(std::string { num_const["bin_num"].contents }, &value, 2);
    else
        fmt::print("Unknown :: {}\n", num_const[0].symbol_name);

    return value;
}

int main(int, char* argv[]) {
    asr::frisc_assembler fasm;
    fasm.assemble(argv[1]);
}
