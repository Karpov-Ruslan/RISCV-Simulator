#include "instruction.hpp"
#include <hart.hpp>

namespace RISCVS::InstructionSet {

using SRegister = std::make_signed_t<Register::RegisterType>;
using URegister = std::make_unsigned_t<Register::RegisterType>;

using SImmediate = std::make_signed_t<Immediate>;
using UImmediate = std::make_unsigned_t<Immediate>;

using RegIdx = Hart::RegisterIndex;

bool Add(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] + hart[rs2];
    return true;
}

bool Sub(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] - hart[rs2];
    return true;
}

bool Xor(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] ^ hart[rs2];
    return true;
}

bool Or(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] | hart[rs2];
    return true;
}

bool And(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] & hart[rs2];
    return true;
}

bool Sll(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] << hart[rs2];
    return true;
}

bool Srl(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] >> hart[rs2];
    return true;
}

bool Sra(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = static_cast<SRegister>(hart[rs1]) >> hart[rs2];
    return true;
}

bool Slt(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = (static_cast<SRegister>(hart[rs1]) < static_cast<SRegister>(hart[rs2])) ? 1 : 0;
    return true;
}

bool Sltu(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = (static_cast<URegister>(hart[rs1]) < static_cast<URegister>(hart[rs2])) ? 1 : 0;
    return true;
}

bool AddI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);

    std::cerr << "rs1: " << rs1 << ", imm: " << imm << '\n';

    hart[rd] = hart[rs1] + imm;
    return true;
}

bool XorI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart[rs1] ^ imm;
    return true;
}

bool OrI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart[rs1] | imm;
    return true;
}

bool AndI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart[rs1] & imm;
    return true;
}

bool SllI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart[rs1] << (imm & 0b11111U);
    return true;
}

bool SrlI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart[rs1] >> (imm & 0b11111U);
    return true;
}

bool SraI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = static_cast<SRegister>(hart[rs1]) >> (imm & 0b11111U);
    return true;
}

bool SltI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = (static_cast<SRegister>(hart[rs1]) < static_cast<SImmediate>(imm)) ? 1 : 0;
    return true;
}

bool SltIU(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = (static_cast<URegister>(hart[rs1]) < static_cast<UImmediate>(imm)) ? 1 : 0;
    return true;
}

bool Lb(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart.Load(hart[rs1] + imm);
    return true;
}

bool Lh(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart.Load(hart[rs1] + imm); //FUCK: replace with bitwise bit setting: only lower part
    return true;
}

bool Lw(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart.Load(hart[rs1] + imm);
    return true;
}

bool Lbu(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart.Load(hart[rs1] + imm);
    return true;
}

bool Lhu(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart.Load(hart[rs1] + imm);
    return true;
}

bool Sb(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart.Store<Byte>(hart[rs1] + imm, (hart[rs2] & ((1 << (8*sizeof(Byte))) - 1)));
    return true;
}

bool Sh(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart.Store<Half>(hart[rs1] + imm, (hart[rs2] & ((1 << (8*sizeof(Half))) - 1)));
    return true;
}

bool Sw(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    std::cerr << "rs1: " << rs1 << ", rs2: " << rs2 << ", imm: " << imm << '\n';
    hart.Store<Word>(hart[rs1] + imm, hart[rs2]);
    // hart.Store<Word>(hart[rs1] + imm, (hart[rs2] & ((1 << (8*sizeof(Word))) - 1)));
    return true;
}

bool Beq(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    if (hart[rs1] == hart[rs2]) {
        hart.MovePC(imm);
    }
    return false;
}

bool Bne(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    if (hart[rs1] != hart[rs2]) {
        hart.MovePC(imm);
    }
    return false;
}

bool Blt(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    if (static_cast<SRegister>(hart[rs1]) < static_cast<SRegister>(hart[rs2])) {
        hart.MovePC(imm);
    }
    return false;
}

bool Bge(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    if (static_cast<SRegister>(hart[rs1]) >= static_cast<SRegister>(hart[rs2])) {
        hart.MovePC(imm);
    }
    return false;
}

bool BltU(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    if (hart[rs1] < hart[rs2]) {
        hart.MovePC(imm);
    }
    return false;
}

bool BgeU(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    if (hart[rs1] >= hart[rs2]) {
        hart.MovePC(imm);
    }
    return false;
}

bool Jal(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    Immediate imm = std::get<Immediate>(param2);
    hart[rd] = hart.GetPC() + 4;
    hart.MovePC(imm);
    return false;
}

bool Jalr(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart.GetPC() + 4;
    hart.SetPC(hart[rs1] + imm);
    return false;
}

bool Lui(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    Immediate imm = std::get<Immediate>(param2);
    hart[rd] = imm << 12;
    return true;
}

bool AuiPC(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    Immediate imm = std::get<Immediate>(param2);
    hart[rd] = hart.GetPC() + (imm << 12);
    return false;
}

bool ECall(FUNC_SIGNATURE) {
    // TODO: I donnow
    hart.Stop();
    return false;
}

bool EBreak(FUNC_SIGNATURE) {
    // TODO: I donnow
    hart.Stop();
    return false;
}


}