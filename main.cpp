#include <iostream>
#include <Decoder.hpp>
#include <hart.hpp>
#include <machine.hpp>
#include <cstdio>
#include <chrono>

int main(int argc, const char* argv[]) {
    using namespace RISCVS;

#ifdef MMAP
    uint32_t loadOffset = 0x10094;
    Machine machine{"../ram/code.bin", loadOffset};
#else // MMAP
    Machine machine{};
#endif // MMAP

    // Machine machine{};
    Hart hart{machine, 0x100d8};

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
