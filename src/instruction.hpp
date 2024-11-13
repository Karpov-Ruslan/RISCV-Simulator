// TODO: I think it's not good location for this file. Change it if you wish.

#pragma once

#include <variant>
#include <optional>
#include <functional>
#include <cstdint>

#include <hart.hpp>

namespace RISCVS {

#define PARAM_TYPES int8_t, int16_t, int32_t, uint8_t, uint16_t, uint32_t

// Just useful typedefs
using Register = Hart::Register;
using RegIdx = Hart::RegisterIndex;
using Immediate = uint32_t;
using UImmediate = uint32_t;

// Load/Store things
using Word = int32_t;
using Half = int16_t;
using Byte = int8_t;
using UWord = uint32_t;
using UHalf = uint16_t;
using UByte = uint8_t;

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

void AddI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart[rs1] + imm;
}

void SubI(FUNC_SIGNATURE) {
    RegIdx rd = std::get<RegIdx>(param1);
    RegIdx rs1 = std::get<RegIdx>(param2);
    Immediate imm = std::get<Immediate>(param3);
    hart[rd] = hart[rs1] - imm;
}

}

}