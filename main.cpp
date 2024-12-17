#include <iostream>
#include <Decoder.hpp>
#include <hart.hpp>
#include <machine.hpp>
#include <cstdio>

int main(int argc, const char* argv[]) {
    using namespace RISCVS;

    int32_t pcInitValue = 0;
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

    /* Load file into RAM */

    /* Start execution from RAM */

    Machine machine{};
    Hart hart{machine, pcInitValue};

    for (int i = 0; !hart.IsStop(); ++i) {
        hart.Execute();
        // char x = getchar();
        // if (x == 'q') {
        //     break;
        // }
    }

    hart.Dump();

    // Decoder::TestDecoder();
}
