#pragma once

#include <cstdint>
#include <array>
#include <type_traits>

#include <machine.hpp>
#include <Decoder.hpp>
#include "register.hpp"

namespace RISCVS {

class Hart {
public:
    using RegisterIndex = uint16_t;

    constexpr static RegisterIndex NUM_REGISTER = 32U;

    explicit Hart(Machine& machine, int32_t programCounter = 0) : machine(machine), pc{programCounter} {}

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

    int32_t Load(const int32_t memoryRef) const {
        return machine.Load<int32_t>(memoryRef);
    }

    int32_t M(const int32_t memoryRef) const {
        return Load(memoryRef);
    }

    [[nodiscard]] uint32_t Fetch(const int32_t memoryRef) const {
        return Load(memoryRef);
    }

    template<typename T>
    void Store(const int32_t memoryRef, const T value) const {
        machine.Store<T>(memoryRef, value);
    }

    void Execute(bool requireSkip = false) {
        if (!IsStop()) {
            uint32_t binInstruction = Load(pc);
            Instruction instruction = Decoder::Decode(binInstruction);
            bool shiftPC = instruction.PFN_Instruction(*this, instruction.param1, instruction.param2, instruction.param3);
            
            // NextInstructionPC();
            if (shiftPC && !requireSkip) {
                NextInstructionPC();
            }
        }
        // Dump();
    }

    void Dump(int max_reg = 32) const {

        if (max_reg > 32) {
            return;
        }

        std::cout << "++++++++DUMP_STATE++++++++\n";
        std::cout << "pc: " << pc << '\n';
        for (int reg_idx = 0; reg_idx < max_reg; ++reg_idx) {
            std::cout << "x[" << reg_idx << "] = " << reg[reg_idx] << '\n';
        }
        std::cout << "++++++++++++++++++++++++++\n";
    }

    void Stop() {
        isHalt = true;
    }

    void Run() {
        isHalt = false;
    }

    bool IsStop() const {
        return isHalt;
    }

private:
    std::array<Register, NUM_REGISTER> reg{Register::REGISTER_MODE::ZERO, Register::REGISTER_MODE::DEFAULT};
    int32_t pc = 0x100d8; 

    Machine& machine;
    bool isHalt = false;
};

}