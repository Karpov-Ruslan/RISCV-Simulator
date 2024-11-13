#pragma once

#include <cstdint>
#include <array>
#include <type_traits>

#include <machine.hpp>
#include <Decoder.hpp>

namespace RISCVS {

class Hart {
public:
    using Register = uint32_t;
    using RegisterIndex = uint16_t;

    constexpr static uint16_t NUM_REGISTER = 32U;

    explicit Hart(Machine& machine, int32_t programCounter) : machine(machine) {}

    Register& operator[](const RegisterIndex idx) {
        return reg[idx];
    }

    void MovePC(int32_t offset) {
        // God, please, cast unsigned to signed
        pc += offset;
    }

    [[nodiscard]] uint32_t Load(const int64_t memoryRef) const {
        return machine.Load<uint32_t>(memoryRef);
    }

    void Store(const int64_t memoryRef, const uint32_t value) const {
        machine.Store(memoryRef, value);
    }

    void Execute() {
        uint32_t binInstruction = machine.Load<uint32_t>(pc);
        Instruction instruction = Decoder::Decode(binInstruction);
        instruction.PFN_Instruction(*this, instruction.param1, instruction.param2, instruction.param3);
    }

private:
    // TODO: FUCKFUCKFUCKFUCKFUCK (Register numba 0 is not always zero)
    std::array<Register, NUM_REGISTER> reg{};
    int32_t pc;

    Machine& machine;
};

}