#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <iomanip>
#include <string>
#include <cstdint>

constexpr uint64_t SECTOR_SIZE = 2048;
constexpr char MAGIC_STRING[] = "MICROSOFT*XBOX*MEDIA";
constexpr size_t MAGIC_LENGTH = 20;

struct DirectoryEntryHeader {
    uint16_t leftOffset;      // Offset to left child (in 4-byte units)
    uint16_t rightOffset;     // Offset to right child (in 4-byte units)
    uint32_t startSector;     // Starting sector of file/directory
    uint32_t fileSize;        // Size in bytes
    uint8_t  attributes;      // File attributes
    uint8_t  nameLength;      // Length of filename
} __attribute__((packed));

struct Entry {
    DirectoryEntryHeader header;
    std::string name;
    std::string path;
    bool isDirectory;
};

uint32_t swapEndian32(uint32_t val) {
    return ((val & 0xFF000000) >> 24) |
           ((val & 0x00FF0000) >> 8)  |
           ((val & 0x0000FF00) << 8)  |
           ((val & 0x000000FF) << 24);
}

uint16_t swapEndian16(uint16_t val) {
    return ((val & 0xFF00) >> 8) | ((val & 0x00FF) << 8);
}

constexpr uint64_t MAGIC_OFFSET = 0x10000;  // Magic string offset within XISO header

uint64_t findMagicOffset(std::ifstream& file, uint64_t fileSize) {
    // These are LSEEK offsets - the image offset, not where magic is
    const uint64_t lseekOffsets[] = {
        0,              // Standard - image starts at 0, magic at 0x10000
        0x0FD90000,     // XGD2DVD / Redump - image at 0xFD90000, magic at 0xFDA0000
        0x02080000,     // XGD3
        0x18300000      // XGD1
    };

    char buffer[MAGIC_LENGTH];
    
    for (auto lseekOffset : lseekOffsets) {
        uint64_t magicPosition = MAGIC_OFFSET + lseekOffset;
        
        if (magicPosition + MAGIC_LENGTH > fileSize) continue;
        
        file.seekg(magicPosition, std::ios::beg);
        file.read(buffer, MAGIC_LENGTH);
        
        if (file.gcount() == MAGIC_LENGTH && 
            std::memcmp(buffer, MAGIC_STRING, MAGIC_LENGTH) == 0) {
            std::cout << "Found magic at file offset: 0x" << std::hex << magicPosition << std::dec << std::endl;
            std::cout << "Image offset (lseek): 0x" << std::hex << lseekOffset << std::dec << std::endl;
            return lseekOffset;  // Return the IMAGE offset, not the magic position!
        }
    }
    
    return UINT64_MAX;  // Not found
}

void parseDirectoryTree(std::ifstream& file, uint64_t imageOffset, 
                       uint64_t dirSector, uint64_t dirSize, uint64_t offset,
                       const std::string& parentPath, std::vector<Entry>& entries) {
    
    if (offset * 4 >= dirSize) return;
    
    uint64_t position = imageOffset + (dirSector * SECTOR_SIZE) + (offset * 4);
    
    file.seekg(position, std::ios::beg);
    
    DirectoryEntryHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(DirectoryEntryHeader));
    
    if (file.gcount() != sizeof(DirectoryEntryHeader)) return;
    
    // Check for padding
    if (header.leftOffset == 0xFFFF || header.nameLength == 0 || header.nameLength > 255) {
        return;
    }
    
    // Read filename
    std::vector<char> nameBuffer(header.nameLength);
    file.read(nameBuffer.data(), header.nameLength);
    std::string name(nameBuffer.data(), header.nameLength);
    
    // Process left child first
    if (header.leftOffset != 0) {
        parseDirectoryTree(file, imageOffset, dirSector, dirSize, header.leftOffset, parentPath, entries);
    }
    
    // Process current entry
    Entry entry;
    entry.header = header;
    entry.name = name;
    entry.path = parentPath.empty() ? name : parentPath + "/" + name;
    entry.isDirectory = (header.attributes & 0x10) != 0;
    entries.push_back(entry);
    
    // Recursively process subdirectory
    if (entry.isDirectory && header.fileSize > 0) {
        parseDirectoryTree(file, imageOffset, header.startSector, header.fileSize, 0, entry.path, entries);
    }
    
    // Process right child
    if (header.rightOffset != 0) {
        parseDirectoryTree(file, imageOffset, dirSector, dirSize, header.rightOffset, parentPath, entries);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <iso_file>" << std::endl;
        return 1;
    }
    
    std::string isoPath = argv[1];
    std::ifstream file(isoPath, std::ios::binary);
    
    if (!file) {
        std::cerr << "Failed to open: " << isoPath << std::endl;
        return 1;
    }
    
    // Get file size
    file.seekg(0, std::ios::end);
    uint64_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::cout << "File size: " << fileSize << " bytes (" << (fileSize / 1024 / 1024) << " MB)" << std::endl;
    
    // Find magic offset (this returns the IMAGE offset, not magic position)
    uint64_t imageOffset = findMagicOffset(file, fileSize);
    if (imageOffset == UINT64_MAX) {
        std::cerr << "Magic string not found - not a valid XISO" << std::endl;
        return 1;
    }
    
    // Read root directory info (magic is at imageOffset + MAGIC_OFFSET)
    file.seekg(imageOffset + MAGIC_OFFSET + MAGIC_LENGTH, std::ios::beg);
    uint32_t rootSector = 0, rootSize = 0;
    file.read(reinterpret_cast<char*>(&rootSector), 4);
    file.read(reinterpret_cast<char*>(&rootSize), 4);
    
    std::cout << "Raw root sector: 0x" << std::hex << rootSector << std::dec << " (" << rootSector << ")" << std::endl;
    std::cout << "Raw root size: 0x" << std::hex << rootSize << std::dec << " (" << rootSize << ")" << std::endl;
    
    // Xbox 360 ISOs use little-endian, so no swap needed on x86
    // But let's check both interpretations
    uint32_t rootSectorSwapped = swapEndian32(rootSector);
    uint32_t rootSizeSwapped = swapEndian32(rootSize);
    
    std::cout << "Swapped root sector: 0x" << std::hex << rootSectorSwapped << std::dec << " (" << rootSectorSwapped << ")" << std::endl;
    std::cout << "Swapped root size: 0x" << std::hex << rootSizeSwapped << std::dec << " (" << rootSizeSwapped << ")" << std::endl;
    
    std::cout << "\nUsing raw (little-endian) values:" << std::endl;
    
    std::cout << "Root directory sector: " << rootSector << " (0x" << std::hex << rootSector << std::dec << ")" << std::endl;
    std::cout << "Root directory size: " << rootSize << " bytes" << std::endl;
    
    // Dump first few bytes of root directory for inspection
    uint64_t rootDirOffset = imageOffset + (static_cast<uint64_t>(rootSector) * SECTOR_SIZE);
    std::cout << "Root directory offset in file: " << rootDirOffset << " (0x" << std::hex << rootDirOffset << std::dec << ")" << std::endl;
    
    file.seekg(rootDirOffset, std::ios::beg);
    std::vector<char> rootDirData(64);
    file.read(rootDirData.data(), 64);
    
    std::cout << "First 64 bytes of root directory:" << std::endl;
    for (int i = 0; i < 64; i += 16) {
        std::cout << std::hex << std::setfill('0');
        for (int j = 0; j < 16 && i + j < 64; j++) {
            std::cout << std::setw(2) << (static_cast<unsigned char>(rootDirData[i + j]) & 0xFF) << " ";
        }
        std::cout << "  ";
        for (int j = 0; j < 16 && i + j < 64; j++) {
            char c = rootDirData[i + j];
            std::cout << (c >= 32 && c < 127 ? c : '.');
        }
        std::cout << std::dec << std::endl;
    }
    std::cout << std::endl;
    
    // Parse directory tree
    std::vector<Entry> entries;
    parseDirectoryTree(file, imageOffset, rootSector, rootSize, 0, "", entries);
    
    std::cout << "Found " << entries.size() << " entries:" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    for (const auto& entry : entries) {
        std::cout << (entry.isDirectory ? "[DIR] " : "[FILE]")
                  << std::setw(10) << entry.header.fileSize << " bytes  "
                  << entry.path << std::endl;
    }
    
    // Summary
    size_t dirCount = 0, fileCount = 0;
    uint64_t totalSize = 0;
    
    for (const auto& entry : entries) {
        if (entry.isDirectory) {
            dirCount++;
        } else {
            fileCount++;
            totalSize += entry.header.fileSize;
        }
    }
    
    std::cout << std::string(80, '-') << std::endl;
    std::cout << "Summary: " << fileCount << " files, " << dirCount << " directories" << std::endl;
    std::cout << "Total file size: " << totalSize << " bytes (" << (totalSize / 1024 / 1024) << " MB)" << std::endl;
    
    return 0;
}
