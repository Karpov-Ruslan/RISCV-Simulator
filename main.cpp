#include <iostream>
#include <Decoder.hpp>
#include <hart.hpp>
#include <machine.hpp>
#include <cstdio>
#include <chrono>

int main(int argc, const char* argv[]) {
    using namespace RISCVS;

    int32_t pcInitValue = 0x100d8;
    for (size_t i = 0; i < argc; i++) {
        const auto cmdArg = std::string_view(argv[i]);

        // Flag has 1 parameter
        if (i + 1 < argc) {
            if (cmdArg == "--pc") {
                // std::stoi does not support std::string_view (it is so stupid)
                // std::atoi is UB-generator :)
                pcInitValue = std::stoi(std::string(argv[i + 1]));
            }
        }
      }

  
#ifdef MMAP
    uint32_t loadOffset = 0x10094;
    Machine machine{"../ram/code.bin", loadOffset};
#else // MMAP
    Machine machine{};
#endif // MMAP

    Hart hart{machine, pcInitValue};

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; !hart.IsStop(); ++i) {
        hart.Execute();
        // hart.Dump(18);
        // char x = getchar();
        // if (x == 'q') {
        //     break;
        // }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Execution time: " << duration.count() << " milliseconds" << std::endl;


    hart.Dump();

    // Decoder::TestDecoder();
}
