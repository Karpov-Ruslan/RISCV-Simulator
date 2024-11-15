#include <hart.hpp>
#include <machine.hpp>
#include <iostream>
#include <cstdlib>

#define CHECK(cond)                                                         \
    if (!(cond)) {                                                          \
        std::cerr << testIdx << " is broken\n";     /* Not informative!*/   \
        return false;                                                       \
    }

namespace RISCVS {

    namespace Decoder {

        bool TestR(  Hart& hart,
                    Type::R instr,
                    std::function<bool(Uint, Uint, Uint)> cond) {
                std::cerr << "---------[]---------\n";
                RegIdx rdIdx = 0;
                RegIdx rs1Idx = 1;
                RegIdx rs2Idx = 2;

                hart[rdIdx] = std::rand();
                hart[rs1Idx] = std::rand();
                hart[rs2Idx] = std::rand();

                static int testIdx = 0;
                testIdx++;

                auto testInstr = instr.Build(rdIdx, rs1Idx, rs2Idx);
                auto pc = hart.GetPC();
                hart.Store(pc, testInstr);

                hart.Execute();

                CHECK(cond(hart[rdIdx], hart[rs1Idx], hart[rs2Idx]));

                return true;
        }

        bool TestI(  Hart& hart,
                    Type::ILogic instr,
                    std::function<bool(Uint, Uint, Uint)> cond) {
                std::cerr << "---------[]---------\n";
                RegIdx rdIdx = 0;
                RegIdx rs1Idx = 1;
                
                hart[rdIdx] = std::rand();
                hart[rs1Idx] = std::rand();
                Immediate imm  = std::rand() % 500;

                static int testIdx = 0;
                testIdx++;

                std::cerr << rdIdx << ' ' << rs1Idx << ' ' << imm << '\n';
                auto testInstr = instr.Build(rdIdx, rs1Idx, imm);
                auto pc = hart.GetPC();
                hart.Store(pc, testInstr);

                hart.Execute();

                CHECK(cond(hart[rdIdx], hart[rs1Idx], imm));

                return true;
        }

        int TestDecoder() {
            Machine machine{};
            Hart hart{machine};

            std::srand(0);

            // R
            TestR(hart, Add, [](Uint rd, Uint rs1, Uint rs2){return rd == (rs1 + rs2);});
            TestR(hart, Sub, [](Uint rd, Uint rs1, Uint rs2){return rd == (rs1 - rs2);});
            TestR(hart, Xor, [](Uint rd, Uint rs1, Uint rs2){return rd == (rs1 ^ rs2);});
            
            // ILogic
            TestI(hart, AddI, [](Uint rd, Uint rs1, Uint imm){return rd == (rs1 + imm);});
            TestI(hart, XorI, [](Uint rd, Uint rs1, Uint imm){return rd == (rs1 ^ imm);});
            TestI(hart, OrI, [](Uint rd, Uint rs1, Uint imm){return rd == (rs1 | imm);});

            // ILoad

            // IJump

            // IEnv
        }

    } // Decoder

} // RISCVS