#pragma once
#include "../instruction.hpp"

#include <iostream>

using Uint = unsigned;
namespace RISCVS {

    using RegIdx = uint16_t;
    using Imm = uint32_t;

    namespace Decoder {

        Uint GetOpcode(Uint code);
        bool TestGetField();
        Instruction Decode(Uint binInstruction);


        namespace Type {
            struct R {
                static constexpr Uint Opcode = 0b110011;

                const Uint funct3;
                const Uint funct7;

                Uint Build(RegIdx rd, RegIdx rs1, RegIdx rs2) const;
            };

            struct ILogic {
                static constexpr Uint Opcode = 0b0010011;
                const Uint funct3;

                Uint Build(RegIdx rd, RegIdx rs1, Imm imm) const;
            };

            struct ILoad {
                static constexpr Uint Opcode  = 0b0000011;
                const Uint funct3;

                Uint Build(RegIdx rd, RegIdx rs1, Imm imm) const;
            };

            struct IJump {
                static constexpr Uint Opcode  = 0b1100111;
                const Uint funct3;

                Uint Build(RegIdx rd, RegIdx rs1, Imm imm) const;
            };

            struct IEnv {
                static constexpr Uint Opcode = 0b1110011;
                const Uint funct3;
                const Uint imm;

                Uint Build(RegIdx rd, RegIdx rs1, Imm imm) const;
            };
        } // Type

        constexpr Type::R Add  = Type::R{.funct3 = 0x0, .funct7 = 0x00};
        constexpr Type::R Sub  = Type::R{.funct3 = 0x0, .funct7 = 0x20};
        constexpr Type::R Xor  = Type::R{.funct3 = 0x4, .funct7 = 0x00};
        constexpr Type::R Or   = Type::R{.funct3 = 0x6, .funct7 = 0x00};
        constexpr Type::R And  = Type::R{.funct3 = 0x7, .funct7 = 0x00};
        constexpr Type::R Sll  = Type::R{.funct3 = 0x1, .funct7 = 0x00};
        constexpr Type::R Srl  = Type::R{.funct3 = 0x5, .funct7 = 0x00};
        constexpr Type::R Sra  = Type::R{.funct3 = 0x5, .funct7 = 0x20};
        constexpr Type::R Slt  = Type::R{.funct3 = 0x2, .funct7 = 0x00};
        constexpr Type::R Sltu = Type::R{.funct3 = 0x3, .funct7 = 0x00};

    } // Decoder

} // RISCVS
