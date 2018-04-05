#include <fnsim/processors/frisc.hpp>

#include <memory>

namespace fnsim = ucle::fnsim;

fnsim::status fnsim::frisc_simulator::execute_move_(word_t, bool fn, const reg<32>& IR)
{
    // std::cout << "MOVE" << "\n";

    auto src = IR[21] ? word_t(regs_.SR) : (fn ? unop::sign_extend(IR[{19, 0}], 20) : word_t(regs_.R[IR[{19, 17}]]));

    if (IR[20])
        regs_.SR = src;
    else
        regs_.R[IR[{25, 23}]] = src;

    return status::ok;
}

fnsim::status fnsim::frisc_simulator::execute_alu_(word_t opcode, bool fn, const reg<32>& IR)
{
    auto& dest = regs_.R[IR[{25, 23}]];
    auto src1 = word_t(regs_.R[IR[{22, 20}]]);
    auto src2 = fn ? unop::sign_extend(IR[{19, 0}], 20) : word_t(regs_.R[IR[{19, 17}]]);
    bool C = regs_.SR.C;
    frisc_arith_flags flags;

    switch (opcode) {
        case 0b00001: {
            // std::cout << "OR" << "\n";
            auto [res, new_flags] = binop::op_or(src1, src2);
            dest = res;
            flags = new_flags;
            break;
        } case 0b00010: {
            // std::cout << "AND" << "\n";
            auto [res, new_flags] = binop::op_and(src1, src2);
            dest = res;
            flags = new_flags;
            break;
        } case 0b00011: {
            // std::cout << "XOR" << "\n";
            auto [res, new_flags] = binop::op_xor(src1, src2);
            dest = res;
            flags = new_flags;
            break;
        } case 0b00100: {
            // std::cout << "ADD" << "\n";
            auto [res, new_flags] = binop::op_add(src1, src2);
            dest = res;
            flags = new_flags;
            break;
        } case 0b00101: {
            // std::cout << "ADC" << "\n";
            auto [res, new_flags] = binop::op_adc(src1, src2, C);
            dest = res;
            flags = new_flags;
            break;
        } case 0b00110: {
            // std::cout << "SUB" << "\n";
            auto [res, new_flags] = binop::op_sub(src1, src2);
            dest = res;
            flags = new_flags;
            break;
        } case 0b00111: {
            // std::cout << "SBC" << "\n";
            auto [res, new_flags] = binop::op_sbc(src1, src2, C);
            dest = res;
            flags = new_flags;
            break;
        } case 0b01000: {
            // std::cout << "ROTL" << "\n";
            auto [res, new_flags] = binop::op_rtl(src1, src2);
            dest = res;
            flags = new_flags;
            break;
        } case 0b01001: {
            // std::cout << "ROTR" << "\n";
            auto [res, new_flags] = binop::op_rtr(src1, src2);
            dest = res;
            flags = new_flags;
            break;
        } case 0b01010: {
            // std::cout << "SHL" << "\n";
            auto [res, new_flags] = binop::op_shl(src1, src2);
            dest = res;
            flags = new_flags;
            break;
        } case 0b01011: {
            // std::cout << "SHR" << "\n";
            auto [res, new_flags] = binop::op_shr(src1, src2);
            dest = res;
            flags = new_flags;
            break;
        } case 0b01100: {
            // std::cout << "ASHR" << "\n";
            auto [res, new_flags] = binop::op_asr(src1, src2);
            dest = res;
            flags = new_flags;
            break;
        } case 0b01101: {
            // std::cout << "CMP" << "\n";
            auto res = binop::op_sub(src1, src2);
            flags = res.second;
            break;
        } default:
            return status::invalid_instruction;
    }

    regs_.SR.Z = flags[3];
    regs_.SR.V = flags[2];
    regs_.SR.C = flags[1];
    regs_.SR.N = flags[0];

    return status::ok;
}

fnsim::status fnsim::frisc_simulator::execute_mem_(word_t opcode, bool fn, const reg<32>& IR)
{
    auto& reg = regs_.R[IR[{25, 23}]];
    auto addr = unop::sign_extend(IR[{19, 0}], 20) + (fn ? word_t(regs_.R[IR[{22, 20}]]) : 0);

    switch (opcode) {
        case 0b10000:
            // std::cout << "POP" << "\n";
            reg = read_<word_t>(regs_.SP);
            regs_.SP += 4;
            break;
        case 0b10001:
            // std::cout << "PUSH" << "\n";
            regs_.SP -= 4;
            write_<word_t>(regs_.SP, reg);
            break;
        case 0b10010:
            // std::cout << "LOADB" << "\n";
            reg = read_<byte_t>(addr);
            break;
        case 0b10011:
            // std::cout << "STOREB" << "\n";
            write_<byte_t>(addr, static_cast<byte_t>(reg));
            break;
        case 0b10100:
            // std::cout << "LOADH" << "\n";
            reg = read_<half_t>(addr);
            break;
        case 0b10101:
            // std::cout << "STOREH" << "\n";
            write_<half_t>(addr, static_cast<half_t>(reg));
            break;
        case 0b10110:
            // std::cout << "LOAD" << "\n";
            reg = read_<word_t>(addr);
            break;
        case 0b10111:
            // std::cout << "STORE" << "\n";
            write_<word_t>(addr, reg);
            break;
        default:
            return status::invalid_instruction;
    }

    return status::ok;
}

fnsim::status fnsim::frisc_simulator::execute_ctrl_(word_t opcode, bool fn, const reg<32>& IR)
{
    if (!eval_cond_(IR[{25, 22}]))
        return status::ok;

    auto addr = fn ? unop::sign_extend(IR[{19, 0}], 20) : word_t(regs_.R[IR[{19, 17}]]);

    switch (opcode) {
        case 0b11000:
            // std::cout << "JP" << "\n";
            regs_.PC = addr;
            break;
        case 0b11001:
            // std::cout << "CALL" << "\n";
            regs_.SP -= 4;
            write_<word_t>(regs_.SP, regs_.PC);
            regs_.PC = addr;
            break;
        case 0b11010:
            // std::cout << "JR" << "\n";
            regs_.SP += addr;
            break;
        case 0b11011: {
            // std::cout << "RETX" << "\n";
            auto rtcode = IR[{1, 0}];
            regs_.PC = read_<word_t>(regs_.SP);
            regs_.SP += 4;
            if (rtcode == 0b01)      /* RETI */
                regs_.SR.GIE = 1;
            else if (rtcode == 0b11) /* RETN */
                regs_.IIF = 1;
            break;
        }
        case 0b11111:
            // std::cout << "HALT" << "\n";
            terminate_();
            break;
        default:
            return status::invalid_instruction;
    }

    return status::ok;
}

constexpr bool fnsim::frisc_simulator::eval_cond_(word_t cond) const
{
    auto& SR = regs_.SR;
    switch (cond) {
        default:     return false;
        case 0b0000: return true;                       //  $       = Unconditional
        case 0b0001: return  SR.N;                      //  N, M    = Negative, Minus
        case 0b0010: return !SR.N;                      //  NN, P   = Non-Negative, Plus
        case 0b0011: return  SR.C;                      //  C, UGE  = Carry, Unsigned-Greater-Equal
        case 0b0100: return !SR.C;                      //  NC, ULT = Not-Carry, Unsigned-Less-Than
        case 0b0101: return  SR.V;                      //  V       = oVerflow
        case 0b0110: return !SR.V;                      //  NV      = Not-oVerflow
        case 0b0111: return  SR.Z;                      //  Z, EQ   = Zero, Equals
        case 0b1000: return !SR.Z;                      //  NZ, NE  = Not-Zero, Not-Equals
        case 0b1001: return !SR.C ||  SR.Z;             //  ULE     = Unsigned-Less-Equal
        case 0b1010: return  SR.C && !SR.Z;             //  UGT     = Unsigned-Greater-Than
        case 0b1011: return  SR.N ^ SR.V;               //  SLT     = Signed-Less-Than
        case 0b1100: return (SR.N ^ SR.V) || SR.Z;      //  SLE     = Signed-Less-Equal
        case 0b1101: return !(SR.N ^ SR.V);             //  SGE     = Signed-Greater-Equal
        case 0b1110: return !(SR.N ^ SR.V) && !SR.Z;    //  SGT     = Signed-Greater-Than
    }
}

fnsim::status fnsim::frisc_simulator::execute_single() {
    // std::cout << word_t(regs_.PC) << "\n";

    reg<32> IR = read_<word_t>(address_t(regs_.PC));
    regs_.PC += 4;

    auto opcode = IR[{31, 27}];
    auto fn = IR[26];

    status stat;
    if (opcode == 0b00000) {
        stat = execute_move_(opcode, fn, IR);
    } else if (opcode >= 0b00001 && opcode <= 0b01101) {
        stat = execute_alu_(opcode, fn, IR);
    } else if (opcode >= 0b10000 && opcode <= 0b10111) {
        stat = execute_mem_(opcode, fn, IR);
    } else if (opcode >= 0b11000 && opcode <= 0b11111) {
        stat = execute_ctrl_(opcode, fn, IR);
    } else {
        // std::cout << "Unknown!" << "\n";
        stat = status::invalid_instruction;
    }

    return stat;
}

fnsim::reg_info fnsim::frisc_simulator::get_reg_info()
{
    return {
        {"R0", regs_.R[0]},
        {"R1", regs_.R[1]},
        {"R2", regs_.R[2]},
        {"R3", regs_.R[3]},
        {"R4", regs_.R[4]},
        {"R5", regs_.R[5]},
        {"R6", regs_.R[6]},
        {"SP", regs_.SP},

        {"PC", regs_.PC},
        {"SR", regs_.SR},
        {"IIF", regs_.IIF},
    };
}

fnsim::functional_processor_simulator_ptr fnsim::make_frisc_simulator(fnsim::processor_config cfg)
{
    return std::make_unique<frisc_simulator>(cfg);
}