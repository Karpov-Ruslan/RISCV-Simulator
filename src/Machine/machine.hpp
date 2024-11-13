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
#ifdef INV_MEMORY_ORDER
        std::string bit_repr = bitset(data).to_string();
        std::cout << bit_repr << std::endl;
        std::reverse(bit_repr.begin(), bit_repr.end());
        std::cout << bit_repr << std::endl;
        temp = static_cast<T>(bitset(bit_repr).to_ullong());
#endif
        ram.write(reinterpret_cast<char*>(&temp), sizeof(T));
    }

    MACHINE_ATTR T Load(const int32_t memoryRef) {
        ram.seekg(memoryRef);
        T ret;
        ram.read(reinterpret_cast<char*>(&ret), sizeof(T));
        // No working code)
        // std::string bit_repr = std::bitset<sizeof(T)>(ret).to_string();
        // std::cout << bit_repr << std::endl;
        // std::reverse(bit_repr.begin(), bit_repr.end());
        // ret = static_cast<T>(std::bitset<sizeof(T)>(bit_repr).to_ullong());
        return ret;
    }

private:
    const char* RAM_PATH = "../ram/ram.txt";

    std::fstream ram;
};

}
