#include "XboxInternals/Iso/IsoImage.h"
#include <fstream>
#include <vector>
#include <system_error>
#include <filesystem>
#include <cstring>
#include <algorithm>

namespace fs = std::filesystem;

namespace XboxInternals::Iso {

struct IsoImage::Impl {
    std::string path;
    uint64_t size = 0;
    std::ifstream file;
    std::vector<IsoEntry> entries;
    uint64_t imageOffset = 0;

    bool findMagicOffset();
    void parseDirectoryTree(uint64_t dirSector, uint64_t dirSize, uint64_t offset, const std::string& parentPath);
    IsoEntry parseDirectoryEntry(uint64_t position, uint64_t baseOffset);
};

IsoImage::IsoImage() : impl_(new Impl()) {}

IsoImage::~IsoImage() = default;

bool IsoImage::Impl::findMagicOffset() {
    // XISO magic string is always at a fixed offset (0x10000) within the XISO header
    // These are the possible image start offsets (lseek offsets)
    const uint64_t lseekOffsets[] = {
        0,              // Standard GDF - image starts at 0
        0x0FD90000,     // XGD2DVD / Redump format
        0x02080000,     // XGD3
        0x18300000      // XGD1
    };

    const uint64_t MAGIC_OFFSET_IN_HEADER = 0x10000;
    char buffer[MAGIC_LENGTH + 1] = {0};
    
    for (auto lseekOffset : lseekOffsets) {
        uint64_t magicPosition = MAGIC_OFFSET_IN_HEADER + lseekOffset;
        
        if (magicPosition + MAGIC_LENGTH > size) {
            continue;
        }
        
        file.seekg(magicPosition, std::ios::beg);
        if (!file.good()) {
            continue;
        }
        
        file.read(buffer, MAGIC_LENGTH);
        
        if (file.gcount() == MAGIC_LENGTH && 
            std::memcmp(buffer, MAGIC_STRING, MAGIC_LENGTH) == 0) {
            imageOffset = lseekOffset;  // Store the IMAGE offset, not the magic position
            return true;
        }
    }
    return false;
}

IsoEntry IsoImage::Impl::parseDirectoryEntry(uint64_t position, uint64_t baseOffset) {
    IsoEntry entry;
    
    file.seekg(position, std::ios::beg);
    file.read(reinterpret_cast<char*>(&entry.header), sizeof(DirectoryEntryHeader));
    
    if (file.gcount() != sizeof(DirectoryEntryHeader)) {
        return entry;  // Return empty entry on error
    }

    // Read filename
    if (entry.header.nameLength > 0 && entry.header.nameLength <= 255) {
        std::vector<char> nameBuffer(entry.header.nameLength);
        file.read(nameBuffer.data(), entry.header.nameLength);
        entry.name.assign(nameBuffer.data(), entry.header.nameLength);
    }

    entry.size = entry.header.fileSize;
    entry.startSector = entry.header.startSector;
    entry.offset = imageOffset + (static_cast<uint64_t>(entry.header.startSector) * SECTOR_SIZE);
    entry.type = (entry.header.attributes & ATTR_DIRECTORY) ? IsoEntryType::Directory : IsoEntryType::File;

    return entry;
}

void IsoImage::Impl::parseDirectoryTree(uint64_t dirSector, uint64_t dirSize, uint64_t offset, const std::string& parentPath) {
    if (offset * 4 >= dirSize) {
        return;  // Offsets are in 4-byte units
    }

    uint64_t position = imageOffset + (dirSector * SECTOR_SIZE) + (offset * 4);
    
    IsoEntry entry = parseDirectoryEntry(position, offset);

    if (entry.name.empty() || entry.header.leftOffset == 0xFFFF) {
        return;  // Skip padding/invalid entries
    }

    // Process left child
    if (entry.header.leftOffset != 0) {
        parseDirectoryTree(dirSector, dirSize, entry.header.leftOffset, parentPath);
    }

    // Add current entry
    entry.path = parentPath.empty() ? entry.name : parentPath + "/" + entry.name;
    entries.push_back(entry);

    // Recursively process subdirectory
    if (entry.type == IsoEntryType::Directory && entry.header.fileSize > 0) {
        parseDirectoryTree(entry.header.startSector, entry.header.fileSize, 0, entry.path);
    }

    // Process right child
    if (entry.header.rightOffset != 0) {
        parseDirectoryTree(dirSector, dirSize, entry.header.rightOffset, parentPath);
    }
}

bool IsoImage::open(const std::string& path) {
    close();
    impl_->path = path;
    
    // Try to open the file with filesystem path for better Unicode support
    std::filesystem::path fsPath(path);
    impl_->file.open(fsPath, std::ios::binary | std::ios::ate);
    
    if (!impl_->file) {
        return false;
    }

    auto end = impl_->file.tellg();
    if (end < 0) {
        return false;
    }

    impl_->size = static_cast<uint64_t>(end);
    info_.imageSize = impl_->size;
    info_.sectorSize = SECTOR_SIZE;
    
    // Reset to beginning after getting size
    impl_->file.seekg(0, std::ios::beg);
    if (!impl_->file.good()) {
        return false;
    }

    // Find magic string offset
    if (!impl_->findMagicOffset()) {
        return false;  // Not a valid XISO
    }

    info_.imageOffset = impl_->imageOffset;
    info_.type = (impl_->imageOffset == 0) ? IsoType::GDF : 
                 (impl_->imageOffset == 0x02080000) ? IsoType::XGD3 : IsoType::Unknown;

    // Read root directory info from header
    // Magic is at imageOffset + 0x10000, root sector/size immediately after
    impl_->file.seekg(impl_->imageOffset + 0x10000 + MAGIC_LENGTH, std::ios::beg);
    uint32_t rootSector = 0, rootSize = 0;
    impl_->file.read(reinterpret_cast<char*>(&rootSector), 4);
    impl_->file.read(reinterpret_cast<char*>(&rootSize), 4);

    info_.rootDirSector = rootSector;
    info_.rootDirSize = rootSize;

    // Parse directory tree
    impl_->parseDirectoryTree(rootSector, rootSize, 0, "");

    return true;
}

void IsoImage::close() {
    if (impl_->file.is_open()) impl_->file.close();
    impl_->path.clear();
    impl_->size = 0;
    impl_->entries.clear();
    impl_->imageOffset = 0;
    info_ = IsoInfo();
}

std::vector<IsoEntry> IsoImage::listEntries() const {
    return impl_->entries;
}

bool IsoImage::extractFile(const IsoEntry& entry, const std::string& outputDir) {
    if (!impl_->file.is_open()) return false;
    if (entry.type != IsoEntryType::File) return false;
    if (entry.size == 0) return true;  // Empty file, nothing to extract

    // Create full directory structure from entry.path
    fs::path fullPath = fs::path(outputDir) / entry.path;
    fs::path dir = fullPath.parent_path();
    
    std::error_code ec;
    fs::create_directories(dir, ec);
    if (ec) return false;

    std::ofstream of(fullPath, std::ios::binary);
    if (!of) return false;

    // Read from start sector
    impl_->file.seekg(static_cast<std::streamoff>(entry.offset), std::ios::beg);
    
    uint64_t remaining = entry.size;
    constexpr size_t bufferSize = 65536;
    std::vector<char> buffer(bufferSize);

    while (remaining > 0) {
        size_t toRead = static_cast<size_t>(std::min<uint64_t>(remaining, bufferSize));
        impl_->file.read(buffer.data(), toRead);
        auto bytesRead = impl_->file.gcount();
        if (bytesRead <= 0) break;
        
        of.write(buffer.data(), bytesRead);
        remaining -= bytesRead;
    }

    return remaining == 0;
}

bool IsoImage::extractAll(const std::string& outputDir) {
    for (const auto& entry : impl_->entries) {
        if (entry.type == IsoEntryType::File) {
            // Create subdirectory structure
            fs::path fullPath = fs::path(outputDir) / entry.path;
            fs::path dir = fullPath.parent_path();
            
            std::error_code ec;
            fs::create_directories(dir, ec);
            if (ec) continue;

            // Extract to proper path
            std::ofstream of(fullPath, std::ios::binary);
            if (!of || entry.size == 0) continue;

            impl_->file.seekg(static_cast<std::streamoff>(entry.offset), std::ios::beg);
            
            uint64_t remaining = entry.size;
            constexpr size_t bufferSize = 65536;
            std::vector<char> buffer(bufferSize);

            while (remaining > 0) {
                size_t toRead = static_cast<size_t>(std::min<uint64_t>(remaining, bufferSize));
                impl_->file.read(buffer.data(), toRead);
                auto bytesRead = impl_->file.gcount();
                if (bytesRead <= 0) break;
                
                of.write(buffer.data(), bytesRead);
                remaining -= bytesRead;
            }
        }
    }
    return true;
}

bool IsoImage::createFromDirectory(const std::string& inputDir, const std::string& outputPath) {
    // Stub implementation - full version would:
    // 1. Scan directory tree and build file list
    // 2. Calculate total size and allocate sectors
    // 3. Build AVL directory tree structure
    // 4. Write XISO header with magic at proper offset
    // 5. Write directory entries with left/right child offsets
    // 6. Write file data at calculated sector positions
    
    // For now, return false to indicate not implemented
    (void)inputDir;
    (void)outputPath;
    return false;
}

bool IsoImage::createFromIso(const std::string& inputIso, const std::string& outputPath, bool scrub) {
    // Stub implementation - full version would:
    // 1. Open source ISO
    // 2. Read and validate XISO structure
    // 3. Optionally repack/optimize layout
    // 4. Write to new ISO at outputPath
    
    // For now, just copy the file if scrub is false
    (void)inputIso;
    (void)outputPath;
    (void)scrub;
    return false;
}

} // namespace XboxInternals::Iso
