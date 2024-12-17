#pragma once

#include <type_traits>
#include <fstream>
#include <bitset>
#include <iostream>
#include <string_view>
#include <cstdint>

#include <defines.hpp>

// #define MMAP

namespace RISCVS {

template<typename T, typename ... U>
concept DataTypeRequirementsBase = (std::is_same_v<T, U> || ...);

template<typename T>
concept DataTypeRequirements = DataTypeRequirementsBase<T, PARAM_TYPES>;

#define MACHINE_ATTR template<typename T> requires DataTypeRequirements<T>

class Machine {
public:

    Machine();

    Machine(std::string_view code_path, uint32_t loadOffset);

    ~Machine();

    MACHINE_ATTR void Store(const int32_t memoryRef, const T data) {
        // std::cerr << "MemoryRef: " << (unsigned) memoryRef << '\n';
        if (useFile_) {
            ram.seekp((unsigned) memoryRef);

            if (ram.fail()) {
                throw "seekp failed\n";
            }

            using bitset = std::bitset<8*sizeof(T)>;
            T temp = data;
            ram.write(reinterpret_cast<char*>(&temp), sizeof(T));

            if (ram.fail()) {
                throw "write failed\n";
            }
        } else {
            // std::cerr << "Read: " << mmapRam_ + memoryRef/4U + loadOffset_/4U << '\n'; 
            mmapRam_[memoryRef/4U] = data;
        }
    }

    MACHINE_ATTR T Load(const int32_t memoryRef) {
        if (useFile_) {
            ram.seekg((unsigned) memoryRef);

            if (ram.fail()) {
                throw "seekp failed\n";
            }

            T ret;
            ram.read(reinterpret_cast<char*>(&ret), sizeof(T));

            if (ram.fail()) {
                throw "read failed\n";
            }

            return ret;
        } else {
            // std::cerr << "From: " << std::hex << memoryRef << '\n';
            uint32_t read = mmapRam_[memoryRef/4U];
            // std::cerr << memoryRef << '\n';
            // std::cerr << std::bitset<32>{read} << '\n';
            return read;
        }
    }

private:
    bool useFile_ = true;
    uint32_t loadOffset_ = 0;
    uint32_t* mmapRam_ = nullptr;

    const char* RAM_PATH = "../ram/ram.bin";

    std::fstream ram;
};

}
