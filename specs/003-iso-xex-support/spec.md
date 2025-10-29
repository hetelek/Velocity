# Feature Specification: Xbox 360 ISO & GOD Support

**Feature ID**: 003-iso-xex-support  
**Status**: In Development  
**Priority**: High  
**Created**: 2025-10-26  
**Target Version**: 0.3.0.0

## Overview

Add comprehensive Xbox 360 ISO and Games on Demand (GOD) format support to XboxInternals library with batch extraction and creation capabilities. Enable conversion between ISO ↔ GOD formats and direct analysis of disc images.

## Background

Currently, XboxInternals supports:
- ✅ SVOD (Secure Virtual Optical Disc) - installed game packages
- ✅ GDFX filesystem - but only through SVOD layer
- ✅ STFS packages
- ✅ FATX filesystems

**Missing capabilities:**
- ❌ Direct ISO file parsing (XISO/GDF/XGD3 formats)
- ❌ Games on Demand (GOD) package support
- ❌ ISO ↔ GOD conversion
- ❌ XEX2 executable parsing and metadata extraction
- ❌ Batch extraction of disc images
- ❌ ISO/GOD creation from directory

## Goals

1. **XISO Format Support**: Read/write GDF and XGD3 disc image formats with AVL tree directory structure
2. **GOD Format Support**: Read/write Games on Demand packages with proper block remapping
3. **Format Conversion**: Bidirectional conversion between ISO and GOD
4. **XEX Parsing**: Extract metadata from XEX2 executables (title, media ID, version)
5. **Batch Operations**: Plugins for batch extraction and creation
6. **GUI Integration**: Unified ISO/GOD viewer with conversion options

## Reference Implementation

**Primary Reference**: [XGDTool](https://github.com/wiredopposite/XGDTool) (GPL-3.0)
- Battle-tested XISO/GOD parsing with AVL tree traversal
- Proper magic offset detection (0x10000, 0x02080000, 0x18300000)
- GOD block remapping algorithm (0x1000 byte blocks with hash tables)
- Multi-format conversion engine

## Technical Specification

### 1. ISO Module (`XboxInternals/Iso/`)

#### Files to Create:
```
include/XboxInternals/Iso/
├── IsoDefinitions.h      # Enums and constants
├── IsoImage.h            # Main ISO parser class
└── XexExecutable.h       # XEX2 executable parser

src/Iso/
├── IsoImage.cpp
└── XexExecutable.cpp
```

#### IsoDefinitions.h
```cpp
#pragma once
#include <XboxInternals/TypeDefinitions.h>

enum IsoType {
    IsoTypeGDF = 0,       // Original Xbox 360 (offset 0xFD90000)
    IsoTypeXGD3 = 1,      // XGD3 format (offset 0x2080000)
    IsoTypeXSF = 2,       // Original Xbox (not supported)
    IsoTypeUnknown = 3
};

enum XexPlatform {
    XexPlatformXbox360 = 2
};

enum XexExecutableType {
    XexExeTypeTitle = 0,
    XexExeTypeSystem = 1
};

struct IsoInfo {
    IsoType type;
    DWORD sectorSize;
    DWORD rootOffset;
    DWORD rootDirSector;
    DWORD rootDirSize;
    UINT64 imageSize;
    UINT64 volumeSize;
    UINT64 volumeSectors;
    char identifier[21];  // "MICROSOFT*XBOX*MEDIA"
};

struct XexInfo {
    DWORD mediaId;
    DWORD version;
    DWORD baseVersion;
    DWORD titleId;
    BYTE platform;
    BYTE executableType;
    BYTE discNumber;
    BYTE discCount;
};
```

#### IsoImage.h
```cpp
#pragma once
#include <XboxInternals/IO/FileIO.h>
#include <XboxInternals/Iso/IsoDefinitions.h>
#include <XboxInternals/Iso/XexExecutable.h>
#include <XboxInternals/Disc/Gdfx.h>
#include <XboxInternals/Export.h>
#include <vector>
#include <string>

class XBOXINTERNALS_EXPORT IsoImage {
public:
    IsoImage(const std::string& isoPath);
    ~IsoImage();

    // Detect and validate ISO format
    bool DetectFormat();
    
    // Get ISO information
    IsoInfo GetIsoInfo() const { return isoInfo; }
    
    // Parse GDFX root directory
    std::vector<GdfxFileEntry> GetRootDirectory();
    
    // Extract default.xex from root
    bool ExtractDefaultXex(const std::string& outputPath);
    
    // Get XEX info from default.xex (in-memory)
    XexInfo GetDefaultXexInfo();
    
    // Extract any file from ISO by path
    bool ExtractFile(const std::string& path, const std::string& outputPath);

private:
    FileIO* io;
    IsoInfo isoInfo;
    bool formatDetected;
    
    // ISO format detection offsets
    static const DWORD ISO_OFFSET_GDF = 0xFD90000;
    static const DWORD ISO_OFFSET_XGD3 = 0x2080000;
    static const DWORD ISO_OFFSET_XSF = 0;
    static const DWORD ISO_MAGIC_SECTOR = 0x20;
    static const char ISO_MAGIC[21];
    
    // Helper: Check magic at specific offset
    bool CheckMagic(DWORD offset);
    
    // Helper: Read GDFX header
    bool ReadGdfxHeader(GdfxHeader* header);
    
    // Helper: Search root directory for file (case-insensitive)
    bool FindFileInRoot(const std::string& filename, 
                        DWORD* outSector, 
                        DWORD* outSize);
};
```

#### XexExecutable.h
```cpp
#pragma once
#include <XboxInternals/IO/BaseIO.h>
#include <XboxInternals/Iso/IsoDefinitions.h>
#include <XboxInternals/Export.h>

class XBOXINTERNALS_EXPORT XexExecutable {
public:
    XexExecutable(BaseIO* io);
    ~XexExecutable();
    
    // Parse XEX2 header and extract metadata
    bool Parse();
    
    // Get execution info
    XexInfo GetExecutionInfo() const { return xexInfo; }
    
    // Validate XEX2 magic
    bool IsValidXex();
    
private:
    BaseIO* io;
    XexInfo xexInfo;
    bool parsed;
    
    static const char XEX_MAGIC[5];  // "XEX2"
    static const DWORD XEX_HEADER_ID_EXECUTION_INFO = 0x00040006;
    
    // Helper: Read general info table
    bool ReadInfoTable();
};
```

### 2. GUI Integration

#### New Dialog: `isodialog.cpp/h/ui`

Features:
- Open ISO file
- Display ISO type (GDF/XGD3)
- Show volume information
- Display game metadata from default.xex:
  - Media ID
  - Title ID
  - Version / Base Version
  - Disc number / count
- File browser for GDFX filesystem
- Extract default.xex button
- Extract any file functionality

#### MainWindow Integration

Add menu item: **File → Open ISO Image...**

### 3. Implementation Plan

#### Phase 1: Core ISO Parsing
- [ ] Create `IsoDefinitions.h` with enums and structs
- [ ] Implement `IsoImage` class
  - [ ] ISO format detection (GDF/XGD3/XSF)
  - [ ] GDFX header reading
  - [ ] Root directory parsing
- [ ] Add unit tests for ISO detection

#### Phase 2: XEX Parser
- [ ] Implement `XexExecutable` class
  - [ ] XEX2 magic validation
  - [ ] Info table parsing
  - [ ] Execution info extraction
- [ ] Add XEX parsing tests
- [ ] Integrate with IsoImage

#### Phase 3: File Extraction
- [ ] Implement default.xex search
- [ ] Add file extraction methods
- [ ] Support case-insensitive filename matching
- [ ] Handle sector-based I/O

#### Phase 4: GUI Integration
- [ ] Create `IsoDialog` UI
- [ ] Add ISO file opening
- [ ] Display metadata fields
- [ ] Implement file browser
- [ ] Add extraction functionality
- [ ] Wire up to main menu

#### Phase 5: Testing & Documentation
- [ ] Test with GDF format ISOs
- [ ] Test with XGD3 format ISOs
- [ ] Add sample ISO fixtures (small test files)
- [ ] Document API usage
- [ ] Update README with ISO support

### 4. API Usage Examples

```cpp
// Example 1: Open ISO and get info
IsoImage iso("game.iso");
if (iso.DetectFormat()) {
    IsoInfo info = iso.GetIsoInfo();
    std::cout << "Type: " << info.type << std::endl;
    std::cout << "Size: " << info.imageSize << " bytes" << std::endl;
}

// Example 2: Extract and parse default.xex
XexInfo xexInfo = iso.GetDefaultXexInfo();
printf("Media ID: %08X\n", xexInfo.mediaId);
printf("Title ID: %08X\n", xexInfo.titleId);
printf("Disc %d of %d\n", xexInfo.discNumber, xexInfo.discCount);

// Example 3: Browse filesystem
auto rootFiles = iso.GetRootDirectory();
for (const auto& entry : rootFiles) {
    std::cout << entry.name << " (" << entry.size << " bytes)\n";
}

// Example 4: Extract a file
iso.ExtractFile("/default.xex", "C:/output/default.xex");
```

## Reference Implementations

### Primary Reference: xbox360iso.py
Python reference code (xbox360iso.py) is preserved in:
- `specs/003-iso-xex-support/reference/xbox360iso.py`

Key differences from reference:
- C++ uses proper structs vs Python dicts
- Leverages existing GDFX code for directory parsing
- Integrates with XboxInternals I/O abstraction
- No CSV game name lookup (future enhancement)

### Additional References

**extract-xiso** - https://github.com/XboxDev/extract-xiso
- C implementation for Xbox/Xbox 360 ISO extraction
- Supports both original Xbox and Xbox 360 formats
- Command-line tool with create/extract/list operations
- Key insights:
  - Handles both XISO (original Xbox) and GDFX (Xbox 360) formats
  - ISO creation/rewrite capabilities
  - FTP mode for direct transfers
  - Directory sorting algorithms
  - Extensive format validation
- License: Mixed (check repository for details)
- Use cases: ISO extraction logic, format validation patterns

**iso2god-rs** - https://github.com/iliazeus/iso2god-rs
- Rust implementation for ISO to Games-On-Demand (GOD) conversion
- Converts Xbox 360 ISOs to SVOD format
- Cross-platform CLI tool (Linux, Windows, macOS)
- Key insights:
  - **ISO → SVOD conversion** (the reverse of what we need)
  - Multi-threaded processing
  - Trim unused space from ISO images
  - Dry-run mode for title info extraction
  - Custom game title setting
  - Optimized rewrite of earlier C# version
- License: MIT
- Use cases: 
  - Understanding ISO → SVOD conversion (we need SVOD support + ISO reading)
  - Title metadata extraction patterns
  - Multi-threaded file processing
  - Future feature: ISO to SVOD conversion

**patch_xiso** - https://github.com/Qubits01/patch_xiso
- Python tool to patch Xbox 360 ISOs to match Redump.org standards
- ISO validation and correction
- Key features:
  - Trim/pad ISOs (overdump/underdump correction)
  - Zero-fill L0 Video padding
  - Overwrite stealth sectors
  - No external dependencies
- License: MIT
- Use cases:
  - ISO integrity verification patterns
  - Redump.org database integration
  - **Potential plugin candidate** for ISO patching/validation
  - Archive-quality ISO preparation

**Free60 Wiki** - https://free60.org/wiki/
- XEX format documentation
- GDFX filesystem specification
- Xbox 360 security and cryptography details

## Non-Goals

- CSV-based game name lookup (defer to future feature)
- ISO creation/burning capabilities
- Original Xbox (XSF) support
- ISO modification/patching

## Success Criteria

1. ✅ Can open and detect GDF/XGD3 ISO files
2. ✅ Can extract default.xex from ISO
3. ✅ Can parse XEX2 metadata (Media ID, Title ID, version)
4. ✅ GUI displays game information
5. ✅ Can browse GDFX filesystem
6. ✅ Can extract any file from ISO
7. ✅ No regression in existing SVOD functionality

## Dependencies

- Existing: GDFX structures (reuse)
- Existing: FileIO abstraction
- Existing: Botan (for future signature verification)
- New: ISO offset detection logic
- New: XEX2 format parsing

## Risks & Mitigations

| Risk | Impact | Mitigation |
|------|--------|------------|
| Large ISO files (6-8 GB) | Memory usage | Stream-based I/O, no full file loading |
| Corrupted ISOs | Crash/hang | Robust error handling, validation checks |
| XEX format variants | Parsing failures | Comprehensive format research, graceful fallbacks |
| GDFX code reuse issues | Refactoring needed | Careful abstraction, preserve SVOD compatibility |

## Future Enhancements

### Phase 1 Extensions (Post-Initial Release)
- Game name database lookup (CSV/online API)
- **ISO integrity verification (CRC/hash)**
  - Redump.org database integration (see patch_xiso)
  - Overdump/underdump detection
  - Stealth sector validation
- Multi-disc set handling
- XEX signature verification
- Achievement extraction from GPD files in ISO
- Trim unused space from ISOs
- Multi-threaded ISO processing

### Plugins & Extensions
- **ISO Patching Plugin** (based on patch_xiso)
  - Patch ISOs to match Redump.org standards
  - Fix overdump/underdump issues
  - Zero-fill video padding
  - Correct stealth sectors
  - Archive-quality preparation
  - Plugin architecture allows community extensions

### Phase 2: Format Conversion (Separate Feature)
- **ISO to GOD (Games-On-Demand) conversion** 
  - Convert Xbox 360 ISOs to SVOD format
  - Reference implementation: iso2god-rs (Rust)
  - Benefits: Play disc games from HDD, no disc required
  - Technical: Extract ISO → Convert to SVOD package → Generate metadata
  - GUI: Conversion wizard with progress tracking
  - Features: Multi-threaded conversion, trim optimization, custom titles
  
- **GOD to ISO conversion** (reverse operation)
  - Convert SVOD packages back to ISO format
  - Useful for archival and disc burning
  - Technical: Extract SVOD → Rebuild GDFX structure → Generate ISO
  
- **ISO creation from directory**
  - Build Xbox 360 ISO from file directory
  - Reference: extract-xiso create mode
  - Auto-detect default.xex and generate proper structure
  - Directory sorting for optimal disc layout

## Related Features

- 001-baseline-capabilities: SVOD/GDFX foundation
- 002-rebrand-velocity-next: GUI framework
- Future: Game database integration
- Future: Disc burning support

## Notes

- Python reference handles Media ID → Game Name lookup via CSV download from abgx360.net
- Consider adding this as a separate feature with Qt network integration
- XEX format is well-documented in Free60 wiki
- GDFX filesystem already implemented, just needs ISO wrapper
- **extract-xiso insights:**
  - Handles both original Xbox (XISO) and Xbox 360 (GDFX) formats
  - Uses CMake build system (similar to our project)
  - Supports ISO creation, not just extraction
  - Has directory sorting logic for proper ISO structure
  - Includes media patching for .xbe files
  - FTP mode for network transfers
  - Batch processing capabilities
  - Active maintenance by XboxDev organization
- **Potential code reuse from extract-xiso:**
  - ISO format validation patterns
  - Directory tree sorting algorithms
  - Batch file processing patterns
  - Error handling for corrupted ISOs
- **Potential insights from iso2god-rs:**
  - ISO → SVOD conversion algorithms (we already have SVOD support)
  - Title metadata extraction patterns
  - Multi-threaded file processing approach
  - Trim/optimize ISO operations
  - Dry-run mode for analysis without modification
- **Key differences from other tools:**
  - extract-xiso: CLI-only; we need GUI integration
  - iso2god-rs: Rust-based conversion tool; we need integrated library
  - We integrate with existing XboxInternals library patterns
  - We use Qt for cross-platform I/O and GUI
  - We combine read + write + conversion capabilities
