#include "src/Decoder/Decoder.hpp"
#include <iostream>
#include <bitset>

int main() {
    if (TestGetField()) {
        std::cerr << "TestGetField is OK\n";
    }

    //                     f7      rs2   rs1   f3  rd    op  
    constexpr Uint add = 0b0000000'00010'00100'000'01000'0110011;
    constexpr Uint sub = 0b0100000'00010'00100'000'01000'0110011;

    //                      imm          rs1   f3  rd    op  
    constexpr Uint addi = 0b000000000101'00100'000'01000'0010011;
    constexpr Uint xori = 0b000000001010'00100'100'01000'0010011;

    Decoder::Decode(add);
    Decoder::Decode(sub);
    Decoder::Decode(addi);
    Decoder::Decode(xori);
}
