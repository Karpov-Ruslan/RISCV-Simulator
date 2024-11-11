// TODO: I think it's not good location for this file. Change it if you wish.

#pragma once

#include <variant>
#include <optional>
#include <functional>
#include <cstdint>

namespace RISCVS {

#define PARAM_TYPES int8_t, int16_t, int32_t, uint8_t, uint16_t, uint32_t

// Just useful typedefs
using Register = int32_t;
using Immediate = int32_t;
using UImmediate = uint32_t;

// Load/Store things
using Word = int32_t;
using Half = int16_t;
using Byte = int8_t;
using UWord = uint32_t;
using UHalf = uint16_t;
using UByte = uint8_t;

struct Instruction {
    using Param = std::optional<std::variant<PARAM_TYPES>>;

    std::function<void(const Param&, const Param&, const Param&)> PFN_Instruction;
    Param param1;
    Param param2;
    Param param3;
};

}