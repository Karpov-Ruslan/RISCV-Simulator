#pragma once

#include <type_traits>

#include "../instruction.hpp"

namespace RISCVS {

template<typename T>
concept DataTypeRequirements = std::conjunction_v<std::disjunction<PARAM_TYPES>, T>;

#define MACHINE_ATTR template<typename T> requires DataTypeRequirements<T>

class Machine {
public:
    Machine();

    MACHINE_ATTR void Store(uint64_t memoryRef, T data);

    MACHINE_ATTR T Load(uint64_t memoryRef);

private:
    // TODO: add file system
};

}
