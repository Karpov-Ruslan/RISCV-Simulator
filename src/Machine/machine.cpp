#include "machine.hpp"

#ifdef MMAP
    #include <sys/mman.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #include <errno.h>
    #include <cstring>
#endif // MMAP

namespace RISCVS {

Machine::Machine() : ram(RAM_PATH) {
    if (!ram.is_open()) {
        throw std::runtime_error("RAM was not opened");
    }
}

Machine::Machine(std::string_view code_path, uint32_t loadOffset) {
    ;    
#ifdef MMAP
    this->loadOffset_ = loadOffset;
    useFile_ = false;

    int fd = open(code_path.data(), O_RDONLY);
    if (fd == -1) {
        throw std::runtime_error("Failed to open code.bin");
    }

    // Get file size
    off_t fileSize = lseek(fd, 0, SEEK_END);
    if (fileSize == -1) {
        close(fd);
        throw std::runtime_error("Failed to get file size");
    }
    lseek(fd, 0, SEEK_SET);

    // Read the file content and write it to the shared memory
    char* buffer = new char[fileSize];
    ssize_t bytesRead = read(fd, buffer, fileSize);
    if (bytesRead != fileSize) {
        delete[] buffer;
        close(fd);
        throw std::runtime_error("Failed to read entire file");
    }

    // printf("File size: %ld\n", fileSize);

    void *mmapRam = mmap(NULL, 16ULL * 1024 * 1024 * 1024,
                        PROT_READ | PROT_WRITE,
                        MAP_ANONYMOUS | MAP_PRIVATE,
                        -1, 0);

    if (mmapRam == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        throw std::runtime_error("Failed to map memory");
    }

    // Write the buffer to memory
    memcpy((char*) mmapRam + loadOffset, buffer, fileSize);
    delete[] buffer;

    // printf("Successfully mapped 16GB of anonymous memory: %p with offset %d\n", mmapRam, loadOffset);
    // std::cout << "Try read: " << std::bitset<32>{((uint32_t*)mmapRam)[loadOffset_/4U]} << '\n';

    this->mmapRam_ = static_cast<uint32_t*>(mmapRam);
#endif // MMAP
}

Machine::~Machine() {
    if (useFile_) {
        ram.close();
    } else {
        ;
    #ifdef MMAP
        munmap(mmapRam_, 16ULL * 1024 * 1024 * 1024);
    #endif // MMAP
    }
}

} // namespace RISCVS