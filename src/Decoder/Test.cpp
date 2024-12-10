#include <hart.hpp>
#include <machine.hpp>
#include <iostream>
#include <cstdlib>
#include <Decoder.hpp>
#include <register.hpp>

#define CHECK(cond)                                                         \
    if (!(cond)) {                                                          \
        std::cerr << testIdx << " is broken\n";     /* Not informative!*/   \
        return false;                                                       \
    }

namespace {

    constexpr int32_t MaxValue = 500;
    constexpr int32_t MaxAddr = 500;
    constexpr int32_t MaxAddrShift = 500;

    int32_t GetRandValue() {
        return rand() % MaxValue;
    }

    int32_t GetRandAddr() {
        return rand() % MaxAddr;
    }

    int32_t GetRandAddrShift() {
        return rand() % MaxAddrShift;
    }

} // anon namespace

namespace RISCVS {

    namespace Decoder {

        bool TestR(  Hart& hart,
                    Type::R instr,
                    std::function<bool(Uint, Uint, Uint)> cond) {
                std::cerr << "---------[]---------\n";
                RegIdx rdIdx = 1;
                RegIdx rs1Idx = 2;
                RegIdx rs2Idx = 3;

                hart[rdIdx] = GetRandValue();
                hart[rs1Idx] = GetRandValue();
                hart[rs2Idx] = GetRandValue();

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
                RegIdx rdIdx = 1;
                RegIdx rs1Idx = 2;
                
                hart[rdIdx] = GetRandValue();
                hart[rs1Idx] = GetRandValue();
                Immediate imm  = GetRandValue();

                static int testIdx = 0;
                testIdx++;

                auto testInstr = instr.Build(rdIdx, rs1Idx, imm);
                auto pc = hart.GetPC();
                hart.Store(pc, testInstr);

                hart.Execute();

                CHECK(cond(hart[rdIdx], hart[rs1Idx], imm));

                return true;
        }

        bool TestI(  Hart& hart,
                    Type::ILoad instr,
                    std::function<bool(Uint, Uint, Uint)> cond) {
                std::cerr << "---------[]---------\n";
                RegIdx rdIdx = 1;
                RegIdx rs1Idx = 2;
                
                hart[rdIdx] = GetRandValue();
                hart[rs1Idx] = GetRandAddr();
                Immediate imm  = GetRandAddrShift();

                static int testIdx = 0;
                testIdx++;

                auto testInstr = instr.Build(rdIdx, rs1Idx, imm);
                auto pc = hart.GetPC();
                hart.Store(pc, testInstr);

                const Uint storedImm  = GetRandValue();
                hart.Store(hart[rs1Idx] + imm, storedImm);
                hart.Execute();

                CHECK(cond(hart[rdIdx], hart[rs1Idx], imm));

                return true;
        }

        bool TestB( Hart& hart,
                    Type::B instr,
                    std::function<bool(Uint, Uint)> cond,
                    Register::RegisterType rs1Value,
                    Register::RegisterType rs2Value) {
                std::cerr << "---------[]---------\n";
                RegIdx rs1Idx = 1;
                RegIdx rs2Idx = 2;
                
                hart[rs1Idx] = rs1Value;
                hart[rs2Idx] = rs2Value;

                Immediate imm  = GetRandAddrShift() & (~1); // wrt to immediate B-format

                static int testIdx = 0;
                testIdx++;

                auto testInstr = instr.Build(rs1Idx, rs2Idx, imm);
                auto pc = hart.GetPC();
                hart.Store(pc, testInstr);

                hart.Execute();

                CHECK(hart.GetPC() == (pc + int(cond(rs1Value, rs2Value)) * imm));

                return true;
        }

        bool Cmp(int32_t lhs, int32_t rhs, int32_t bits = 32) {
            int32_t mask = Mask(0, bits - 1);
            return (lhs & mask) == (rhs & mask);
        }

        bool CmpB(int32_t lhs, int32_t rhs) {
            return Cmp(lhs, rhs, 8);
        }

        bool CmpH(int32_t lhs, int32_t rhs) {
            return Cmp(lhs, rhs, 16);
        }

        bool CmpW(int32_t lhs, int32_t rhs) {
            return Cmp(lhs, rhs, 32);
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
            TestI(hart, Lb, [&hart](Uint rd, Uint rs1, Uint imm){return CmpB(hart.M(rs1 + imm), rd);});
            TestI(hart, Lh, [&hart](Uint rd, Uint rs1, Uint imm){return CmpH(hart.M(rs1 + imm), rd);});
            TestI(hart, Lw, [&hart](Uint rd, Uint rs1, Uint imm){return CmpW(hart.M(rs1 + imm), rd);});

            // IJump
                // jalr

            // IEnv
                // ebreak

            // S
                // sb
                // sh
                // sw

            // B
            TestB(hart, Beq, [&hart](Uint rs1, Uint rs2){return (rs1 == rs2);}, 0, 0);
            TestB(hart, Beq, [&hart](Uint rs1, Uint rs2){return (rs1 == rs2);}, 0, 1);
            
            TestB(hart, Bne, [&hart](Uint rs1, Uint rs2){return (rs1 != rs2);}, 0, 1);
            TestB(hart, Bne, [&hart](Uint rs1, Uint rs2){return (rs1 != rs2);}, 0, 0);
            
            TestB(hart, Blt, [&hart](Uint rs1, Uint rs2){return (rs1 < rs2);}, 0, 1);
            TestB(hart, Blt, [&hart](Uint rs1, Uint rs2){return (rs1 < rs2);}, 0, 0);
            
            // U
                // lui
                // auipc

            // J
                // jal
            
            return 0; // return number of passed tests
        }

    } // Decoder

} // RISCVS