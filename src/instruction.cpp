#include "instruction.hpp"
#include <hart.hpp>

namespace RISCVS::InstructionSet {

using Register = Hart::Register;
using SRegister = std::make_signed_t<Register>;
using URegister = std::make_unsigned_t<Register>;

using SImmediate = std::make_signed_t<Immediate>;
using UImmediate = std::make_unsigned_t<Immediate>;

using RegIdx = Hart::RegisterIndex;

void Add(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] + hart[rs2];
}

void Sub(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] - hart[rs2];
}

void Xor(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] ^ hart[rs2];
}

void Or(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] | hart[rs2];
}

void And(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] & hart[rs2];
}

void Sll(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] << hart[rs2];
}

void Srl(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] >> hart[rs2];
}

void Sra(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = static_cast<Register>(static_cast<SRegister>(hart[rs1]) >> hart[rs2]);
}

void Slt(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = (static_cast<SRegister>(hart[rs1]) < static_cast<SRegister>(hart[rs2])) ? 1 : 0;
}

void Sltu(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = (static_cast<URegister>(hart[rs1]) < static_cast<URegister>(hart[rs2])) ? 1 : 0;
}

void AddI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart[rs1] + imm;
}

void XorI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart[rs1] ^ imm;
}

void OrI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart[rs1] | imm;
}

void AndI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart[rs1] & imm;
}

void SllI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart[rs1] << (imm & 0b11111U);
}

void SrlI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart[rs1] >> (imm & 0b11111U);
}

void SraI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = static_cast<Register>(static_cast<SRegister>(hart[rs1]) >> (imm & 0b11111U));
}

void SltI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = (static_cast<SRegister>(hart[rs1]) < static_cast<SImmediate>(imm)) ? 1 : 0;
}

void SltIU(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = (static_cast<URegister>(hart[rs1]) < static_cast<UImmediate>(imm)) ? 1 : 0;
}

void Lb(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = static_cast<UByte>(hart.Load<Byte>(hart[rs1] + imm));
}

void Lh(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = static_cast<UHalf>(hart.Load<Half>(hart[rs1] + imm));
}

void Lw(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = static_cast<UWord>(hart.Load<Word>(hart[rs1] + imm));
}

void Lbu(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart.Load<UByte>(hart[rs1] + imm);
}

void Lhu(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart.Load<UHalf>(hart[rs1] + imm);
}

void Sb(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart.Store<Byte>(hart[rs1] + imm, (hart[rs2] & ((1 << (8*sizeof(Byte))) - 1)));
}

void Sh(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart.Store<Half>(hart[rs1] + imm, (hart[rs2] & ((1 << (8*sizeof(Half))) - 1)));
}

void Sw(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart.Store<Word>(hart[rs1] + imm, (hart[rs2] & ((1 << (8*sizeof(Word))) - 1)));
}

void Beq(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    if (hart[rs1] == hart[rs2]) {
        hart.MovePC(imm);
    }
}

void Bne(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    if (hart[rs1] != hart[rs2]) {
        hart.MovePC(imm);
    }
}

void Blt(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    if (static_cast<SRegister>(hart[rs1]) < static_cast<SRegister>(hart[rs2])) {
        hart.MovePC(imm);
    }
}

void Bge(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    if (static_cast<SRegister>(hart[rs1]) >= static_cast<SRegister>(hart[rs2])) {
        hart.MovePC(imm);
    }
}

void BltU(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    if (hart[rs1] < hart[rs2]) {
        hart.MovePC(imm);
    }
}

void BgeU(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    if (hart[rs1] >= hart[rs2]) {
        hart.MovePC(imm);
    }
}

void Jal(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    Immediate imm = std::get<Immediate>(param2);
    hart[rd] = hart.GetPC() + 4;
    hart.MovePC(imm);
}

void Jalr(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart.GetPC() + 4;
    hart.SetPC(hart[rs1] + imm);
}

void Lui(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    Immediate imm = std::get<Immediate>(param2);
    hart[rd] = imm << 12;
}

void AuiPC(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    Immediate imm = std::get<Immediate>(param2);
    hart[rd] = hart.GetPC() + (imm << 12);
}

void ECall(FUNC_SIGNATURE) {
    // TODO: I donnow
}

void EBreak(FUNC_SIGNATURE) {
    // TODO: I donnow
}


}