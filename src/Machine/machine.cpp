#include "machine.hpp"
#include <format>

namespace RISCVS {

Machine::Machine() : ram(RAM_PATH) {
    if (!ram.is_open()) {
        throw std::runtime_error("RAM was not opened");
    }

    InitSystemMemory();
}

Machine::~Machine() {
    ram.close();
}

Machine::MemoryRefType Machine::TranslateMemoryAddrIntoPageAddr(const MemoryRefType memoryAddr) {
    return ((memoryAddr >> PAGE_BASE) << PAGE_BASE);
}

Machine::PhysicalPageRefType Machine::PageTableSearch(const VirtualPageRefType pageVirtualAddr) {
    for (uint32_t i = 0; i < (1 << 20); i++) {
        auto selectedPageVirtualAddr = PhysicalLoad<uint32_t>(SYSTEM_MEMORY_ADDR + 2*i*sizeof(uint32_t));
        if (selectedPageVirtualAddr == pageVirtualAddr) {
            return static_cast<MemoryRefType>(PhysicalLoad<uint32_t>(SYSTEM_MEMORY_ADDR + (2*i + 1)*sizeof(uint32_t)));
        }
    }

    throw std::runtime_error(std::format("Page Table does not know page virtual address: {}", pageVirtualAddr));
}

void Machine::InitSystemMemory() {
    // Page Table Init
    for (uint32_t i = 0; i < (1 << 20); i++) {
        auto it = 2*i*sizeof(uint32_t);
        PhysicalStore<uint32_t, false>(SYSTEM_MEMORY_ADDR + 2*i*sizeof(uint32_t), i << 12);
        PhysicalStore<uint32_t, false>(SYSTEM_MEMORY_ADDR + (2*i + 1)*sizeof(uint32_t), i << 12);
    }
}


}