#pragma once

#include <type_traits>
#include <fstream>
#include <bitset>
#include <iostream>
#include <string_view>
#include <cstdint>

#include <defines.hpp>

namespace RISCVS {

template<typename T, typename ... U>
concept DataTypeRequirementsBase = (std::is_same_v<T, U> || ...);

template<typename T>
concept DataTypeRequirements = DataTypeRequirementsBase<T, PARAM_TYPES>;

#define MACHINE_ATTR template<typename T> requires DataTypeRequirements<T>

class Machine {
public:
    Machine();

    ~Machine();

    MACHINE_ATTR void Store(const int32_t memoryRef, const T data) {
        ram.seekp(memoryRef);
        using bitset = std::bitset<8*sizeof(T)>;
        T temp = data;
        ram.write(reinterpret_cast<char*>(&temp), sizeof(T));
    }

    MACHINE_ATTR T Load(const int32_t memoryRef) {
        ram.seekg(memoryRef);
        T ret;
        ram.read(reinterpret_cast<char*>(&ret), sizeof(T));
        return ret;
    }

private:
    const char* RAM_PATH = "../ram/ram.bin";

    std::fstream ram;
};

}
