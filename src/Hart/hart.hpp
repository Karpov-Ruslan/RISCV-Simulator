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

    constexpr static RegisterIndex NUM_REGISTER = 32U;

    explicit Hart(Machine& machine, int32_t programCounter) : machine(machine) {}

    Register& operator[](const RegisterIndex idx) {
        return reg[idx];
    }

    void MovePC(const int32_t offset) {
        // God, please, cast unsigned to signed
        pc += offset;
    }

    void SetPC(const int32_t newPC) {
        pc = newPC;
    }

    [[nodiscard]] int32_t GetPC() const {
        return pc;
    }

    void NextInstructionPC() {
        MovePC(sizeof(uint32_t));
    }

    template<typename T>
    [[nodiscard]] T Load(const int32_t memoryRef) const {
        return machine.Load<T>(memoryRef);
    }

    [[nodiscard]] uint32_t Fetch(const int32_t memoryRef) const {
        return Load<uint32_t>(memoryRef);
    }

    template<typename T>
    void Store(const int32_t memoryRef, const T value) const {
        machine.Store<T>(memoryRef, value);
    }

    void Execute() {
        uint32_t binInstruction = machine.Load<uint32_t>(pc);
        Instruction instruction = Decoder::Decode(binInstruction);
        NextInstructionPC();
        instruction.PFN_Instruction(*this, instruction.param1, instruction.param2, instruction.param3);
    }

private:
    // TODO: FUCKFUCKFUCKFUCKFUCK (Register numba 0 is not always zero)
    std::array<Register, NUM_REGISTER> reg{};
    int32_t pc;

    Machine& machine;
};

}