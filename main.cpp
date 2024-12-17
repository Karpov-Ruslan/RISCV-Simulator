#include <iostream>
#include <Decoder.hpp>
#include <hart.hpp>
#include <machine.hpp>
#include <cstdio>

int main(int argc, const char* argv[]) {
    using namespace RISCVS;

    /* Load file into RAM */

    /* Start execution from RAM */

    Machine machine{};
    Hart hart{machine};

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
