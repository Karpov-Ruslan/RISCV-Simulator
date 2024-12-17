#include "instruction.hpp"
#include <hart.hpp>

namespace RISCVS::InstructionSet {

using SRegister = std::make_signed_t<Register::RegisterType>;
using URegister = std::make_unsigned_t<Register::RegisterType>;

using SImmediate = std::make_signed_t<Immediate>;
using UImmediate = std::make_unsigned_t<Immediate>;

using RegIdx = Hart::RegisterIndex;

#define D(name, p1, p2, p3) std::cout << #name << '\n';
#define D(name, p1, p2, p3) 

bool Add(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] + hart[rs2];
    D(add, rd, rs1, rs2);
    return true;
}

bool Sub(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] - hart[rs2];
    D(sub, rd, rs1, rs2);
    return true;
}

bool Xor(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] ^ hart[rs2];
    D(xor, rd, rs1, rs2);
    return true;
}

bool Or(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] | hart[rs2];
    D(or, rd, rs1, rs2);
    return true;
}

bool And(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] & hart[rs2];
    D(and, rd, rs1, rs2);
    return true;
}

bool Sll(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] << hart[rs2];
    D(sll, rd, rs1, rs2);
    return true;
}

bool Srl(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = hart[rs1] >> hart[rs2];
    D(srl, rd, rs1, rs2);
    return true;
}

bool Sra(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = static_cast<SRegister>(hart[rs1]) >> hart[rs2];
    D(sra, rd, rs1, rs2);
    return true;
}

bool Slt(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = (static_cast<SRegister>(hart[rs1]) < static_cast<SRegister>(hart[rs2])) ? 1 : 0;
    D(slt, rd, rs1, rs2);
    return true;
}

bool Sltu(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    RegIdx rs2 = std::get<RegIdx>(param3);
    hart[rd] = (static_cast<URegister>(hart[rs1]) < static_cast<URegister>(hart[rs2])) ? 1 : 0;
    D(sltu, rd, rs1, rs2);
    return true;
}

bool AddI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart[rs1] + imm;
    D(addi, rd, rs1, rs2);
    return true;
}

bool XorI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart[rs1] ^ imm;
    D(xori, rd, rs1, rs2);
    return true;
}

bool OrI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart[rs1] | imm;
    D(ori, rd, rs1, rs2);
    return true;
}

bool AndI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart[rs1] & imm;
    D(andi, rd, rs1, rs2);
    return true;
}

bool SllI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart[rs1] << (imm & 0b11111U);
    D(slli, rd, rs1, rs2);
    return true;
}

bool SrlI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart[rs1] >> (imm & 0b11111U);
    D(srli, rd, rs1, rs2);
    return true;
}

bool SraI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = static_cast<SRegister>(hart[rs1]) >> (imm & 0b11111U);
    D(srai, rd, rs1, rs2);
    return true;
}

bool SltI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = (static_cast<SRegister>(hart[rs1]) < static_cast<SImmediate>(imm)) ? 1 : 0;
    D(slti, rd, rs1, rs2);
    return true;
}

bool SltIU(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = (static_cast<URegister>(hart[rs1]) < static_cast<UImmediate>(imm)) ? 1 : 0;
    D(sltiu, rd, rs1, rs2);
    return true;
}

bool Lb(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart.Load(hart[rs1] + imm);
    D(lb, rd, rs1, rs2);
    return true;
}

bool Lh(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart.Load(hart[rs1] + imm); //FUCK: replace with bitwise bit setting: only lower part
    D(lh, rd, rs1, rs2);
    return true;
}

bool Lw(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart.Load(hart[rs1] + imm);
    D(lw, rd, rs1, rs2);
    return true;
}

bool Lbu(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart.Load(hart[rs1] + imm);
    D(lbu, rd, rs1, rs2);
    return true;
}

bool Lhu(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart.Load(hart[rs1] + imm);
    D(lhu, rd, rs1, rs2);
    return true;
}

bool Sb(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart.Store<Byte>(hart[rs1] + imm, (hart[rs2] & ((1 << (8*sizeof(Byte))) - 1)));
    D(sb, rd, rs1, rs2);
    return true;
}

bool Sh(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart.Store<Half>(hart[rs1] + imm, (hart[rs2] & ((1 << (8*sizeof(Half))) - 1)));
    D(sh, rd, rs1, rs2);
    return true;
}

bool Sw(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart.Store<Word>(hart[rs1] + imm, hart[rs2]);
    D(sw, rd, rs1, rs2);
    return true;
}

bool Beq(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    D(beq, rd, rs1, rs2);
    if (hart[rs1] == hart[rs2]) {
        hart.MovePC(imm);
        return false;
    }
    return true;
}

bool Bne(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    D(bne, rd, rs1, rs2);
    if (hart[rs1] != hart[rs2]) {
        hart.MovePC(imm);
        return false;
    }
    return true;
}

bool Blt(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    D(blt, rd, rs1, rs2);
    if (static_cast<SRegister>(hart[rs1]) < static_cast<SRegister>(hart[rs2])) {
        hart.MovePC(imm);
        return false;
    }
    return true;
}

bool Bge(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    D(bge, rd, rs1, rs2);
    if (static_cast<SRegister>(hart[rs1]) >= static_cast<SRegister>(hart[rs2])) {
        hart.MovePC(imm);
        return false;
    }
    return true;
}

bool BltU(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    D(bltu, rd, rs1, rs2);
    if (hart[rs1] < hart[rs2]) {
        hart.MovePC(imm);
        return false;
    }
    return true;
}

bool BgeU(FUNC_SIGNATURE) {
    RegIdx rs1 = std::get<RegIdx>(param1);
    RegIdx rs2 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    D(bgeu, rd, rs1, rs2);
    if (hart[rs1] >= hart[rs2]) {
        hart.MovePC(imm);
        return false;
    }
    return true;
}

bool Jal(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    Immediate imm = std::get<Immediate>(param2);
    hart[rd] = hart.GetPC() + 4;
    D(jal, rd, rs1, rs2);
    if (imm == 0) {
        return true;    
    }
    hart.MovePC(imm);
    return false;
}

bool Jalr(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    D(jalr, rd, rs1, rs2);
    hart[rd] = hart.GetPC() + 4;
    hart.SetPC(hart[rs1] + imm);
    return false;
}

bool Lui(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    Immediate imm = std::get<Immediate>(param2);
    hart[rd] = imm << 12;
    D(lui, rd, rs1, rs2);
    return true;
}

bool AuiPC(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    Immediate imm = std::get<Immediate>(param2);
    hart[rd] = hart.GetPC() + (imm << 12);
    D(auipc, rd, rs1, rs2);
    return true;
}

#include <unistd.h>
#include <sys/syscall.h>

bool ECall(FUNC_SIGNATURE) {
    D(ecall, rd, rs1, rs2);
    if (hart[17] == 93) {
        hart.Stop();
    }

    int32_t ret = syscall(hart[17]); // a7
    hart[10] = ret;
    return true;
}

bool EBreak(FUNC_SIGNATURE) {
    D(ebreak, rd, rs1, rs2);
    hart.Stop();
    return false;
}

}