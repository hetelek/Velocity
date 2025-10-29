#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace XboxInternals::Iso {

// XISO format constants (based on XGDTool reference)
constexpr uint32_t SECTOR_SIZE = 0x800;  // 2048 bytes
constexpr char MAGIC_STRING[] = "MICROSOFT*XBOX*MEDIA";
constexpr uint32_t MAGIC_LENGTH = 20;
constexpr uint64_t MAGIC_OFFSET = 0x10000;  // Standard offset
constexpr uint32_t ROOT_DIR_SECTOR = 0x108;  // Default root sector
constexpr uint8_t PAD_BYTE = 0xFF;

// Directory entry attributes
constexpr uint8_t ATTR_FILE = 0x20;
constexpr uint8_t ATTR_DIRECTORY = 0x10;
constexpr uint8_t ATTR_HIDDEN = 0x02;
constexpr uint8_t ATTR_SYSTEM = 0x04;

enum class IsoType : uint32_t {
    Unknown = 0,
    GDF,      // Standard Xbox ISO
    XGD3,     // XGD3 variant (offset 0x02080000)
    XSF       // Custom format
};

struct IsoInfo {
    IsoType type = IsoType::Unknown;
    uint32_t sectorSize = SECTOR_SIZE;
    uint64_t rootOffset = 0;
    uint32_t rootDirSector = 0;
    uint32_t rootDirSize = 0;
    uint64_t imageSize = 0;
    uint64_t volumeSize = 0;
    uint64_t imageOffset = 0;  // Offset to magic string
};

struct XexInfo {
    std::string mediaId;
    uint32_t version = 0;
    uint32_t baseVersion = 0;
    std::string titleId;
    std::string gameName;  // Extracted from XEX header
    uint8_t platform = 0;
    uint8_t executableType = 0;
    uint8_t discNumber = 0;
    uint8_t discCount = 0;
};

enum class IsoEntryType : uint8_t {
    File,
    Directory
};

// XISO directory entry header (14 bytes + variable filename)
#pragma pack(push, 1)
struct DirectoryEntryHeader {
    uint16_t leftOffset;    // AVL tree left child (in 4-byte units)
    uint16_t rightOffset;   // AVL tree right child (in 4-byte units)
    uint32_t startSector;   // File/directory start sector
    uint32_t fileSize;      // Size in bytes
    uint8_t attributes;     // File type attributes
    uint8_t nameLength;     // Filename length
};
#pragma pack(pop)

struct IsoEntry {
    std::string name;
    std::string path;
    IsoEntryType type = IsoEntryType::File;
    uint64_t size = 0;
    uint64_t offset = 0;  // File offset in ISO
    uint32_t startSector = 0;
    DirectoryEntryHeader header;
};

} // namespace XboxInternals::Iso
