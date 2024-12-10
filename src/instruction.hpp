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

    std::function<bool(Hart&, const Param&, const Param&, const Param&)> PFN_Instruction;
    Param param1;
    Param param2;
    Param param3;
};

namespace InstructionSet {

using IParam = Instruction::Param;

#define FUNC_SIGNATURE Hart& hart, const IParam& param1, const IParam& param2, const IParam& param3

// Logical and arithmetical instructions
bool Add(FUNC_SIGNATURE);
bool Sub(FUNC_SIGNATURE);
bool Xor(FUNC_SIGNATURE);
bool Or(FUNC_SIGNATURE);
bool And(FUNC_SIGNATURE);
bool Sll(FUNC_SIGNATURE);
bool Srl(FUNC_SIGNATURE);
bool Sra(FUNC_SIGNATURE);
bool Slt(FUNC_SIGNATURE);
bool Sltu(FUNC_SIGNATURE);

// Immediate logical and arithmetical instructions
bool AddI(FUNC_SIGNATURE);
bool XorI(FUNC_SIGNATURE);
bool OrI(FUNC_SIGNATURE);
bool AndI(FUNC_SIGNATURE);
bool SllI(FUNC_SIGNATURE);
bool SrlI(FUNC_SIGNATURE);
bool SraI(FUNC_SIGNATURE);
bool SltI(FUNC_SIGNATURE);
bool SltIU(FUNC_SIGNATURE);

// Load instructions
bool Lb(FUNC_SIGNATURE);
bool Lh(FUNC_SIGNATURE);
bool Lw(FUNC_SIGNATURE);
bool Lbu(FUNC_SIGNATURE);
bool Lhu(FUNC_SIGNATURE);

// Store instructions
bool Sb(FUNC_SIGNATURE);
bool Sh(FUNC_SIGNATURE);
bool Sw(FUNC_SIGNATURE);

bool Beq(FUNC_SIGNATURE);
bool Bne(FUNC_SIGNATURE);
bool Blt(FUNC_SIGNATURE);
bool Bge(FUNC_SIGNATURE);
bool BltU(FUNC_SIGNATURE);
bool BgeU(FUNC_SIGNATURE);

bool Jal(FUNC_SIGNATURE);
bool Jalr(FUNC_SIGNATURE);

bool Lui(FUNC_SIGNATURE);
bool AuiPC(FUNC_SIGNATURE);

bool ECall(FUNC_SIGNATURE);
bool EBreak(FUNC_SIGNATURE);

}

}