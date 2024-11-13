#pragma once

#include <cstdint>
#include <array>
#include <machine.hpp>

namespace RISCVS {

class Hart {
public:
    using Register = uint32_t;
    using RegisterIndex = uint16_t;
    constexpr static uint16_t NUM_REGISTER = 32U;

    explicit Hart(Machine& machine) : machine(machine) {}

    Register& operator[](const RegisterIndex idx) {
        return reg[idx];
    }

    [[nodiscard]] uint32_t Load(const int64_t memoryRef) const {
        return machine.Load<uint32_t>(memoryRef);
    }

    void Store(const int64_t memoryRef, const uint32_t value) const {
        machine.Store(memoryRef, value);
    }



private:
    // TODO: FUCKFUCKFUCKFUCKFUCK (Register numba 0 is not always zero)
    std::array<Register, NUM_REGISTER> reg{};

    Machine& machine;
};

}