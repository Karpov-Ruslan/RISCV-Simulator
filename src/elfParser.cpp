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


struct SectionInfo {
    std::string name;
    uint64_t addr;
    uint64_t size;
    std::vector<char> data; // For .text and .data
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
    void extractBinary(const std::string& outputFile);
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
        if (name == ".text" || name == ".data" || name == ".bss") {
            SectionInfo sec;
            sec.name = name;
            sec.addr = shdr[i].sh_addr;
            sec.size = shdr[i].sh_size;

            if (shdr[i].sh_type != SHT_NOBITS) {
                const char* secData = reinterpret_cast<const char*>(ehdr) + shdr[i].sh_offset;
                sec.data.assign(secData, secData + shdr[i].sh_size);
            }

            sections.push_back(sec);
        }
    }
    return true;
}

void ElfParser::extractBinary(const std::string& outputFile) {
    if (sections.empty()) {
        std::cerr << "No sections to process.\n";
        return;
    }

    uint64_t baseAddr = sections[0].addr;
    uint64_t endAddr = sections[0].addr + sections[0].size;

    for (const auto& sec : sections) {
        baseAddr = std::min(baseAddr, sec.addr);
        endAddr = std::max(endAddr, sec.addr + sec.size);
    }

    uint64_t totalSize = endAddr - baseAddr;
    std::vector<char> binary(totalSize, 0);

    for (const auto& sec : sections) {
        if (!sec.data.empty()) {
            uint64_t offset = sec.addr - baseAddr;
            std::copy(sec.data.begin(), sec.data.end(), binary.begin() + offset);
        }
    }

    int outFd = open(outputFile.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);

    if (write(outFd, binary.data(), binary.size()) != static_cast<ssize_t>(binary.size())) {
        std::cerr << "Error writing to output file.\n";
    }

    close(outFd);
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


// int main(int argc, char** argv) {
//     ElfParser parser(argv[1]);

//     if (!parser.parse()) {
//         return 1;
//     }

//     parser.printElfInfo();
//     parser.extractBinary(argv[2]);
//     return 0;
// }
