#include "instruction.hpp"
#include <hart.hpp>

namespace RISCVS::InstructionSet {

using Register = Hart::Register;
using RegIdx = Hart::RegisterIndex;

void Add(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] + hart[rs2];
    hart.NextInstructionPC();
}

void Sub(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] - hart[rs2];
    hart.NextInstructionPC();
}

void AddI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart[rs1] + imm;
    hart.NextInstructionPC();
}

}