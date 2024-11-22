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

        Uint PutField(Uint startBit, Uint endBit, Uint code) {
            return (code & Mask(0U, endBit - startBit)) << startBit;
        }

        Uint GetSignBit(Uint code) {
            return GetField(31U, 31U, code);
        }

        Uint ExtendWithSignBit(Uint from, Uint code) {
            Uint signBit = GetSignBit(code);
            Uint oneExtend = Mask(from, 31U);
            return signBit == 1U ? oneExtend : 0U;
        }

        #define GETPUT(start_idx, end_idx, field) \
        Uint Get##field(Uint code) {return GetField(start_idx, end_idx, code);} \
        Uint Put##field(Uint code) {return PutField(start_idx, end_idx, code);}

        GETPUT(0U, 6U, Opcode)
        GETPUT(7U, 11U, Rd)
        GETPUT(12U, 14U, Funct3)
        GETPUT(15U, 19U, Rs1)
        GETPUT(20U, 24U, Rs2)
        GETPUT(25U, 31U, Funct7)

        #undef GETPUT

        Uint GetImmTypeI(Uint code) {
            Uint immBody = GetField(20U, 30U, code);
            Uint imm = immBody | ExtendWithSignBit(11U, code);
            return imm;
        }

        Uint PutImmTypeI(Immediate imm) {
            return PutField(20U, 31U, imm);
        }

        Uint GetImmTypeS(Uint code) {
            Uint immBodyPart1 = GetField(7U, 11U, code);
            Uint immBodyPart2 = GetField(25U, 30U, code) << 5U;
            Uint imm = immBodyPart1 | immBodyPart2 | ExtendWithSignBit(11U, code);
            return imm;
        }

        Uint GetImmTypeU(Uint code) {
            Uint imm = GetField(12U, 30U, code);
            imm <<= 12U;
            return imm;
        }

        Uint GetImmTypeJ(Uint code) {
            Uint immBodyPart1 = GetField(21U, 30U, code) << 1U;
            Uint immBodyPart2 = GetField(20U, 20U, code) << 11U;
            Uint immBodyPart3 = GetField(12U, 19U, code) << 12U;
            Uint imm = immBodyPart1 | immBodyPart2 | immBodyPart3 | ExtendWithSignBit(20U, code);
            return imm;
        }

        bool TestGetField() {
            //                     f7      rs2   rs1   f3  rd    op  
            constexpr Uint sra = 0b0100000'00010'00100'101'01000'0110011;

            #define CHECK(cond) if (!(cond)) {std::cerr << #cond << " failed\n"; return false;}

            CHECK(GetOpcode(sra) == 0b0110011);
            CHECK(GetRd(sra)     == 0b01000);
            CHECK(GetFunct3(sra) == 0b101);
            CHECK(GetRs1(sra)    == 0b00100);
            CHECK(GetRs2(sra)    == 0b00010);
            CHECK(GetFunct7(sra) == 0b0100000);

            #undef CHECK

            return true;
        }

        Uint Type::R::Build(RegIdx rd, RegIdx rs1, RegIdx rs2) const {
            return  Opcode              |
                    PutRd(rd)           |
                    PutRs1(rs1)         |
                    PutRs2(rs2)         |
                    PutFunct3(funct3)   |
                    PutFunct7(funct7);
        }

        Uint IBuild(Uint Opcode, Uint funct3, RegIdx rd, RegIdx rs1, Immediate imm) {
        return  Opcode              |
                PutRd(rd)           |
                PutRs1(rs1)         |
                PutFunct3(funct3)   |
                PutImmTypeI(imm);
        }

        #define BUILD_TYPE(IType)                                            \
        Uint Type::I##IType::Build(RegIdx rd, RegIdx rs1, Immediate imm) const {   \
            return IBuild(Opcode, funct3, rd, rs1, imm);                    \
        }

        BUILD_TYPE(Logic)
        BUILD_TYPE(Load)
        BUILD_TYPE(Jump)
        BUILD_TYPE(Env)

        #undef BUILD_TYPE

        constexpr Uint mergeFunct(Uint funct3, Uint funct7) {
            return funct3 | (funct7 << 3);
        }

        constexpr Uint mergeFunct(Type::R instr) {
            return mergeFunct(instr.funct3, instr.funct7);
        }

        Instruction DecodeR(Uint binInstruction) {
            Uint mergedFunct = mergeFunct(GetFunct3(binInstruction), GetFunct7(binInstruction));
            RegIdx rs1 = GetRs1(binInstruction);
            RegIdx rs2 = GetRs2(binInstruction);
            RegIdx rd  = GetRd(binInstruction);

            #define CASE(Instr) case mergeFunct(Instr):                             \
                                    std::cerr << #Instr "\n";                       \
                                    return Instruction{                             \
                                        .PFN_Instruction = InstructionSet::Instr,   \
                                        .param1 = rd,                               \
                                        .param2 = rs1,                              \
                                        .param3 = rs2};
            switch (mergedFunct) {
                CASE(Add)
                CASE(Sub)
                CASE(Xor)
                CASE(Or)
                CASE(And)
                CASE(Sll)
                CASE(Sra)
                CASE(Slt)
                CASE(Sltu)           

                default:
                    std::cerr   << "Unkown R instruction: "
                                << std::bitset<32>{mergedFunct} << '\n';
                    return Instruction{};
            }
            #undef CASE
        }

        bool IsSraI(Uint imm) {
            return GetField(5U, 11U, imm) == SraI.highImm;
        }

        Instruction DecodeILogic(Uint binInstruction) {
            Uint funct3 = GetFunct3(binInstruction);
            RegIdx rd = GetRd(binInstruction);
            RegIdx rs1 = GetRs1(binInstruction);
            Immediate imm = GetImmTypeI(binInstruction);
            // TODO: ^--- common for all DecodeI*: move to function

            #define CASE(Instr) case Instr.funct3:                      \
                        std::cerr << #Instr "\n";                       \
                        return Instruction{                             \
                            .PFN_Instruction = InstructionSet::Instr,   \
                            .param1 = rd,                               \
                            .param2 = rs1,                              \
                            .param3 = imm};
            switch(funct3) {
                CASE(AddI)
                CASE(XorI)
                CASE(OrI)
                CASE(AndI)
                CASE(SllI)
                CASE(SltI)
                CASE(SltIU)

                case SrlI.funct3:
                    if (IsSraI(imm)) {
                        std::cerr << "SraI\n";
                        return Instruction{
                        .PFN_Instruction = InstructionSet::SraI,
                        .param1 = rd,
                        .param2 = rs1,
                        .param3 = imm};
                    }                    

                    std::cerr << "SrlI\n";
                    return Instruction{
                        .PFN_Instruction = InstructionSet::SrlI,
                        .param1 = rd,
                        .param2 = rs1,
                        .param3 = imm};

                default:
                    std::cerr   << "Unkown ILogic instruction: "
                                << std::bitset<32>{funct3} << '\n';
                    return Instruction{};
            }
            #undef CASE
        }

        Instruction DecodeILoad(Uint binInstruction) {
            Uint funct3 = GetFunct3(binInstruction);
            RegIdx rd = GetRd(binInstruction);
            RegIdx rs1 = GetRs1(binInstruction);
            Immediate imm = GetImmTypeI(binInstruction);

            #define CASE(Instr) case Instr.funct3:                      \
                        std::cerr << #Instr "\n";                       \
                        return Instruction{                             \
                            .PFN_Instruction = InstructionSet::Instr,   \
                            .param1 = rd,                               \
                            .param2 = rs1,                              \
                            .param3 = imm};
            switch(funct3) {
                CASE(Lb)
                CASE(Lh)
                CASE(Lw)
                CASE(Lbu)
                CASE(Lhu)

                default:
                    std::cerr   << "Unkown ILoad instruction: "
                                << std::bitset<32>{funct3} << '\n';
                    return Instruction{};
            }
            #undef CASE
        }

        Instruction DecodeIJump(Uint binInstruction) {
            Uint funct3 = GetFunct3(binInstruction);
            RegIdx rd = GetRd(binInstruction);
            RegIdx rs1 = GetRs1(binInstruction);
            Immediate imm = GetImmTypeI(binInstruction);

            std::cerr << "Jalr\n";
            return Instruction{                            
                .PFN_Instruction = InstructionSet::Jalr,
                .param1 = rd,
                .param2 = rs1,
                .param3 = imm};
        }

        Instruction DecodeIEnv(Uint binInstruction) {
            Uint funct3 = GetFunct3(binInstruction);
            RegIdx rd = GetRd(binInstruction);
            RegIdx rs1 = GetRs1(binInstruction);
            Immediate imm = GetImmTypeI(binInstruction);

            switch (imm)
            {
            case ECall.imm:
                std::cerr << "ECall\n";
                return Instruction{                            
                    .PFN_Instruction = InstructionSet::ECall,
                    .param1 = rd,
                    .param2 = rs1,
                    .param3 = imm};
            
            case EBreak.imm:
                std::cerr << "EBreak\n";
                return Instruction{                            
                    .PFN_Instruction = InstructionSet::EBreak,
                    .param1 = rd,
                    .param2 = rs1,
                    .param3 = imm};

            default:
                std::cerr   << "Unkown IEnv instruction: "
                            << std::bitset<32>{imm} << '\n';
                return Instruction{};
            }
        }

        Instruction DecodeS(Uint binInstruction) {
            Uint funct3 = GetFunct3(binInstruction);
            RegIdx rs1 = GetRs1(binInstruction);
            RegIdx rs2 = GetRs2(binInstruction);
            Immediate imm = GetImmTypeS(binInstruction);

            #define CASE(Instr) case Instr.funct3:             \
                std::cerr << #Instr "\n";                      \
                return Instruction{                            \
                    .PFN_Instruction = InstructionSet::Instr,  \
                    .param1 = rs1,                             \
                    .param2 = rs2,                             \
                    .param3 = imm};
            switch (funct3)
            {
                CASE(Sb)
                CASE(Sh)
                CASE(Sw)
                
            default:
                std::cerr   << "Unkown S instruction: "
                            << std::bitset<32>{imm} << '\n';
                return Instruction{};
            }
            #undef CASE
        }

        Instruction DecodeB(Uint binInstruction) {
            Uint funct3 = GetFunct3(binInstruction);
            RegIdx rs1 = GetRs1(binInstruction);
            RegIdx rs2 = GetRs2(binInstruction);
            Immediate imm = GetImmTypeS(binInstruction);

            #define CASE(Instr) case Instr.funct3:             \
                std::cerr << #Instr "\n";                      \
                return Instruction{                            \
                    .PFN_Instruction = InstructionSet::Instr,  \
                    .param1 = rs1,                             \
                    .param2 = rs2,                             \
                    .param3 = imm};
            switch (funct3)
            {
                CASE(Beq)
                CASE(Bne)
                CASE(Blt)
                CASE(Bge)
                CASE(BltU)
                CASE(BgeU)
                
            default:
                std::cerr   << "Unkown B instruction: "
                            << std::bitset<32>{imm} << '\n';
                return Instruction{};
            }
            #undef CASE
        }

        Instruction DecodeU(Uint binInstruction) {
            Uint opcode = GetOpcode(binInstruction); 
            RegIdx rd = GetRd(binInstruction);
            Immediate imm = GetImmTypeU(binInstruction);

            #define CASE(Instr) case Instr.Opcode:             \
                std::cerr << #Instr "\n";                      \
                return Instruction{                            \
                    .PFN_Instruction = InstructionSet::Instr,  \
                    .param1 = rd,                              \
                    .param2 = imm};
            switch (opcode)
            {
                CASE(Lui)
                CASE(AuiPC)
                
            default:
                std::cerr   << "Unkown U instruction: "
                            << std::bitset<32>{imm} << '\n';
                return Instruction{};
            }
            #undef CASE
        }

        Instruction DecodeJ(Uint binInstruction) {
            RegIdx rd = GetRd(binInstruction);
            Immediate imm = GetImmTypeJ(binInstruction);

            return Instruction{                
                .PFN_Instruction = InstructionSet::Jal, 
                .param1 = rd,
                .param2 = imm};
        }

        Instruction Decode(Uint binInstruction) {
            // std::cerr << std::bitset<32>{binInstruction} << '\n';
            Uint const opcode = GetOpcode(binInstruction);

            switch (opcode)
            {
                case Type::R::Opcode:
                    std::cerr << "R\n";
                    return DecodeR(binInstruction);
                
                case Type::ILogic::Opcode:
                    std::cerr << "ILogic\n";
                    return DecodeILogic(binInstruction);

                case Type::ILoad::Opcode:
                    std::cerr << "ILoad\n";
                    return DecodeILoad(binInstruction);

                case Type::IJump::Opcode:
                    std::cerr << "IJump\n";
                    return DecodeIJump(binInstruction);
                
                case Type::IEnv::Opcode:
                    std::cerr << "IEnv\n";
                    return DecodeIEnv(binInstruction);

                case Type::S::Opcode:
                    std::cerr << "S\n";
                    return DecodeS(binInstruction);

                case Type::B::Opcode:
                    std::cerr << "B\n";
                    return DecodeB(binInstruction);

                case Lui.Opcode:
                case AuiPC.Opcode:
                    std::cerr << "U\n";
                    return DecodeU(binInstruction);

                case Type::J::Opcode:
                    std::cerr << "J\n";
                    return DecodeJ(binInstruction);

                default:
                    std::cerr << "Unknown instruction: " << std::bitset<32>{opcode} << '\n';
                    break;
            }

            return Instruction{};
        }
    } // Decoder

}; // RISCVS