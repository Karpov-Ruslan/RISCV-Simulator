#include <hart.hpp>
#include <machine.hpp>

int main() {
    using namespace RISCVS;

    Machine machine{};
    Hart hart{machine};

    hart[1] = 5;
    hart[2] = 4;
    hart[3] = 6;
    hart[4] = 2;

    //                         f7      rs2   rs1   f3  rd    op
    constexpr Uint addx1x2 = 0b0000000'00010'00001'000'00001'0110011;
    Uint test = Decoder::Add.Build(0, 0, 0);

    std::cout << std::bitset<32>{addx1x2} << '\n';
    std::cout << std::bitset<32>{test} << '\n';

    constexpr Uint subx3x4 = 0b0100000'00100'00011'000'00011'0110011;

    hart.Dump(5);

    hart.Store(0U, addx1x2);
    hart.Execute();
    hart.Dump(5);

    hart.Store(4U, subx3x4);
    hart.Execute();
    hart.Dump(5);
}
