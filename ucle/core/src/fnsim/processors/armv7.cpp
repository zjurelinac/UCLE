#include <fnsim/processors/armv7.hpp>

#include <memory>

namespace fnsim = ucle::fnsim;
namespace armv7 = ucle::fnsim::armv7;



constexpr bool armv7::armv7_simulator::eval_cond_(word_t cond) const
{
    auto& CPSR = regs_.CPSR;
    switch (cond) {
        default:     return false;
        case 0b0000: return  CPSR.Z;            // Equal
        case 0b0001: return !CPSR.Z;            // Not equal
        case 0b0010: return  CPSR.C;            // Carry set / unsigned higher or same
        case 0b0011: return !CPSR.C;            // Carry clear / unsigned lower
        case 0b0100: return  CPSR.N;            // Minus / negative
        case 0b0101: return !CPSR.N;            // Plus / positive or zero
        case 0b0110: return  CPSR.V;            // Overflow
        case 0b0111: return !CPSR.V;            // No overflow
        case 0b1000: return  CPSR.C && !CPSR.Z; // Unsigned higher
        case 0b1001: return !CPSR.C ||  CPSR.Z; // Unsigned lower or same
        case 0b1010: return  CPSR.N ==  CPSR.V; // Signed greater than or equal
        case 0b1011: return  CPSR.N !=  CPSR.V; // Signed less than
        case 0b1100: return !CPSR.Z && (CPSR.N == CPSR.V); // Signed greater than
        case 0b1101: return  CPSR.Z || (CPSR.N != CPSR.V); // Signed less than or equal
        case 0b1110: return true;               // Always (unconditional)
        case 0b1111: return true;               // Additional unconditional instructions
    }
}

fnsim::functional_processor_simulator_ptr<32> armv7::make_armv7_simulator(fnsim::processor_config cfg)
{
    return std::make_unique<armv7_simulator>(cfg);
}
