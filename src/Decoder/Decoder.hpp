#pragma once
#include "../instruction.hpp"

#include <iostream>

using Uint = unsigned;
namespace RISCVS {

    namespace Decoder {

        Uint GetOpcode(Uint code);
        bool TestGetField();
        Instruction Decode(Uint binInstruction);

    } // Decoder

} // RISCVS
