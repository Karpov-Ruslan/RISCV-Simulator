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
        return (rand() % MaxAddrShift) & (~1);
    }

} // anon namespace

namespace RISCVS {

    namespace Decoder {

        bool TestR(  Hart& hart,
                    Type::R instr,
                    std::function<bool(Uint, Uint, Uint)> cond,
                    Register::RegisterType rs1 = 0,
                    Register::RegisterType rs2 = 0) {
                std::cerr << "---------[]---------\n";
                RegIdx rdIdx = 1;
                RegIdx rs1Idx = 2;
                RegIdx rs2Idx = 3;

                hart[rdIdx] = GetRandValue();
                hart[rs1Idx] = rs1 != 0 ? rs1 : GetRandValue();
                hart[rs2Idx] = rs2 != 0 ? rs2 : GetRandValue();

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
                    std::function<bool(Uint, Uint, Uint)> cond, Register::RegisterType rs1 = 0) {
                std::cerr << "---------[]---------\n";
                RegIdx rdIdx = 1;
                RegIdx rs1Idx = 2;
                
                hart[rdIdx] = GetRandValue();
                hart[rs1Idx] = (rs1 != 0) ? rs1 : GetRandValue();
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

                Immediate imm  = GetRandAddrShift();

                static int testIdx = 0;
                testIdx++;

                auto testInstr = instr.Build(rs1Idx, rs2Idx, imm);
                auto pc = hart.GetPC();
                hart.Store(pc, testInstr);

                hart.Execute();

                CHECK(hart.GetPC() == (pc + int(cond(rs1Value, rs2Value)) * imm));

                return true;
        }

        bool TestS( Hart& hart,
                    Type::S instr,
                    std::function<bool(Uint, Uint, Uint)> cond) {
                std::cerr << "---------[]---------\n";
                RegIdx rs1Idx = 1;
                RegIdx rs2Idx = 2;
                
                hart[rs1Idx] = GetRandAddr();
                hart[rs2Idx] = GetRandValue();
                Immediate imm  = GetRandAddrShift();

                static int testIdx = 0;
                testIdx++;

                auto testInstr = instr.Build(rs1Idx, rs2Idx, imm);
                auto pc = hart.GetPC();
                hart.Store(pc, testInstr);

                hart.Execute();

                CHECK(cond(hart[rs1Idx], hart[rs2Idx], imm));

                return true;
        }

        bool TestU( Hart& hart,
                    Type::U instr,
                    std::function<bool(Uint, Uint)> cond) {
                std::cerr << "---------[]---------\n";
                RegIdx rdIdx = 1;
                Immediate imm = GetRandValue();

                static int testIdx = 0;
                testIdx++;

                auto testInstr = instr.Build(rdIdx, imm);
                auto pc = hart.GetPC();
                hart.Store(pc, testInstr);

                hart.Execute(true);

                CHECK(cond(hart[rdIdx], imm));

                return true;
        }

        bool TestJal(Hart& hart, Immediate imm = 0) {
                std::cerr << "---------[]---------\n";
                RegIdx rdIdx = 1;
                imm = imm != 0 ? imm : GetRandAddrShift();

                static int testIdx = 0;
                testIdx++;

                auto testInstr = Jal.Build(rdIdx, imm);
                auto pc = hart.GetPC();
                hart.Store(pc, testInstr);

                hart.Execute(true);

                CHECK(hart[rdIdx] == (pc + 4) && hart.GetPC() == (pc + imm));

                return true;
        }

        bool TestJalr(Hart& hart, Immediate imm = 0) {
                std::cerr << "---------[]---------\n";
                RegIdx rdIdx = 1;
                RegIdx rs1Idx = 2;

                hart[rs1Idx] = GetRandAddr();
                imm = imm != 0 ? imm : GetRandAddrShift();

                static int testIdx = 0;
                testIdx++;

                auto testInstr = Jalr.Build(rdIdx, rs1Idx, imm);
                auto pc = hart.GetPC();
                hart.Store(pc, testInstr);

                hart.Execute(true);

                CHECK(hart[rdIdx] == (pc + 4) && hart.GetPC() == (hart[rs1Idx] + imm));

                return true;
        }

        bool TestI( Hart& hart,
                    Type::IEnv instr) {
                std::cerr << "---------[]---------\n";

                static int testIdx = 0;
                testIdx++;

                auto testInstr = instr.Build();
                auto pc = hart.GetPC();
                hart.Store(pc, testInstr);

                hart.Execute();

                CHECK(hart.IsStop() == true);
                hart.Run();

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
            // FUCK: It's likely to use random values in tests is a bad idea
            // e.g. it makes pain with Sll and Sra 

            // R
            TestR(hart, Add, [](Uint rd, Uint rs1, Uint rs2){return rd == (rs1 + rs2);});
            TestR(hart, Sub, [](Uint rd, Uint rs1, Uint rs2){return rd == (rs1 - rs2);});
            TestR(hart, Xor, [](Uint rd, Uint rs1, Uint rs2){return rd == (rs1 ^ rs2);});
            TestR(hart, Or, [](Uint rd, Uint rs1, Uint rs2){return rd == (rs1 | rs2);});
            TestR(hart, And, [](Uint rd, Uint rs1, Uint rs2){return rd == (rs1 & rs2);});
            TestR(hart, Sll, [](Uint rd, Uint rs1, Uint rs2){return rd == (rs1 << rs2);}, 58, 2);
            TestR(hart, Srl, [](Uint rd, Uint rs1, Uint rs2){return rd == (rs1 >> rs2);}, 4056, 3);
            TestR(hart, Sra, [](Uint rd, Uint rs1, Uint rs2){return rd == (rs1 >> rs2);}, 2313, 3); // ? msb extends
            TestR(hart, Slt, [](Uint rd, Uint rs1, Uint rs2){return rd == (rs1 < rs2) ? 1 : 0;});
            TestR(hart, Sltu, [](Uint rd, Uint rs1, Uint rs2){return rd == (rs1 < rs2) ? 1 : 0;}); // ? zero extends

            // ILogic
            TestI(hart, AddI, [](Uint rd, Uint rs1, Uint imm){return rd == (rs1 + imm);});
            TestI(hart, XorI, [](Uint rd, Uint rs1, Uint imm){return rd == (rs1 ^ imm);});
            TestI(hart, OrI, [](Uint rd, Uint rs1, Uint imm){return rd == (rs1 | imm);});
            TestI(hart, AndI, [](Uint rd, Uint rs1, Uint imm){return rd == (rs1 & imm);});
            TestI(hart, SllI, [](Uint rd, Uint rs1, Uint imm){return rd == (rs1 << GetField(0U, 4U, imm));}, 12);
            TestI(hart, SrlI, [](Uint rd, Uint rs1, Uint imm){return rd == (rs1 >> GetField(0U, 4U, imm));}, 12 << 10U);
            TestI(hart, SraI, [](Uint rd, Uint rs1, Uint imm){return rd == (rs1 >> GetField(0U, 4U, imm));}, 12 << 10U); // ? msb extends
            TestI(hart, SltI, [](Uint rd, Uint rs1, Uint imm){return rd == (rs1 < imm) ? 1 : 0;});
            TestI(hart, SltIU, [](Uint rd, Uint rs1, Uint imm){return rd == (rs1 < imm) ? 1 : 0;}); // ? zero extends

            // ILoad
            TestI(hart, Lb, [&hart](Uint rd, Uint rs1, Uint imm){return CmpB(hart.M(rs1 + imm), rd);});
            TestI(hart, Lh, [&hart](Uint rd, Uint rs1, Uint imm){return CmpH(hart.M(rs1 + imm), rd);});
            TestI(hart, Lw, [&hart](Uint rd, Uint rs1, Uint imm){return CmpW(hart.M(rs1 + imm), rd);});
            TestI(hart, Lbu, [&hart](Uint rd, Uint rs1, Uint imm){return CmpB(hart.M(rs1 + imm), rd);}); // ? zero extends
            TestI(hart, Lhu, [&hart](Uint rd, Uint rs1, Uint imm){return CmpH(hart.M(rs1 + imm), rd);}); // ? zero extends

            // S
            TestS(hart, Sb, [&hart](Uint rs1, Uint rs2, Uint imm){return CmpB(hart.M(rs1 + imm), rs2);});
            TestS(hart, Sh, [&hart](Uint rs1, Uint rs2, Uint imm){return CmpH(hart.M(rs1 + imm), rs2);});
            TestS(hart, Sw, [&hart](Uint rs1, Uint rs2, Uint imm){return CmpW(hart.M(rs1 + imm), rs2);});

            // B
            TestB(hart, Beq, [&hart](Uint rs1, Uint rs2){return (rs1 == rs2);}, 0, 0);
            TestB(hart, Beq, [&hart](Uint rs1, Uint rs2){return (rs1 == rs2);}, 0, 1);
            
            TestB(hart, Bne, [&hart](Uint rs1, Uint rs2){return (rs1 != rs2);}, 0, 1);
            TestB(hart, Bne, [&hart](Uint rs1, Uint rs2){return (rs1 != rs2);}, 0, 0);
            
            TestB(hart, Blt, [&hart](Uint rs1, Uint rs2){return (rs1 < rs2);}, 0, 1);
            TestB(hart, Blt, [&hart](Uint rs1, Uint rs2){return (rs1 < rs2);}, 0, 0);
            
            TestB(hart, Bge, [&hart](Uint rs1, Uint rs2){return (rs1 >= rs2);}, 2, 1);
            TestB(hart, Bge, [&hart](Uint rs1, Uint rs2){return (rs1 >= rs2);}, 2, 3);
            
            TestB(hart, BltU, [&hart](Uint rs1, Uint rs2){return (rs1 < rs2);}, 0, 1);
            TestB(hart, BltU, [&hart](Uint rs1, Uint rs2){return (rs1 < rs2);}, 0, 0);
            
            TestB(hart, BgeU, [&hart](Uint rs1, Uint rs2){return (rs1 >= rs2);}, 2, 1);
            TestB(hart, BgeU, [&hart](Uint rs1, Uint rs2){return (rs1 >= rs2);}, 2, 3);

            // U
            TestU(hart, Lui, [&hart](Uint rd, Uint imm){return rd == (imm << 12);});
            TestU(hart, AuiPC, [&hart](Uint rd, Uint imm){return rd == (hart.GetPC() + (imm << 12));});

            // J
            TestJal(hart, 12);
            
            // IJump
            TestJalr(hart, 12);

            // IEnv
            TestI(hart, ECall);
            TestI(hart, EBreak);

            return 0; // return number of passed tests
        }

    } // Decoder

} // RISCVS