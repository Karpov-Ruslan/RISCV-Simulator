#include <Decoder.hpp>
#include <hart.hpp>
#include <machine.hpp>

int main(int argc, const char* argv[]) {
    using namespace RISCVS;

    /* Load file into RAM */

    /* Start execution from RAM */

    Machine machine{};
    Hart hart{machine};

    for (int i = 0; i < 100; ++i) {
        hart.Execute();
    }

    hart.Dump();

    // Decoder::TestDecoder();
}
