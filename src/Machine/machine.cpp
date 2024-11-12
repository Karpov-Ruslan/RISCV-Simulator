#include "machine.hpp"

namespace RISCVS {

Machine::Machine() : ram(RAM_PATH) {
    if (!ram.is_open()) {
        throw std::runtime_error("RAM was not opened");
    }
}

Machine::~Machine() {
    ram.close();
}


}