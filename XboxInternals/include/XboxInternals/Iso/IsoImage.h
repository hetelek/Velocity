#pragma once
#include "XboxInternals/Iso/IsoDefinitions.h"
#include "XboxInternals/Export.h"
#include <string>
#include <memory>
#include <vector>

namespace XboxInternals::Iso {

class XBOXINTERNALS_EXPORT IsoImage {
public:
    IsoImage();
    ~IsoImage();

    // Open an ISO image by file path. Returns true on success.
    bool open(const std::string& path);

    // Close the image
    void close();

    // Get basic info
    const IsoInfo& info() const noexcept { return info_; }

    // List all entries (files and directories) in the ISO
    std::vector<IsoEntry> listEntries() const;

    // Extract a single file to the output directory
    bool extractFile(const IsoEntry& entry, const std::string& outputDir);

    // Extract all files to the output directory
    bool extractAll(const std::string& outputDir);

    // Create XISO from directory
    static bool createFromDirectory(const std::string& inputDir, const std::string& outputIso);

    // Create XISO from existing ISO (for format conversion or scrubbing)
    static bool createFromIso(const std::string& inputIso, const std::string& outputIso, bool scrub = false);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    IsoInfo info_;
};

} // namespace XboxInternals::Iso
