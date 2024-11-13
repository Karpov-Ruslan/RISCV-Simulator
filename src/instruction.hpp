// TODO: I think it's not good location for this file. Change it if you wish.

#pragma once

#include <variant>
#include <optional>
#include <functional>
#include <cstdint>

#include <defines.hpp>

namespace RISCVS {

// Just useful typedefs
using Immediate = int32_t;

// Load/Store things
using Word = int32_t;
using Half = int16_t;
using Byte = int8_t;
using UWord = uint32_t;
using UHalf = uint16_t;
using UByte = uint8_t;

class Hart;

struct Instruction {
    using Param = std::variant<PARAM_TYPES>;

    std::function<void(Hart&, const Param&, const Param&, const Param&)> PFN_Instruction;
    Param param1;
    Param param2;
    Param param3;
};

namespace InstructionSet {

using IParam = Instruction::Param;

#define FUNC_SIGNATURE Hart& hart, const IParam& param1, const IParam& param2, const IParam& param3

// Logical and arithmetical instructions
void Add(FUNC_SIGNATURE);
void Sub(FUNC_SIGNATURE);
void Xor(FUNC_SIGNATURE);
void Or(FUNC_SIGNATURE);
void And(FUNC_SIGNATURE);
void Sll(FUNC_SIGNATURE);
void Srl(FUNC_SIGNATURE);
void Sra(FUNC_SIGNATURE);
void Slt(FUNC_SIGNATURE);
void Sltu(FUNC_SIGNATURE);

// Immediate logical and arithmetical instructions
void AddI(FUNC_SIGNATURE);
void XorI(FUNC_SIGNATURE);
void OrI(FUNC_SIGNATURE);
void AndI(FUNC_SIGNATURE);
void SllI(FUNC_SIGNATURE);
void SrlI(FUNC_SIGNATURE);
void SraI(FUNC_SIGNATURE);
void SltI(FUNC_SIGNATURE);
void SltIU(FUNC_SIGNATURE);

// Load instructions
void Lb(FUNC_SIGNATURE);
void Lh(FUNC_SIGNATURE);
void Lw(FUNC_SIGNATURE);
void Lbu(FUNC_SIGNATURE);
void Lhu(FUNC_SIGNATURE);

// Store instructions
void Sb(FUNC_SIGNATURE);
void Sh(FUNC_SIGNATURE);
void Sw(FUNC_SIGNATURE);

void Beq(FUNC_SIGNATURE);
void Bne(FUNC_SIGNATURE);
void Blt(FUNC_SIGNATURE);
void Bge(FUNC_SIGNATURE);
void BltU(FUNC_SIGNATURE);
void BgeU(FUNC_SIGNATURE);

void Jal(FUNC_SIGNATURE);
void Jalr(FUNC_SIGNATURE);

void Lui(FUNC_SIGNATURE);
void AuiPC(FUNC_SIGNATURE);

void ECall(FUNC_SIGNATURE);
void EBreak(FUNC_SIGNATURE);

}

}