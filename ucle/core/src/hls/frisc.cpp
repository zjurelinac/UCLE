//******************************************************************************
//  FRISC HLS v0.1.0
//
//  Desired features in productin version:
//      - register<size> type
//      - signed_cast
//      - fixed-size types (int16, int32...)
//      - uniform HLS interface
//      - header file
//      - OO inheritance
//      - binary util library
//      - info and status queries
//      - policy-based execution?
//******************************************************************************

#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>

#include "clargs.h"

using byte = unsigned char;
using half = unsigned short;
using word = unsigned int;
using longword = unsigned long long;
using size_t = unsigned long;

using cv_flags = std::pair<bool, bool>;

cv_flags calc_add_flags(word src1, word src2);
bool evaluate_condition(unsigned cond, bool Z, bool V, bool C, bool N);
word sign_extend20(word val);

enum class hls_state { initialized, loaded, running, stopped, terminated, exception };

class frisc_hls {
public:
    frisc_hls(size_t mem_size) : _state(hls_state::initialized), _mem_size(mem_size)
        { _memory = new byte[mem_size]; memset(R, 0, sizeof R); PC = SR = 0; IIF = true; }
    ~frisc_hls() { delete[] _memory; }

    void load_program(const char* filename);
    bool run(bool trace = false);
    void reset();

    void print_reg_state();

    byte get_byte(word address)
        { return _memory[address]; }
    half get_half(word address)
        { half h = 0; for (int i = 0; i < 2; ++i) { h = (h << 8) | _memory[address + 1 - i]; } return h; }
    word get_word(word address)
        { word w = 0; for (int i = 0; i < 4; ++i) { w = (w << 8) | _memory[address + 3 - i]; } return w; }

    void set_byte(word address, byte value)
        { _memory[address] = value; }
    void set_half(word address, half value)
        { for (int i = 0; i < 2; ++i) _memory[address + i] = (value & (0xFF << (8*i))) >> (8*i); }
    void set_word(word address, word value)
        { for (int i = 0; i < 4; ++i) _memory[address + i] = (value & (0xFF << (8*i))) >> (8*i); }

    // Register definitions
    word R[8];        // R0-R7 (General-purpose)
    word &SP = R[7];  // R7 == SP (Stack Pointer)
    word PC;          // Program Counter
    word SR;          // Status Register

    bool IIF;           // Internal Interrupt Flag
private:
    void _execute_single();
    void _check_interrupt();
    void _do_interrupt();

    hls_state _state;
    size_t _mem_size;
    byte *_memory;
};

void frisc_hls::load_program(const char* filename) {
    std::ifstream pfile(filename);

    size_t address;
    unsigned word[4] = {0};

    std::string line;
    while (!pfile.eof()) {
        std::getline(pfile, line);

        if (sscanf(line.c_str(), "%8zX %2X %2X %2X %2X", &address, &word[0], &word[1], &word[2], &word[3]) == 5) {
            printf("%08zX %02X %02X %02X %02X\n", address, word[0], word[1], word[2], word[3]);
            for (size_t i = 0; i < 4; ++i) _memory[address + i] = word[i];
        }
    }

    _state = hls_state::loaded;
}

bool frisc_hls::run(bool trace) {
    if (_state != hls_state::loaded || _state != hls_state::stopped)
        return false;

    _state = hls_state::running;

    do {
        _execute_single();
        if (trace) print_reg_state();
    } while (_state == hls_state::running);

    return (_state == hls_state::terminated) || (_state == hls_state::stopped);
}

void frisc_hls::print_reg_state() {
    puts("------------");
    for (int i = 0; i < 8; ++i)
        printf("R%d: %08X\n", i, R[i]);
    printf("\nPC: %08X\nSR: %08X\nIIF = %d\n", PC, SR, IIF);
    puts("------------");
}

void frisc_hls::_execute_single() {
    word instr = get_word(PC);

    unsigned opcode = (instr & 0xF8000000) >> 27;
    unsigned fn = (instr & (1 << 26)) >> 26;

    printf("%08X: Executing %08X = %08X %0X, operands = %08X %08X\n", PC, instr, opcode, fn);

    PC += 4;

    // MOVE operation
    if (opcode == 0b00000) {
        word &dest = R[(instr & (0b111 << 23)) >> 23];
        word src = fn ? sign_extend20(instr & 0x000FFFFF) : R[(instr & (0b111 << 17)) >> 17];

        bool ssr = instr & (1 << 21);
        bool dsr = instr & (1 << 20);

        if (ssr) src = SR;
        if (dsr) dest = SR;

        dest = src;

    // ALU operation
    } else if (opcode >= 0b00001 && opcode <= 0b01100) {
        word &dest = R[(instr & (0b111 << 23)) >> 23];
        word src1 = R[(instr & (0b111 << 20)) >> 20];
        word src2 = fn ? sign_extend20(instr & 0x000FFFFF) : R[(instr & (0b111 << 17)) >> 17];

        bool N = SR & 1, C = SR & 2, V = SR & 4, Z = SR & 8;

        switch(opcode) {
            default: {
                fprintf(stderr, "Unknown operation: %04X!\n", opcode);
                _state = hls_state::exception;
                break;
            } case 0b00001: {  // OR
                dest = src1 | src2;
                C = V = 0;
                break;
            } case 0b00010: {  // AND
                dest = src1 & src2;
                C = V = 0;
                break;
            } case 0b00011: {  // XOR
                dest = src1 ^ src2;
                C = V = 0;
                break;
            } case 0b00100: { // ADD
                dest = src1 + src2;
                auto cv = calc_add_flags(src1, src2);
                C = cv.first; V = cv.second;
                break;
            } case 0b00101: {  // ADC
                dest = src1 + src2 + C;
                auto cv = calc_add_flags(src1, src2 + C);
                C = cv.first; V = cv.second;
                break;
            } case 0b00110: {  // SUB
                dest = src1 + ~src2;
                auto cv = calc_add_flags(src1, ~src2);
                C = cv.first; V = cv.second;
                break;
            } case 0b00111: {  // SBC - TODO: Check validity of assumptions
                dest = src1 + ~(src2 + C);
                auto cv = calc_add_flags(src1, ~(src2 + C));
                C = cv.first; V = cv.second;
                break;
            } case 0b01000: {  // ROTL - TODO: Implement
                src2 &= 0x1F;  // (% 32)
                int wrap_bits = 32 - src2;
                word wrap_mask = (1 << wrap_bits) - 1;
                // dest = (src1 << src2) | ();
                // C =
                V = 0;
                break;
            } case 0b01001: {  // ROTR - TODO: Implement
                src2 &= 0x1F;  // (% 32)
                int wrap_bits = 32 - src2;
                word wrap_mask = (1 << wrap_bits) - 1;

                // C =
                V = 0;
                break;
            } case 0b01010: {  // SHL
                dest = src1 << src2;
                // C =
                V = 0;
                break;
            } case 0b01011: {  // SHR
                dest = src1 >> src2;
                V = 0;
                break;
            } case 0b01100: {  // ASHR
                dest = ((signed) src1) >> src2;
                V = 0;
                break;
            } case 0b01101: {  // CMP
                unsigned res = src1 - src2;
                auto cv = calc_add_flags(src1, ~src2);
                C = cv.first; V = cv.second;
                N = res & (1UL << 31);
                Z = res == 0;
                break;
            }
        }

        if (opcode != 0b01101) {  // If not a CMP (CMP sets all flags on its own)
            N = dest & (1UL << 31);
            Z = dest == 0;
        }

        SR &= ~0b1111;  // Clear lower 4 bits
        SR |= (Z << 3) | (V << 2) | (C << 1) | N;

    // Memory operation
    } else if (opcode >= 0b10010 && opcode <= 0b10111) {
        unsigned &reg = R[(instr & (0b111 << 23)) >> 23];
        unsigned adr_reg = R[(instr & (0b111 << 20)) >> 20];
        unsigned addr = sign_extend20(instr & 0x000FFFFF) + (fn ? adr_reg : 0);

        switch (opcode) {
            default:
                fprintf(stderr, "Unknown operation: %04X!\n", opcode);
                _state = hls_state::exception;
                break;
            case 0b10000:  // POP
                reg = get_word(SP);
                SP += 4;
                break;
            case 0b10001:  // PUSH
                SP -= 4;
                set_word(SP, reg);
                break;
            case 0b10010:  // LOADB
                reg = get_byte(addr);
                break;
            case 0b11011:  // STOREB
                set_byte(addr, reg & 0xFF);
                break;
            case 0b10100:  // LOADH
                addr &= ~1;
                reg = get_half(addr);
                break;
            case 0b10101:  // STOREH
                addr &= ~1;
                set_half(addr, reg & 0xFFFF);
                break;
            case 0b10110:  // LOAD
                addr &= ~3;
                reg = get_word(addr);
                break;
            case 0b10111:  // STORE
                addr &= ~3;
                set_word(addr, reg);
                break;
        }

    // Control operation
    } else if (opcode) {
        unsigned cond = (instr & (0b1111 << 22)) >> 22;

        switch (opcode) {
            default:
                fprintf(stderr, "Unknown operation: %04X!\n", opcode);
                _state = hls_state::exception;
                break;
            case 0b11000:  // JP
                //
                break;
            case 0b11001:  // CALL
                //
                break;
            case 0b11010:  // JR
                //
                break;
            case 0b11011:  // RET[I/N]
                //
                break;
            case 0b11111:  // HALT
                _state = hls_state::terminated;
                break;
        }
    }

    // test for interrupts
    // if any, process them
}

int main(int, char* argv[]) {
    frisc_hls simulator(1024);
    simulator.load_program(argv[1]);
    std::cout << simulator.run(true) << "\n";
}

cv_flags calc_add_flags(word src1, word src2) {
    bool C = (((longword) src1) + src2) & (1ULL << 32);
    bool V = C ^ (((src1 & 0x7FFFFFFF) + (src2 & 0x7FFFFFFF)) & (1U << 31));
    return {C, V};
}

bool evaluate_condition(unsigned cond, bool Z, bool V, bool C, bool N) {
    switch (cond) {
        default:
            fprintf(stderr, "Unknown condition code: %d!\n", cond);
            return false;
        case 0b0000: return true;           //  $       = Unconditional
        case 0b0001: return  N;             //  N, M    = Negative, Minus
        case 0b0010: return !N;             //  NN, P   = Non-Negative, Plus
        case 0b0011: return  C;             //  C, UGE  = Carry, Unsigned-Greater-Equal
        case 0b0100: return !C;             //  NC, ULT = Not-Carry, Unsigned-Less-Than
        case 0b0101: return  V;             //  V       = oVerflow
        case 0b0110: return !V;             //  NV      = Not-oVerflow
        case 0b0111: return  Z;             //  Z, EQ   = Zero, Equals
        case 0b1000: return !Z;             //  NZ, NE  = Not-Zero, Not-Equals
        case 0b1001: return !C ||  Z;       //  ULE     = Unsigned-Less-Equal
        case 0b1010: return  C && !Z;       //  UGT     = Unsigned-Greater-Than
        case 0b1011: return N ^ V;          //  SLT     = Signed-Less-Than
        case 0b1100: return (N ^ V) || Z;   //  SLE     = Signed-Less-Equal
        case 0b1101: return !(N ^ V);       //  SGE     = Signed-Greater-Equal
        case 0b1110: return !(N ^ V) && !Z; //  SGT     = Signed-Greater-Than
    }
}

word sign_extend20(word val) {
    return (val & (1 << 19)) ? (val | 0xFFF00000) : (val & 0x000FFFFF);
}