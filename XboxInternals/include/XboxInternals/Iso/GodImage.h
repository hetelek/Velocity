#pragma once
#include "XboxInternals/Iso/GodDefinitions.h"
#include "XboxInternals/Iso/IsoDefinitions.h"
#include "XboxInternals/Export.h"
#include <string>
#include <vector>
#include <memory>

namespace XboxInternals::Iso {

// Games on Demand reader - reads GOD packages and converts to XISO format
class XBOXINTERNALS_EXPORT GodImage {
public:
    GodImage();
    ~GodImage();

    // Open a GOD package directory (contains Data*.bin files)
    bool open(const std::string& godDirectory);

    // Close the GOD package
    void close();

    // Read a sector from the virtual XISO
    bool readSector(uint32_t sector, char* buffer, size_t bufferSize);

    // Read arbitrary bytes from the virtual XISO
    bool readBytes(uint64_t offset, size_t size, char* buffer);

    // Get total sectors in the virtual XISO
    uint32_t totalSectors() const;

    // Get GOD package info
    const IsoInfo& info() const noexcept { return info_; }

    // List all entries from the embedded XISO
    std::vector<IsoEntry> listEntries() const;

    // Extract file from GOD package
    bool extractFile(const IsoEntry& entry, const std::string& outputDir);

    // Extract all files from GOD package
    bool extractAll(const std::string& outputDir);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    IsoInfo info_;
};

// GOD package writer - creates GOD packages from XISO or directory
class GodWriter {
public:
    GodWriter();
    ~GodWriter();

    // Create GOD package from XISO file
    bool createFromIso(const std::string& isoPath, const std::string& outputDir);

    // Create GOD package from directory
    bool createFromDirectory(const std::string& inputDir, const std::string& outputDir);

    // Set game metadata for the GOD package
    void setMetadata(const std::string& titleId, const std::string& gameName);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace XboxInternals::Iso
