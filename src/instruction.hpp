// TODO: I think it's not good location for this file. Change it if you wish.

#pragma once

#include <variant>
#include <optional>
#include <functional>
#include <cstdint>

#include <defines.hpp>

namespace RISCVS {

// Just useful typedefs
using Immediate = uint32_t;
using UImmediate = uint32_t;

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

void Add(FUNC_SIGNATURE);
void Sub(FUNC_SIGNATURE);
void AddI(FUNC_SIGNATURE);

}

}