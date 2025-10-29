#pragma once
#include "XboxInternals/Iso/IsoDefinitions.h"
#include "XboxInternals/Export.h"
#include <string>

namespace XboxInternals::Iso {

// Lightweight parser for XEX filenames and minimal header info extraction.
class XBOXINTERNALS_EXPORT XexExecutable {
public:
    XexExecutable() = default;

    // Parse a filesystem path or filename to extract metadata (titleId, disc number, etc.)
    // This is a best-effort parser that looks for common patterns in filenames.
    void parseFromFilename(const std::string& filename);

    // Parse XEX header from file data to extract game name and other metadata
    bool parseFromFile(const std::string& filePath);

    const XexInfo& info() const noexcept { return info_; }

private:
    XexInfo info_;
};

} // namespace XboxInternals::Iso
