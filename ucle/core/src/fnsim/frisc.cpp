#include <fnsim/frisc.hpp>

#include <common/types.hpp>

#include <iostream>


ucle::status_t ucle::fnsim::frisc_simulator::execute_move_(word_t, bool fn, const reg<32>& IR)
{
    std::cout << "MOVE" << "\n";

    auto dest = IR[20] ? &regs_.SR : &regs_.R[IR[{25, 23}]];
    auto src = IR[21] ? regs_.SR.get() : (fn ? unop::sign_extend(IR[{19, 0}], 20) : regs_.R[IR[{19, 17}]].get());

    *dest = src;
    return success::ok;
}

ucle::status_t ucle::fnsim::frisc_simulator::execute_alu_(word_t opcode, bool fn, const reg<32>& IR)
{
    auto dest = &regs_.R[IR[{25, 23}]];
    auto src1 = regs_.R[IR[{22, 20}]].get();
    auto src2 = fn ? unop::sign_extend(IR[{19, 0}], 20) : regs_.R[IR[{19, 17}]].get();

    switch (opcode) {
        case 0b00001:
            std::cout << "OR" << "\n";
            // auto [res, new_flags] = binop::op_or(src1, src2, old_flags);
            // dest = res;
            // flags = new_flags;
            break;
        case 0b00010:
            std::cout << "AND" << "\n";
            // auto [res, new_flags] = binop::op_and(src1, src2, old_flags);
            // dest = res;
            // flags = new_flags;
            break;
        case 0b00011:
            std::cout << "XOR" << "\n";
            // auto [res, new_flags] = binop::op_xor(src1, src2, old_flags);
            // dest = res;
            // flags = new_flags;
            break;
        case 0b00100:
            std::cout << "ADD" << "\n";
            // auto [res, new_flags] = binop::op_add(src1, src2, old_flags);
            // dest = res;
            // flags = new_flags;
            break;
        case 0b00101:
            std::cout << "ADC" << "\n";
            // auto [res, new_flags] = binop::op_adc(src1, src2, old_flags);
            // dest = res;
            // flags = new_flags;
            break;
        case 0b00110:
            std::cout << "SUB" << "\n";
            // auto [res, new_flags] = binop::op_sub(src1, src2, old_flags);
            // dest = res;
            // flags = new_flags;
            break;
        case 0b00111:
            std::cout << "SBC" << "\n";
            // auto [res, new_flags] = binop::op_sbc(src1, src2, old_flags);
            // dest = res;
            // flags = new_flags;
            break;
        case 0b01000:
            std::cout << "ROTL" << "\n";
            // auto [res, new_flags] = binop::op_rtl(src1, src2, old_flags);
            // dest = res;
            // flags = new_flags;
            break;
        case 0b01001:
            std::cout << "ROTR" << "\n";
            // auto [res, new_flags] = binop::op_rtr(src1, src2, old_flags);
            // dest = res;
            // flags = new_flags;
            break;
        case 0b01010:
            std::cout << "SHL" << "\n";
            // auto [res, new_flags] = binop::op_shl(src1, src2, old_flags);
            // dest = res;
            // flags = new_flags;
            break;
        case 0b01011:
            std::cout << "SHR" << "\n";
            // auto [res, new_flags] = binop::op_shr(src1, src2, old_flags);
            // dest = res;
            // flags = new_flags;
            break;
        case 0b01100:
            std::cout << "ASHR" << "\n";
            // auto [res, new_flags] = binop::op_asr(src1, src2, old_flags);
            // dest = res;
            // flags = new_flags;
            break;
        case 0b01101:
            std::cout << "CMP" << "\n";
            // auto [_, new_flags] = binop::op_asr(src1, src2, old_flags);
            // flags = new_flags;
            break;
        default:
            return error::invalid_instruction;
    }

    return success::ok;
}

ucle::status_t ucle::fnsim::frisc_simulator::execute_mem_(word_t opcode, bool fn, const reg<32>& IR)
{
    auto& reg = regs_.R[IR[{25, 23}]];
    auto addr = unop::sign_extend(IR[{19, 0}], 20) + (fn ? regs_.R[IR[{22, 20}]].get() : 0);

    switch (opcode) {
        case 0b10000:
            std::cout << "POP" << "\n";
            reg = read_word_(regs_.SP.get());
            regs_.SP += 4;
            break;
        case 0b10001:
            std::cout << "PUSH" << "\n";
            regs_.SP -= 4;
            write_word_(regs_.SP.get(), reg.get());
            break;
        case 0b10010:
            std::cout << "LOADB" << "\n";
            reg = read_byte_(addr);
            break;
        case 0b10011:
            std::cout << "STOREB" << "\n";
            write_byte_(addr, reg.get());  // TODO: get_byte?
            break;
        case 0b10100:
            std::cout << "LOADH" << "\n";
            reg = read_half_(addr);
            break;
        case 0b10101:
            std::cout << "STOREH" << "\n";
            write_half_(addr, reg.get());  // TODO: get_half?
            break;
        case 0b10110:
            std::cout << "LOAD" << "\n";
            reg = read_word_(addr);
            break;
        case 0b10111:
            std::cout << "STORE" << "\n";
            write_word_(addr, reg.get());
            break;
    }

    return success::ok;
}

ucle::status_t ucle::fnsim::frisc_simulator::execute_ctrl_(word_t opcode, bool fn, const reg<32>& IR)
{
    if (!eval_cond_(IR[{25, 22}]))
        return success::ok;

    auto addr = fn ? unop::sign_extend(IR[{19, 0}], 20) : regs_.R[IR[{19, 17}]].get();

    switch (opcode) {
        case 0b11000:
            std::cout << "JP" << "\n";
            regs_.PC = addr;
            break;
        case 0b11001:
            std::cout << "CALL" << "\n";
            regs_.SP -= 4;
            write_word_(regs_.SP.get(), regs_.PC.get());
            regs_.PC = addr;
            break;
        case 0b11010:
            std::cout << "JR" << "\n";
            regs_.SP += addr;
            break;
        case 0b11011: {
            std::cout << "RETX" << "\n";
            auto rtcode = IR[{1, 0}];
            regs_.PC = read_word_(regs_.SP.get());
            regs_.SP += 4;
            // if (rtcode == 0b01)      /* RETI */
            //      /* set GIE = 1 */
            // else if (rtcode == 0b11) /* RETN */
            //      /* set IIF = 1 */
            break;
        }
        case 0b11111:
            std::cout << "HALT" << "\n";
            terminate_();
            break;
        default:
            return error::invalid_instruction;
    }

    return success::ok;
}

constexpr bool ucle::fnsim::frisc_simulator::eval_cond_(word_t) const
{
    return true;
}

ucle::status_t ucle::fnsim::frisc_simulator::execute_single_() {
    reg<32> IR = read_word_(address_t(regs_.PC));
    std::cout << regs_.PC.get() << ": ";

    auto opcode = IR[{31, 27}];
    auto fn = IR[26];

    status_t stat;
    if (opcode == 0b00000) {
        stat = execute_move_(opcode, fn, IR);
    } else if (opcode >= 0b00001 && opcode <= 0b01101) {
        stat = execute_alu_(opcode, fn, IR);
    } else if (opcode >= 0b10000 && opcode <= 0b10111) {
        stat = execute_mem_(opcode, fn, IR);
    } else if (opcode >= 0b11000 && opcode <= 0b11111) {
        stat = execute_ctrl_(opcode, fn, IR);
    } else {
        std::cout << "Unknown!" << "\n";
        return error::invalid_instruction;
    }

    regs_.PC += 4;

    return stat;
}

/* Test */
int main(int, char* argv[]) {
    ucle::fnsim::simulator_config cfg{.memory_size = 0x1000};
    ucle::fnsim::frisc_simulator fs(cfg);
    fs.load_pfile(argv[1]);
    fs.run();
}
