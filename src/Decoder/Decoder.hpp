#pragma once

using Int = int;
using Uint = unsigned;

enum InstrIdx{
    Add,
    Sub,
    Unknown
};

using Instruction = InstrIdx;

bool TestGetField();

/* Pipeline:

*/

#include <iostream>

Uint GetOpcode(Uint code);
namespace Decoder {
    Instruction Decode(Uint binInstruction);
};
