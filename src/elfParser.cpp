#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>

struct SectionInfo {
    std::string name;
    uint64_t addr;
    uint64_t size;
    std::vector<char> data; // For .text, .data, .rodata, and .bss
};

class ElfParser {
public:
    explicit ElfParser(const std::string& filepath) : filepath(filepath), map(nullptr), filesize(0) {}

    ~ElfParser() {
        if (map) {
            munmap(map, filesize);
        }
    }

    bool parse();
    uint64_t getTextSectionAddress();
    void extractBinaryRAM(const std::string& outputFile, uint64_t baseAddress);
    void extractBinaryMMAP(const std::string& outputFile, uint64_t baseAddress);
    void printElfInfo();

private:
    std::string filepath;
    void* map;
    size_t filesize;
    std::vector<SectionInfo> sections;

    template <typename Ehdr, typename Shdr>
    bool processElf(const Ehdr* ehdr, const Shdr* shdr, const char* shstrtab);

    static std::string getSectionName(const char* strtab, size_t offset) {
        return std::string(strtab + offset);
    }
};

bool ElfParser::parse() {
    int fd = open(filepath.c_str(), O_RDONLY);
    if (fd < 0) {
        perror("open");
        return false;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat");
        close(fd);
        return false;
    }

    filesize = st.st_size;
    map = mmap(nullptr, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return false;
    }

    close(fd);
    unsigned char* data = static_cast<unsigned char*>(map);

    if (memcmp(data, ELFMAG, SELFMAG) != 0) {
        std::cerr << "File is not an ELF file.\n";
        return false;
    }

    bool is64Bit = data[EI_CLASS] == ELFCLASS64;

    if (is64Bit) {
        auto* ehdr = reinterpret_cast<Elf64_Ehdr*>(data);
        auto* shdr = reinterpret_cast<Elf64_Shdr*>(data + ehdr->e_shoff);
        const char* shstrtab = reinterpret_cast<const char*>(data + shdr[ehdr->e_shstrndx].sh_offset);
        return processElf(ehdr, shdr, shstrtab);
    } else {
        auto* ehdr = reinterpret_cast<Elf32_Ehdr*>(data);
        auto* shdr = reinterpret_cast<Elf32_Shdr*>(data + ehdr->e_shoff);
        const char* shstrtab = reinterpret_cast<const char*>(data + shdr[ehdr->e_shstrndx].sh_offset);
        return processElf(ehdr, shdr, shstrtab);
    }
}

template <typename Ehdr, typename Shdr>
bool ElfParser::processElf(const Ehdr* ehdr, const Shdr* shdr, const char* shstrtab) {
    for (int i = 0; i < ehdr->e_shnum; ++i) {
        std::string name = getSectionName(shstrtab, shdr[i].sh_name);
        if (name == ".text" || name == ".data" || name == ".bss" || name == ".rodata" ||
            name == ".sdata" || name == ".sbss" || name == ".init_array" || name == ".fini_array") {
            SectionInfo sec;
            sec.name = name;
            sec.addr = shdr[i].sh_addr;
            sec.size = shdr[i].sh_size;

            if (shdr[i].sh_type != SHT_NOBITS) {
                const char* secData = reinterpret_cast<const char*>(ehdr) + shdr[i].sh_offset;
                sec.data.assign(secData, secData + shdr[i].sh_size);
            } else {
                sec.data.resize(shdr[i].sh_size, 0); // For .bss and .sbss - initialisation with zeros
            }

            sections.push_back(sec);
        }
    }
    return true;
}

uint64_t ElfParser::getTextSectionAddress() {
    for (const auto& sec : sections) {
        if (sec.name == ".text") {
            return sec.addr;
        }
    }
    throw std::runtime_error("Section .text not found.");
}

void ElfParser::extractBinaryRAM(const std::string& outputFile, uint64_t baseAddress) {
    if (sections.empty()) {
        std::cerr << "No sections to process.\n";
        return;
    }

    int outFd = open(outputFile.c_str(), O_RDWR);
    if (outFd < 0) {
        perror("open (output)");
        return;
    }

    uint64_t text_addr = getTextSectionAddress();

    for (const auto& sec : sections) {
        uint64_t offset = baseAddress + (sec.addr - text_addr);

        if (lseek(outFd, offset, SEEK_SET) == -1) {
            perror("lseek");
            close(outFd);
            return;
        }

        if (!sec.data.empty()) {
            ssize_t written = write(outFd, sec.data.data(), sec.data.size());
            if (written != static_cast<ssize_t>(sec.data.size())) {
                std::cerr << "Error writing section " << sec.name << ".\n";
                close(outFd);
                return;
            }
            std::cout << "Section " << sec.name << " written at offset: 0x" << std::hex << offset << "\n";
        }
    }

    close(outFd);
    std::cout << "Sections successfully written to RAM file: " << outputFile << "\n";
}

void ElfParser::extractBinaryMMAP(const std::string& outputFile, uint64_t baseAddress) {
    if (sections.empty()) {
        std::cerr << "No sections to process.\n";
        return;
    }

    
    int outFd = open(outputFile.c_str(), O_RDWR | O_CREAT, 0644);
    if (outFd < 0) {
        perror("open (output)");
        return;
    }

    
    uint64_t highestAddress = 0;
    for (const auto& sec : sections) {
        highestAddress = std::max(highestAddress, sec.addr + sec.size);
    }

    
    if (ftruncate(outFd, highestAddress) == -1) {
        perror("ftruncate");
        close(outFd);
        return;
    }

    uint64_t text_addr = getTextSectionAddress();
    
    for (const auto& sec : sections) {
        uint64_t offset = baseAddress + (sec.addr - text_addr);

        
        if (lseek(outFd, offset, SEEK_SET) == -1) {
            perror("lseek");
            close(outFd);
            return;
        }

        if (!sec.data.empty()) {
            ssize_t written = write(outFd, sec.data.data(), sec.data.size());
            if (written != static_cast<ssize_t>(sec.data.size())) {
                std::cerr << "Error writing section " << sec.name << ".\n";
                close(outFd);
                return;
            }
            std::cout << "Section " << sec.name << " written at offset: 0x" << std::hex << offset << "\n";
        }
    }

    close(outFd);
    std::cout << "Sections successfully written to RAM file: " << outputFile << "\n";
}



void ElfParser::printElfInfo() {
    if (!map) {
        std::cerr << "ELF file not loaded.\n";
        return;
    }

    unsigned char* data = static_cast<unsigned char*>(map);
    auto* elfHeader = reinterpret_cast<Elf64_Ehdr*>(data);

    std::cout << "ELF Header Information:" << std::endl;
    std::cout << "  Class:          " << (elfHeader->e_ident[EI_CLASS] == ELFCLASS64 ? "ELF64" : "ELF32") << std::endl;
    std::cout << "  Data:           " << (elfHeader->e_ident[EI_DATA] == ELFDATA2LSB ? "Little Endian" : "Big Endian") << std::endl;
    std::cout << "  Type:           " << elfHeader->e_type << std::endl;
    std::cout << "  Machine:        " << elfHeader->e_machine << std::endl;
    std::cout << "  Entry point:    0x" << std::hex << elfHeader->e_entry << std::endl;

    std::cout << "\nSection Headers (" << sections.size() << "):" << std::endl;
    for (const auto& section : sections) {
        std::cout << "  Name: " << section.name << ", Addr: 0x" << std::hex << section.addr << ", Size: " << std::dec << section.size << std::endl;
    }
}


int main(int argc, char** argv) {
    bool flag = false;

    if (argc < 3 || argc > 4) {
        std::cerr << "Usage: " << argv[0] << " <input.elf> <output.ram> [--ram]\n";
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];

    if (argc == 4 && std::string(argv[3]) == "--ram") {
        flag = true;
    }
    
    ElfParser parser(inputFile);

    if (!parser.parse()) {
        return 1;
    }

    // parser.printElfInfo();
    
    if (flag) {
        parser.extractBinaryRAM(outputFile, 0x10094);
    } else {
        parser.extractBinaryMMAP(outputFile, 0x0);
    }

    return 0;
}

