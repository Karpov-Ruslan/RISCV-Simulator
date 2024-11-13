#include "Decoder.hpp"

#include <bitset>
#include <iostream>

namespace RISCVS {

    namespace Decoder {

        constexpr Uint Mask(Uint startBit, Uint endBit) {
            Uint lowerBound = (1U << (startBit)) - 1U;

            Uint upperBound = (endBit != 31U? (1 << (endBit + 1U)) - 1U : -1U);
            
            return upperBound - lowerBound;
        };

        Uint GetField(Uint startBit, Uint endBit, Uint code) {
            Uint mask = Mask(startBit, endBit);
            Uint field = (code & mask) >> startBit;
            return field;
        }

        Uint GetSignBit(Uint code) {
            return GetField(31U, 31U, code);
        }

        Uint ExtendWithSignBit(Uint from, Uint code) {
            Uint signBit = GetSignBit(code);
            Uint oneExtend = Mask(from, 31U);
            return signBit == 1U ? oneExtend : 0U;
        }

        Uint GetOpcode(Uint code) {
            return GetField(0U, 6U, code);
        }

        Uint GetRd(Uint code) {
            return GetField(7U, 11U, code);
        }

        Uint GetFunct3(Uint code) {
            return GetField(12U, 14U, code);
        }

        Uint GetRs1(Uint code) {
            return GetField(15U, 19U, code);
        }

        Uint GetRs2(Uint code) {
            return GetField(20U, 24U, code);
        }

        Uint GetFunct7(Uint code) {
            return GetField(25U, 31U, code);
        }

        Uint GetImmTypeI(Uint code) {
            Uint immBody = GetField(20U, 30U, code);
            Uint imm = immBody | ExtendWithSignBit(11U, code);
            return imm;
        }

        Uint GetImmTypeS(Uint code) {
            Uint immBodyPart1 = GetField(7U, 11U, code);
            Uint immBodyPart2 = GetField(25U, 30U, code) << 5U;
            Uint imm = immBodyPart1 | immBodyPart2 | ExtendWithSignBit(11U, code);
            return imm;
        }

        #define CHECK(cond) if (!(cond)) {std::cerr << #cond << " failed\n"; return false;}
        bool TestGetField() {
            //                     f7      rs2   rs1   f3  rd    op  
            constexpr Uint sra = 0b0100000'00010'00100'101'01000'0110011;

            CHECK(GetOpcode(sra) == 0b0110011);
            CHECK(GetRd(sra)     == 0b01000);
            CHECK(GetFunct3(sra) == 0b101);
            CHECK(GetRs1(sra)    == 0b00100);
            CHECK(GetRs2(sra)    == 0b00010);
            CHECK(GetFunct7(sra) == 0b0100000);

            return true;
        }

        namespace Type {
            struct R {
                static constexpr Uint Opcode = 0b110011;
                
                Uint rd;
                const Uint funct3;
                Uint rs1;
                Uint rs2;
                const Uint funct7;
            };

            struct I {
                static constexpr Uint OpcodeLogic = 0b0010011;
                static constexpr Uint OpcodeLoad  = 0b0000011;
                static constexpr Uint OpcodeJump  = 0b1100111;
                static constexpr Uint OpcodeEnv   = 0b1110011;

                Uint rd;
                const Uint funct3;
                Uint rs1;
                Uint imm;
            };
        };

        constexpr Type::R Add = Type::R{.funct3 = 0x0,
                                        .funct7 = 0x00};
        constexpr Type::R Sub = Type::R{.funct3 = 0x0,
                                        .funct7 = 0x20};

        constexpr Type::I AddI = Type::I{.funct3 = 0x0};
        constexpr Type::I XorI = Type::I{.funct3 = 0x4};

        constexpr Uint mergeFunct(Uint funct3, Uint funct7) {
            return funct3 | (funct7 << 3);
        }

        constexpr Uint mergeFunct(Type::R instr) {
            return mergeFunct(instr.funct3, instr.funct7);
        }

        Instruction DecodeR(Uint binInstruction) {
            Uint mergedFunct = mergeFunct(GetFunct3(binInstruction), GetFunct7(binInstruction));
            uint16_t rs1 = GetRs1(binInstruction);
            uint16_t rs2 = GetRs2(binInstruction);
            uint16_t rd  = GetRd(binInstruction);

            switch (mergedFunct) {
                case mergeFunct(Add):
                    std::cerr << "add\n";
                    return Instruction{.PFN_Instruction = InstructionSet::Add,
                                        .param1 = rd,
                                        .param2 = rs1,
                                        .param3 = rs2};

                case mergeFunct(Sub):
                    std::cerr << "sub\n";
                    return Instruction{.PFN_Instruction = InstructionSet::Sub,
                                        .param1 = rd,
                                        .param2 = rs1,
                                        .param3 = rs2};

                default:
                    std::cerr << "Unkown R instruction: " << std::bitset<32>{mergedFunct} << '\n';
                    return Instruction{};
            }
        }

        Instruction DecodeILogic(Uint binInstruction) {

            Uint funct3 = GetFunct3(binInstruction);
            Uint rd = GetFunct3(binInstruction);
            Uint rs1 = GetFunct3(binInstruction);
            Uint imm = GetImmTypeI(binInstruction);

            switch(funct3) {
                case AddI.funct3:
                    std::cerr << "addi\n";
                    return Instruction{.PFN_Instruction = InstructionSet::AddI,
                                        .param1 = rd,
                                        .param2 = rs1,
                                        .param3 = imm};

                default:
                    return Instruction{};
            }
        }

        Instruction Decode(Uint binInstruction) {
            std::cerr << std::bitset<32>{binInstruction} << '\n';
            Uint const opcode = GetOpcode(binInstruction);

            switch (opcode)
            {
                case Type::R::Opcode:
                    std::cerr << "R instruction\n";
                    return DecodeR(binInstruction);
                
                case Type::I::OpcodeLogic:
                    std::cerr << "ILogic\n";
                    return DecodeILogic(binInstruction);

                case Type::I::OpcodeLoad:
                    std::cerr << "ILoad\n";
                    break;

                case Type::I::OpcodeJump:
                    std::cerr << "IJump\n";
                    break;
                
                case Type::I::OpcodeEnv:
                    std::cerr << "IEnv\n";
                    break;

                default:
                    std::cerr << "Unknown instruction: " << std::bitset<32>{opcode} << '\n';
                    break;
            }

            return Instruction{};
        }
    } // Decoder

}; // RISCVS