#include "XboxInternals/Iso/GodImage.h"
#include "XboxInternals/Iso/IsoImage.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>

namespace fs = std::filesystem;

namespace XboxInternals::Iso {

struct GodImage::Impl {
    fs::path godDirectory;
    std::vector<fs::path> dataPaths;
    std::vector<std::unique_ptr<std::ifstream>> dataFiles;
    uint32_t totalSectors = 0;
    IsoImage virtualIso;  // Parse the embedded XISO

    bool findDataFiles();
    GodRemap remapSector(uint64_t xisoSector) const;
};

GodImage::GodImage() : impl_(new Impl()) {}

GodImage::~GodImage() = default;

bool GodImage::Impl::findDataFiles() {
    // Look for Data0000, Data0001, etc.
    for (int i = 0; i < 100; ++i) {
        char filename[32];
        std::snprintf(filename, sizeof(filename), "Data%04d", i);
        
        fs::path dataPath = godDirectory / filename;
        if (fs::exists(dataPath) && fs::is_regular_file(dataPath)) {
            dataPaths.push_back(dataPath);
        } else {
            break;  // No more data files
        }
    }

    if (dataPaths.empty()) return false;

    // Open all data files
    for (const auto& path : dataPaths) {
        auto file = std::make_unique<std::ifstream>(path, std::ios::binary);
        if (!file->is_open()) return false;
        dataFiles.push_back(std::move(file));
    }

    // Calculate total sectors
    uint64_t totalDataBlocks = dataPaths.size() * God::DATA_BLOCKS_PER_PART;
    totalSectors = static_cast<uint32_t>((totalDataBlocks * God::BLOCK_SIZE) / SECTOR_SIZE);

    return true;
}

GodRemap GodImage::Impl::remapSector(uint64_t xisoSector) const {
    return remapSectorToGod(xisoSector);
}

bool GodImage::open(const std::string& godDirectory) {
    close();
    impl_->godDirectory = godDirectory;

    if (!impl_->findDataFiles()) {
        return false;
    }

    // Create a temporary virtual ISO reader that reads through GOD remapping
    // For now, we'll extract to temp and open the ISO (simplified)
    // Full implementation would override IsoImage to read through GOD blocks

    info_.type = IsoType::GDF;
    info_.sectorSize = SECTOR_SIZE;
    info_.imageOffset = MAGIC_OFFSET;

    return true;
}

void GodImage::close() {
    impl_->dataFiles.clear();
    impl_->dataPaths.clear();
    impl_->totalSectors = 0;
    impl_->virtualIso.close();
    info_ = IsoInfo();
}

bool GodImage::readSector(uint32_t sector, char* buffer, size_t bufferSize) {
    if (bufferSize < SECTOR_SIZE) return false;
    if (sector >= impl_->totalSectors) return false;

    GodRemap remap = impl_->remapSector(sector);
    
    if (remap.fileIndex >= impl_->dataFiles.size()) return false;

    auto& file = impl_->dataFiles[remap.fileIndex];
    file->seekg(remap.offset, std::ios::beg);
    file->read(buffer, SECTOR_SIZE);

    return file->gcount() == SECTOR_SIZE;
}

bool GodImage::readBytes(uint64_t offset, size_t size, char* buffer) {
    uint32_t sector = static_cast<uint32_t>(offset / SECTOR_SIZE);
    uint32_t sectorOffset = static_cast<uint32_t>(offset % SECTOR_SIZE);
    
    size_t bytesRead = 0;
    std::vector<char> sectorBuffer(SECTOR_SIZE);

    while (bytesRead < size) {
        if (!readSector(sector, sectorBuffer.data(), SECTOR_SIZE)) {
            return false;
        }

        size_t toCopy = std::min<size_t>(size - bytesRead, SECTOR_SIZE - sectorOffset);
        std::memcpy(buffer + bytesRead, sectorBuffer.data() + sectorOffset, toCopy);

        bytesRead += toCopy;
        sector++;
        sectorOffset = 0;
    }

    return true;
}

uint32_t GodImage::totalSectors() const {
    return impl_->totalSectors;
}

std::vector<IsoEntry> GodImage::listEntries() const {
    // For now, return empty - full implementation would parse XISO through GOD
    // This requires creating a virtual IsoImage that reads through readSector()
    return {};
}

bool GodImage::extractFile(const IsoEntry& entry, const std::string& outputDir) {
    // Stub - full implementation would read through GOD remapping
    return false;
}

bool GodImage::extractAll(const std::string& outputDir) {
    // Stub - full implementation would parse directory tree and extract each file
    return false;
}

// ============================================================================
// GodWriter implementation
// ============================================================================

struct GodWriter::Impl {
    std::string titleId;
    std::string gameName;
};

GodWriter::GodWriter() : impl_(new Impl()) {}

GodWriter::~GodWriter() = default;

void GodWriter::setMetadata(const std::string& titleId, const std::string& gameName) {
    impl_->titleId = titleId;
    impl_->gameName = gameName;
}

bool GodWriter::createFromIso(const std::string& isoPath, const std::string& outputDir) {
    // Stub - full implementation would:
    // 1. Open source ISO
    // 2. Create Data0000, Data0001, etc files
    // 3. Write master hash table
    // 4. For each sector, remap to GOD block and write
    // 5. Generate sub hash tables
    // 6. Write final hash
    return false;
}

bool GodWriter::createFromDirectory(const std::string& inputDir, const std::string& outputDir) {
    // Stub - full implementation would:
    // 1. Create ISO in memory/temp from directory
    // 2. Convert to GOD using createFromIso
    return false;
}

} // namespace XboxInternals::Iso
