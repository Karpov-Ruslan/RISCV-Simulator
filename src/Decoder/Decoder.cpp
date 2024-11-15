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

        Uint GetImmTypeS(Uint code) {
            Uint immBodyPart1 = GetField(7U, 11U, code);
            Uint immBodyPart2 = GetField(25U, 30U, code) << 5U;
            Uint imm = immBodyPart1 | immBodyPart2 | ExtendWithSignBit(11U, code);
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

        Uint IBuild(Uint Opcode, Uint funct3, RegIdx rd, RegIdx rs1, Imm imm) {
        return  Opcode              |
                PutRd(rd)           |
                PutRs1(rs1)         |
                PutFunct3(funct3);
        }

        #define BUILD_TYPE(IType)                                            \
        Uint Type::I##IType::Build(RegIdx rd, RegIdx rs1, Imm imm) const {   \
            return IBuild(Opcode, funct3, rd, rs1, imm);                    \
        }

        BUILD_TYPE(Logic)
        BUILD_TYPE(Load)
        BUILD_TYPE(Jump)
        BUILD_TYPE(Env)

        #undef BUILD_TYPE

        // I-logic
        constexpr Type::ILogic AddI  = Type::ILogic{.funct3 = 0x0};
        constexpr Type::ILogic XorI  = Type::ILogic{.funct3 = 0x4};
        constexpr Type::ILogic OrI   = Type::ILogic{.funct3 = 0x6};
        constexpr Type::ILogic AndI  = Type::ILogic{.funct3 = 0x7};
        constexpr Type::ILogic SllI  = Type::ILogic{.funct3 = 0x1}; // !
        constexpr Type::ILogic SrlI  = Type::ILogic{.funct3 = 0x5}; // ! imm[5:12]
        constexpr Type::ILogic SraI  = Type::ILogic{.funct3 = 0x5}; // !
        constexpr Type::ILogic SltI  = Type::ILogic{.funct3 = 0x2};
        constexpr Type::ILogic SltIU = Type::ILogic{.funct3 = 0x3};

        // I-load
        constexpr Type::ILoad Lb  = Type::ILoad{.funct3 = 0x0};
        constexpr Type::ILoad Lh  = Type::ILoad{.funct3 = 0x1};
        constexpr Type::ILoad Lw  = Type::ILoad{.funct3 = 0x2};
        constexpr Type::ILoad Lbu = Type::ILoad{.funct3 = 0x4};
        constexpr Type::ILoad Lhu = Type::ILoad{.funct3 = 0x5};

        // I-jump
        constexpr Type::IJump Jalr = Type::IJump{.funct3 = 0x0};

        // I-env
        constexpr Type::IEnv ECall  = Type::IEnv{.funct3 = 0x0, .imm = 0x0}; // imm 0x0
        constexpr Type::IEnv EBreak = Type::IEnv{.funct3 = 0x0, .imm = 0x1}; // imm 0x1

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
            return GetField(5U, 11U, imm) == 0x20;
        }

        Instruction DecodeILogic(Uint binInstruction) {
            Uint funct3 = GetFunct3(binInstruction);
            RegIdx rd = GetFunct3(binInstruction);
            RegIdx rs1 = GetFunct3(binInstruction);
            RegIdx imm = GetImmTypeI(binInstruction);
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
            RegIdx rd = GetFunct3(binInstruction);
            RegIdx rs1 = GetFunct3(binInstruction);
            RegIdx imm = GetImmTypeI(binInstruction);

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
            RegIdx rd = GetFunct3(binInstruction);
            RegIdx rs1 = GetFunct3(binInstruction);
            RegIdx imm = GetImmTypeI(binInstruction);

            std::cerr << "Jalr\n";
            return Instruction{                            
                .PFN_Instruction = InstructionSet::Jalr,
                .param1 = rd,
                .param2 = rs1,
                .param3 = imm};
        }

        Instruction DecodeIEnv(Uint binInstruction) {
            Uint funct3 = GetFunct3(binInstruction);
            RegIdx rd = GetFunct3(binInstruction);
            RegIdx rs1 = GetFunct3(binInstruction);
            RegIdx imm = GetImmTypeI(binInstruction);

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

        Instruction Decode(Uint binInstruction) {
            std::cerr << std::bitset<32>{binInstruction} << '\n';
            Uint const opcode = GetOpcode(binInstruction);

            switch (opcode)
            {
                case Type::R::Opcode:
                    std::cerr << "R instruction\n";
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

                default:
                    std::cerr << "Unknown instruction: " << std::bitset<32>{opcode} << '\n';
                    break;
            }

            return Instruction{};
        }
    } // Decoder

}; // RISCVS