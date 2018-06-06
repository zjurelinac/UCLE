#include <asm/processors/frisc.hpp>

#include <common/structures.hpp>

#include <libs/fmt/format.h>

#include <util/filesystem.hpp>
#include <util/string.hpp>
#include <util/string_view.hpp>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unordered_map>

using namespace ucle;
using namespace ucle::asr;
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

namespace detail {
    static inline bool fits_into_nbits(unsigned num, int n)
    {
        return num >> n == 0;
    }

    static inline bool fits_ext_into_nbits(int num, int n)
    {
        auto top_bits = num >> n;
        return top_bits == 0 || top_bits == -1;
    }

    static inline void fill_bytes(int value, unsigned byte_no, small_byte_vector& bv)
    {
        using cbu = util::const_bin_util<int>;

        for (auto i = 0u; i < byte_no; ++i)
            bv.push_back(cbu::nth_byte_of(value, i));
    }

    static inline word_t flush_bytes(small_byte_vector& bv)
    {
        word_t value = 0;
        for (int i = bv.size() - 1; i >= 0; --i)
            value = value << 8 | bv[i];
        bv.clear();
        return value;
    }

    static int parse_num_const(const parse_details& num_const)
    {
        if (num_const == "signed_num_const") {
            auto sign = num_const.has("sign") && num_const["sign"].contents == "-" ? -1 : 1;
            return parse_num_const(num_const["num_const"]) * sign;
        }

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

    static int parse_gp_reg(const parse_details& gp_reg)
    {
        static std::unordered_map<std::string_view, int> gp_regs = {
            { "R0", 0b000 }, { "R1", 0b001 }, { "R2", 0b010 },
            { "R3", 0b011 }, { "R4", 0b100 }, { "R5", 0b101 },
            { "R6", 0b110 }, { "R7", 0b111 }, { "SP", 0b111 }
        };

        return gp_regs[gp_reg.contents];
    }

    static int parse_label(const parse_details& label, const label_table& labels)
    {
        if (!labels.count(label.contents))
            throw logical_error(fmt::format("Unknown label: {}", label.contents));
        return labels.at(label.contents);
    }

    static int parse_immediate(const parse_details& imm, const label_table& labels)
    {
        if (imm == "label")
            return parse_label(imm, labels);

        auto val = parse_num_const(imm);

        if (!fits_ext_into_nbits(val, 20))
            throw logical_error(fmt::format("Constant {} cannot fit into sign-extended 20 immediate values.", imm.contents));

        return val;
    }

    static int parse_opcode(const parse_details& opcode)
    {
        static std::unordered_map<std::string_view, int> opcodes = {
            { "MOVE", 0b00000 }, { "OR", 0b00001 }, { "AND", 0b00010 }, { "XOR", 0b00011 },
            { "ADD", 0b00100 }, { "ADC", 0b00101 }, { "SUB", 0b00110 }, { "SBC", 0b00111 },
            { "ROTL", 0b01000 }, { "ROTR", 0b01001 }, { "SHL", 0b01010 }, { "SHR", 0b01011 },
            { "ASHR", 0b01100 }, { "CMP", 0b01101 }, { "POP", 0b10000 }, { "PUSH", 0b10001 },
            { "LOADB", 0b10010 }, { "STOREB", 0b10011 }, { "LOADH", 0b10100 }, { "STOREH", 0b10101 },
            { "LOAD", 0b10110 }, { "STORE", 0b10111 }, { "JP", 0b11000 }, { "CALL", 0b11001 },
            { "JR", 0b11010 }, { "RET", 0b11011 }, { "RETI", 0b11011 }, { "RETN", 0b11011 },
            { "HALT", 0b11111 }
        };

        return opcodes[opcode.contents];
    }

    static int parse_reg_off(const parse_details& reg_off)
    {
        if (reg_off == "gp_reg")
            return parse_gp_reg(reg_off) << 20;

        word_t reg = parse_gp_reg(reg_off[0]);
        auto sign = reg_off["sign"].contents == "-" ? -1 : 1;
        word_t imm = parse_num_const(reg_off[2]);

        if (!fits_ext_into_nbits(imm, 20))
            throw logical_error(fmt::format("Constant {} cannot fit into sign-extended 20 immediate values.", reg_off[2].contents));

        return reg << 20 | (imm * sign & 0xFFFFF);
    }

    static int parse_condition(const parse_details& condition)
    {
        static std::unordered_map<std::string_view, int> conditions = {
            { "N", 0b0001 }, { "M", 0b0001 }, { "NN", 0b0010 }, { "P", 0b0010 },
            { "C", 0b0011 }, { "UGE", 0b0011 }, { "NC", 0b0100 }, { "ULT", 0b0100 },
            { "V", 0b0101 }, { "NV", 0b0110 }, { "Z", 0b0111 }, { "EQ", 0b0111 },
            { "NZ", 0b1000 }, { "NE", 0b1000 }, { "ULE", 0b1001 }, { "UGT", 0b1010 },
            { "SLT", 0b1011 }, { "SLE", 0b1100 }, { "SGE", 0b1101 }, { "SGT", 0b1110 }
        };

        if (condition.empty())
            return 0;

        return conditions[condition.contents.substr(1)];
    }

    static int parse_retcode(const parse_details& opcode)
    {
        static std::unordered_map<std::string_view, int> retcodes = {
            { "RET", 0b00 }, { "RETI", 0b01 }, { "RETN", 0b11 }, { "HALT", 0b00 }
        };

        return retcodes[opcode.contents];
    }

    static inline int data_instr_size(std::string_view dat_opcode)
    {
        if (util::iequals(dat_opcode, "DW")) return 4;
        if (util::iequals(dat_opcode, "DH")) return 2;
        if (util::iequals(dat_opcode, "DB")) return 1;
        // Should be unreachable
        return 0;
    }

    static word_t parse_alu_instr(const parse_details& instr, const label_table& labels)
    {
        word_t opcode = parse_opcode(instr["opcode"]);
        word_t fn = instr[2] != "gp_reg";
        word_t src1 = parse_gp_reg(instr[1]);
        word_t src2 = fn ? parse_immediate(instr[2], labels) : parse_gp_reg(instr[2]) << 17;
        word_t dest = parse_gp_reg(instr[3]);

        return opcode << 27 | fn << 26 | dest << 23 | src1 << 20 | (src2 & 0xFFFFF);
    }

    static word_t parse_cmp_instr(const parse_details& instr, const label_table& labels)
    {
        word_t opcode = parse_opcode(instr["opcode"]);
        word_t fn = instr[2] != "gp_reg";
        word_t src1 = parse_gp_reg(instr[1]);
        word_t src2 = fn ? parse_immediate(instr[2], labels) : parse_gp_reg(instr[2]) << 17;

        return opcode << 27 | fn << 26 | src1 << 20 | (src2 & 0xFFFFF);
    }

    static word_t parse_mov_instr(const parse_details& instr, const label_table& labels)
    {
        word_t opcode = parse_opcode(instr["opcode"]);
        word_t sr_src = instr[1] == "sr_reg";
        word_t sr_dst = instr[2] == "sr_reg";
        word_t fn = !sr_src && instr[1] != "gp_reg";
        word_t src = fn ? parse_immediate(instr[1], labels) : (sr_src ? 0 : parse_gp_reg(instr[1]) << 17);
        word_t dst = (sr_dst ? 0 : parse_gp_reg(instr[2]));

        return opcode << 27 | fn << 26 | dst << 23 | sr_src << 21 | sr_dst << 20 | (src & 0xFFFFF);
    }

    static word_t parse_mem_instr(const parse_details& instr, const label_table& labels) {
        word_t opcode = parse_opcode(instr["opcode"]);
        word_t fn = instr[2] == "gp_reg" || instr[2] == "reg_off";
        word_t reg = parse_gp_reg(instr[1]);
        word_t addr = fn ? parse_reg_off(instr[2]) : parse_immediate(instr[2], labels);

        return opcode << 27 | fn << 26 | reg << 23 | (addr & 0x7FFFFF);
    }

    static word_t parse_stk_instr(const parse_details& instr, const label_table&)
    {
        word_t opcode = parse_opcode(instr["opcode"]);
        word_t reg = parse_gp_reg(instr[1]);

        return opcode << 27 | reg << 23;
    }

    static word_t parse_jmp_instr(const parse_details& instr, address32_t address, const label_table& labels)
    {
        word_t opcode = parse_opcode(instr["opcode"]);
        word_t condition = parse_condition(instr["condition"]);
        word_t fn = instr[2] != "gp_reg";
        word_t addr = fn ? parse_immediate(instr[2], labels) : parse_gp_reg(instr[2]) << 17;

        if (instr["opcode"].contents == "JR")
            addr -= address + 4;

        return opcode << 27 | fn << 26 | condition << 22 | (addr & 0xFFFFF);
    }

    static word_t parse_ret_instr(const parse_details& instr, const label_table&)
    {
        word_t opcode = parse_opcode(instr["opcode"]);
        word_t condition = parse_condition(instr["condition"]);
        word_t retcode = parse_retcode(instr["opcode"]);

        return opcode << 27 | condition << 22 | retcode;
    }

    static void parse_dat_instr(const parse_details& instr, address32_t address, second_pass_result& mcode)
    {
        auto data_size = data_instr_size(instr["opcode"].contents);
        auto data_count = instr["def_list"].children.size();
        auto& data_values = instr["def_list"].children;

        small_byte_vector bytes;
        for (auto i = 0u; i < data_count; ++i) {
            auto value = parse_num_const(data_values[i]);
            fill_bytes(value, data_size, bytes);

            if (bytes.size() == 4) {
                mcode[address] = flush_bytes(bytes);
                address += 4;
            }
        }

        mcode[address] = flush_bytes(bytes);
    }
}

std::string frisc_assembler::read_file_(std::string filename)
{
    std::ifstream ifs(filename.c_str());
    std::string contents((std::istreambuf_iterator<char>(ifs)),
                         (std::istreambuf_iterator<char>()));
    return contents;
}

std::vector<parse_details> frisc_assembler::parse_lines_(std::string_view contents)
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

first_pass_result frisc_assembler::first_pass_(const std::vector<parse_details>& parsed_lines)
{
    std::vector<line_info> results;
    label_table labels;

    address32_t current_addr = 0;

    for (const auto& line : parsed_lines) {
        auto& line_instr = line["line_instr"];

        if (line_instr.empty()) {
            if (!line["line_label"].empty())
                labels[line["line_label"].contents] = current_addr;

            results.push_back({ current_addr, line.contents, {}, false });
            continue;
        }

        auto& instr_class = line_instr[0];

        if (instr_class[0] == "equ_instr") {
            labels[line["line_label"].contents] = detail::parse_num_const(instr_class[0]["signed_num_const"]);
            results.push_back({ current_addr, line.contents, {}, false });
            continue;
        } else if (instr_class[0] == "org_instr") {
            address32_t new_addr = detail::parse_num_const(instr_class[0]["signed_num_const"]);

            if (new_addr != cbu::address_rounded(new_addr, 4))
                throw logical_error(fmt::format("Impossible origin (ORG {}), not rounded to a multiple of 4.", new_addr));

            if (new_addr < current_addr)
                throw logical_error(fmt::format("Impossible origin (ORG {}), lesser than current address (= {}).", new_addr, current_addr));

            current_addr = new_addr;
        }

        if (!line["line_label"].empty())
            labels[line["line_label"].contents] = current_addr;

        results.push_back({ current_addr, line.contents, instr_class[0], (instr_class == "reg_instr" || instr_class[0] == "dat_instr") });

        if (instr_class == "reg_instr") {
            current_addr += 4;
        } else if (instr_class[0] == "dsp_instr") {
            address32_t offset = detail::parse_num_const(instr_class[0]["signed_num_const"]);
            current_addr += offset;
        } else if (instr_class[0] == "dat_instr") {
            auto& dat_instr = instr_class[0];
            auto& def_list = dat_instr["def_list"];
            auto data_size = detail::data_instr_size(dat_instr["opcode"].contents);
            auto data_count = def_list.children.size();

            for (auto i = 4u; i < data_size * data_count; i += 4)
                results.push_back({ current_addr + i, "", {}, true });

            current_addr += cbu::address_rounded(data_count * data_size + sizeof(word_t) - 1, sizeof(word_t));
        }
    }

    return { results, labels };
}

second_pass_result frisc_assembler::second_pass_(const std::vector<line_info>& lines, const label_table& labels)
{
    second_pass_result mcode;

    for (const auto [address, _, instr, is_instr] : lines) {
        if (!is_instr || instr.empty()) continue;

        try {
            if (instr == "dat_instr")
                detail::parse_dat_instr(instr, address, mcode);
            else if (instr == "alu_instr")
                mcode[address] = detail::parse_alu_instr(instr, labels);
            else if (instr == "cmp_instr")
                mcode[address] = detail::parse_cmp_instr(instr, labels);
            else if (instr == "mov_instr")
                mcode[address] = detail::parse_mov_instr(instr, labels);
            else if (instr == "mem_instr")
                mcode[address] = detail::parse_mem_instr(instr, labels);
            else if (instr == "stk_instr")
                mcode[address] = detail::parse_stk_instr(instr, labels);
            else if (instr == "jmp_instr")
                mcode[address] = detail::parse_jmp_instr(instr, address, labels);
            else if (instr == "ret_instr")
                mcode[address] = detail::parse_ret_instr(instr, labels);
        } catch (std::exception& e) {
            fmt::print_colored(fmt::RED, "Error in second pass at line {} :: {}\n", address, e.what());
        }
    }

    return mcode;
}

void frisc_assembler::assemble(std::string filename)
{
    try {
        auto contents = read_file_(filename);
        auto parsed_lines = parse_lines_(contents);
        auto [lines, labels] = first_pass_(parsed_lines);
        auto mcodes = second_pass_(lines, labels);

        auto outfile = util::open_file(util::change_extension(filename, "p").c_str(), "w+");

        for (const auto [address, contents, instr, is_instr] : lines) {
            if (!is_instr) {
                fmt::print(outfile, "                       {}\n", contents);
            } else {
                auto mcode = mcodes[address];
                fmt::print(outfile,
                    "{:08X}  {:02X} {:02X} {:02X} {:02X}  {}\n", address,
                    cbu::nth_byte_of(mcode, 0), cbu::nth_byte_of(mcode, 1), cbu::nth_byte_of(mcode, 2), cbu::nth_byte_of(mcode, 3),
                    contents
                );
            }
        }

        util::close_file(outfile);

    } catch (std::exception& e) {
        fmt::print_colored(stderr, fmt::RED, "{}\n", e.what());
        std::exit(1);
    }
}

void frisc_assembler::init_parser_()
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
                     ilit("NE")  / ilit("C")   / ilit("V")   / ilit("N")   /
                     ilit("Z")   / ilit("M")   / ilit("P")   / ilit("EQ");

    auto condition = opt(undersc >> cond_flag) >= "condition";

    auto def_item = (signed_num_const >> cm_sep) / signed_num_const;
    auto def_list = def_item + N >= "def_list";

    auto alu_opcode = ilit("ADD") / ilit("ADC") / ilit("SUB") / ilit("SBC") / ilit("AND") / ilit("OR")  / ilit("XOR") /
                      ilit("SHL") / ilit("SHR") / ilit("ASHR") / ilit("ROTL") / ilit("ROTR")                         >= "opcode";
    auto cmp_opcode = ilit("CMP")                                                                                    >= "opcode";
    auto mov_opcode = ilit("MOVE")                                                                                   >= "opcode";
    auto mem_opcode = ilit("LOADH") / ilit("LOADB") / ilit("LOAD") / ilit("STOREH") / ilit("STOREB") / ilit("STORE") >= "opcode";
    auto stk_opcode = ilit("PUSH") / ilit("POP")                                                                     >= "opcode";
    auto jmp_opcode = ilit("JP") / ilit("JR") / ilit("CALL")                                                         >= "opcode";
    auto ret_opcode = ilit("RETI") / ilit("RETN") / ilit("RET") / ilit("HALT")                                       >= "opcode";

    auto org_opcode = ilit("ORG")                          >= "opcode";
    auto equ_opcode = ilit("EQU")                          >= "opcode";
    auto dsp_opcode = ilit("DS")                           >= "opcode";
    auto dat_opcode = ilit("DW") / ilit("DH") / ilit("DB") >= "opcode";
    auto end_opcode = ilit("END")                          >= "opcode";

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
    auto end_instr = end_opcode                        >= "end_instr";

    auto reg_instr = alu_instr / cmp_instr / mem_instr / mov_instr / stk_instr / jmp_instr / ret_instr >= "reg_instr";
    auto psd_instr = equ_instr / org_instr / dsp_instr / dat_instr / end_instr                         >= "psd_instr";
    auto any_instr = psd_instr / reg_instr;

    auto line_label   = label / eps()                            >= "line_label";
    auto line_comment = (semicol >> (~eol >> any()) * N) / eps() >= "line_comment";
    auto line_instr   = any_instr / eps()                        >= "line_instr";

    auto line = line_label >> spaces_ >> line_instr >> spaces_ >> line_comment >> spaces_ >> eof >= "line";

    parser_ = line;
}

int main(int, char* argv[]) {
    frisc_assembler fasm;
    fasm.assemble(argv[1]);
}
