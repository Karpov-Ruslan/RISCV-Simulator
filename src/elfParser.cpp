#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <elf.h>


class ElfParser {
private:
    std::string filename;
    std::vector<uint8_t> fileContent;
    Elf64_Ehdr* elfHeader;
    std::vector<Elf64_Phdr*> programHeaders;
    std::vector<Elf64_Shdr*> sectionHeaders;
    char* stringTable;

public:
    ElfParser(const std::string& fname) : filename(fname) {
        readFile();
    }

    void readFile() {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }

        size_t fileSize = file.tellg();
        fileContent.resize(fileSize);

        file.seekg(0);
        file.read(reinterpret_cast<char*>(fileContent.data()), fileSize);
        file.close();

        parseElfHeader();
    }

    void parseElfHeader() {
        if (fileContent.size() < sizeof(Elf64_Ehdr)) {
            std::cerr << "File too small for ELF header" << std::endl;
            return;
        }

        elfHeader = reinterpret_cast<Elf64_Ehdr*>(fileContent.data());

        // Check ELF magic number
        if (memcmp(elfHeader->e_ident, ELFMAG, SELFMAG) != 0) {
            std::cerr << "Invalid ELF magic number" << std::endl;
            return;
        }

        parseProgramHeaders();
        parseSectionHeaders();
    }

    void parseProgramHeaders() {
        if (elfHeader->e_phnum == 0) {
            return;
        }

        for (int i = 0; i < elfHeader->e_phnum; i++) {
            Elf64_Phdr* ph = reinterpret_cast<Elf64_Phdr*>(
                fileContent.data() + elfHeader->e_phoff + (i * elfHeader->e_phentsize)
            );
            programHeaders.push_back(ph);
        }
    }

    void parseSectionHeaders() {
        if (elfHeader->e_shnum == 0) {
            return;
        }

        for (int i = 0; i < elfHeader->e_shnum; i++) {
            Elf64_Shdr* sh = reinterpret_cast<Elf64_Shdr*>(
                fileContent.data() + elfHeader->e_shoff + (i * elfHeader->e_shentsize)
            );
            sectionHeaders.push_back(sh);
        }

        // Get string table
        if (elfHeader->e_shstrndx != SHN_UNDEF) {
            stringTable = reinterpret_cast<char*>(
                fileContent.data() + sectionHeaders[elfHeader->e_shstrndx]->sh_offset
            );
        }
    }

    void printElfInfo() {
        std::cout << "ELF Header Information:" << std::endl;
        std::cout << "  Class:          " << (elfHeader->e_ident[EI_CLASS] == ELFCLASS64 ? "ELF64" : "ELF32") << std::endl;
        std::cout << "  Data:           " << (elfHeader->e_ident[EI_DATA] == ELFDATA2LSB ? "Little Endian" : "Big Endian") << std::endl;
        std::cout << "  Type:           " << elfHeader->e_type << std::endl;
        std::cout << "  Machine:        " << elfHeader->e_machine << std::endl;
        std::cout << "  Entry point:    0x" << std::hex << elfHeader->e_entry << std::endl;

        std::cout << "\nProgram Headers (" << programHeaders.size() << "):" << std::endl;
        for (const auto& ph : programHeaders) {
            std::cout << "  Type: " << ph->p_type << ", Offset: 0x" << std::hex << ph->p_offset 
                      << ", VirtAddr: 0x" << ph->p_vaddr << ", FileSize: " << std::dec << ph->p_filesz << std::endl;
        }

        std::cout << "\nSection Headers (" << sectionHeaders.size() << "):" << std::endl;
        for (const auto& sh : sectionHeaders) {
            std::string name = stringTable ? (stringTable + sh->sh_name) : "Unknown";
            std::cout << "  Name: " << name << ", Type: " << sh->sh_type 
                      << ", Addr: 0x" << std::hex << sh->sh_addr << ", Size: " << std::dec << sh->sh_size << std::endl;
        }
    }

    std::vector<uint8_t> getSectionContent(const std::string& sectionName) {
        for (const auto& sh : sectionHeaders) {
            std::string name = stringTable ? (stringTable + sh->sh_name) : "";
            if (name == sectionName) {
                std::vector<uint8_t> content(sh->sh_size);
                memcpy(content.data(), fileContent.data() + sh->sh_offset, sh->sh_size);
                return content;
            }
        }
        return std::vector<uint8_t>();
    }

    bool dumpSectionsToFile(const char* filename) {
        std::ofstream file(filename, std::ios::binary | std::ios::out);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open output file " << filename << std::endl;
            return false;
        }

        // Find required sections
        Elf64_Shdr* textSection = nullptr;
        Elf64_Shdr* dataSection = nullptr;
        Elf64_Shdr* bssSection = nullptr;

        for (const auto& sh : sectionHeaders) {
            std::string name = stringTable ? (stringTable + sh->sh_name) : "";
            if (name == ".text") textSection = sh;
            else if (name == ".data") dataSection = sh;
            else if (name == ".bss") bssSection = sh;
        }

        // Write .text section
        if (textSection) {
            const uint8_t* textData = fileContent.data() + textSection->sh_offset;
            file.write(reinterpret_cast<const char*>(textData), textSection->sh_size);
            std::cout << "Written .text section: " << textSection->sh_size << " bytes" << std::endl;
        }

        // Write .data section
        if (dataSection) {
            const uint8_t* dataContent = fileContent.data() + dataSection->sh_offset;
            file.write(reinterpret_cast<const char*>(dataContent), dataSection->sh_size);
            std::cout << "Written .data section: " << dataSection->sh_size << " bytes" << std::endl;
        }

        // Write .bss section (zeros)
        if (bssSection) {
            std::vector<uint8_t> zeros(bssSection->sh_size, 0);
            file.write(reinterpret_cast<const char*>(zeros.data()), bssSection->sh_size);
            std::cout << "Written .bss section: " << bssSection->sh_size << " bytes" << std::endl;
        }

        file.close();
        return true;
    }
};


bool elfParserChecker(char* fname, char* ramFile) {
    ElfParser parser(fname);
    parser.printElfInfo();

    if (!parser.dumpSectionsToFile(ramFile)) {
        return false;
    }

    return true;
}
