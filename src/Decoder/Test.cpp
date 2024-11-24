#include <hart.hpp>
#include <machine.hpp>
#include <iostream>
#include <cstdlib>
#include <Decoder.hpp>

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
                RegIdx rdIdx = 0;
                RegIdx rs1Idx = 1;
                RegIdx rs2Idx = 2;

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
                RegIdx rdIdx = 0;
                RegIdx rs1Idx = 1;
                
                hart[rdIdx] = GetRandValue();
                hart[rs1Idx] = GetRandValue();
                Immediate imm  = GetRandValue();

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

        bool TestI(  Hart& hart,
                    Type::ILoad instr,
                    std::function<bool(Uint, Uint, Uint)> cond) {
                std::cerr << "---------[]---------\n";
                RegIdx rdIdx = 0;
                RegIdx rs1Idx = 1;
                
                hart[rdIdx] = GetRandValue();
                hart[rs1Idx] = GetRandAddr();
                Immediate imm  = GetRandAddrShift();

                std::cerr << "rs1_val: " << hart[rs1Idx] << '\n';

                static int testIdx = 0;
                testIdx++;

                std::cerr << "Input params: " << rdIdx << ' ' << rs1Idx << ' ' << imm << '\n';
                auto testInstr = instr.Build(rdIdx, rs1Idx, imm);
                auto pc = hart.GetPC();
                hart.Store(pc, testInstr);

                const Uint storedImm  = GetRandValue();
                std::cerr << "Put imm: " << storedImm << '\n';
                hart.Store(hart[rs1Idx] + imm, storedImm);
                hart.Execute();

                CHECK(cond(hart[rdIdx], hart[rs1Idx], imm));

                return true;
        }

        bool Cmp(int32_t lhs, int32_t rhs, int32_t bits = 32) {
            int32_t mask = Mask(0, bits - 1);
            return (lhs & mask) == (rhs & mask);
        }

        bool CmpB(int32_t lhs, int32_t rhs) {
            std::cerr << "cmpb: " << lhs << ' ' << rhs << '\n';
            return Cmp(lhs, rhs, 8);
        }

        bool CmpH(int32_t lhs, int32_t rhs) {
            std::cerr << "cmph: " << lhs << ' ' << rhs << '\n';
            return Cmp(lhs, rhs, 16);
        }

        bool CmpW(int32_t lhs, int32_t rhs) {
            std::cerr << "cmpw: " << lhs << ' ' << rhs << '\n';
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
            // in the hart[rd] == Mem[hart[rs1] + imm] == storeImm
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
                // beq
                // bne
                // blt

            // U
                // lui
                // auipc

            // J
                // jal
        }

    } // Decoder

} // RISCVS