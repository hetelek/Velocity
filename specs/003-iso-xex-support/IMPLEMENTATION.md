# Implementation Roadmap: ISO/XEX Support

**Feature**: 003-iso-xex-support  
**Branch**: `003-iso-xex-support`  
**Status**: Ready for Implementation  
**Last Updated**: 2025-10-26

---

## Current Status ✅

- **Branch Created**: `003-iso-xex-support` (pushed to GitHub)
- **Specifications**: Complete and committed
  - Feature 003: ISO/XEX Support (full technical spec)
  - Feature 004: ISO ↔ GOD Conversion (future)
  - Plugin Architecture (PLUGINS.md)
  - Reference code preserved (xbox360iso.py + licenses)

---

## Implementation Phases

### **Phase 1: Core ISO Parsing** 🔨 START HERE

#### 1.1 Create ISO Module Structure

```bash
# Create directories
mkdir -p XboxInternals/include/XboxInternals/Iso
mkdir -p XboxInternals/src/Iso
```

**File Structure:**
```
XboxInternals/
├── include/XboxInternals/Iso/
│   ├── IsoDefinitions.h      # Enums, structs (IsoType, XexInfo, etc.)
│   ├── IsoImage.h            # Main ISO parser class
│   └── XexExecutable.h       # XEX2 executable parser
└── src/Iso/
    ├── IsoImage.cpp
    └── XexExecutable.cpp
```

**Tasks:**
- [ ] Create `IsoDefinitions.h` with:
  - `IsoType` enum (GDF, XGD3, XSF, Unknown)
  - `XexPlatform` enum
  - `XexExecutableType` enum
  - `IsoInfo` struct (type, offsets, sizes, identifier)
  - `XexInfo` struct (mediaId, titleId, version, disc info)
  - ISO offset constants

- [ ] Create `IsoImage.h` with:
  - Constructor: `IsoImage(const std::string& isoPath)`
  - `bool DetectFormat()`
  - `IsoInfo GetIsoInfo() const`
  - `std::vector<GdfxFileEntry> GetRootDirectory()`
  - `bool ExtractDefaultXex(const std::string& outputPath)`
  - `XexInfo GetDefaultXexInfo()`
  - `bool ExtractFile(const std::string& path, const std::string& outputPath)`

- [ ] Create `XexExecutable.h` with:
  - Constructor: `XexExecutable(BaseIO* io)`
  - `bool Parse()`
  - `XexInfo GetExecutionInfo() const`
  - `bool IsValidXex()`

#### 1.2 Implement ISO Detection (`IsoImage.cpp`)

**Detection Algorithm:**
```cpp
1. Open ISO file
2. Seek to sector 0x20 (0x10000 bytes)
3. Try each offset:
   - Check 0x20 * sectorSize + 0xFD90000 (GDF)
   - Check 0x20 * sectorSize + 0x2080000 (XGD3)
   - Check 0x20 * sectorSize + 0 (XSF - reject)
4. Read 20 bytes, compare to "MICROSOFT*XBOX*MEDIA"
5. If match, record type and offset
6. Read GDFX header (rootSector, rootSize, creationTime)
7. Calculate image/volume sizes
```

**Tasks:**
- [ ] Implement `CheckMagic(DWORD offset)` helper
- [ ] Implement `DetectFormat()` with offset detection
- [ ] Implement `ReadGdfxHeader(GdfxHeader* header)` wrapper
- [ ] Add error handling for invalid ISOs

#### 1.3 Implement XEX Parser (`XexExecutable.cpp`)

**XEX Parsing Algorithm:**
```cpp
1. Read first 4 bytes, verify "XEX2" magic
2. Read code offset (0x08) and cert offset (0x10)
3. Validate offsets are within bounds
4. Read info table entry count (0x14)
5. Iterate info table:
   - Read header ID (4 bytes)
   - If ID == 0x00040006 (execution info):
     - Save address of execution info
6. Seek to execution info address:
   - Read mediaId (4 bytes)
   - Read version (4 bytes)
   - Read baseVersion (4 bytes)
   - Read titleId (4 bytes)
   - Read platform (1 byte)
   - Read executableType (1 byte)
   - Read discNumber (1 byte)
   - Read discCount (1 byte)
```

**Tasks:**
- [ ] Implement `IsValidXex()` - verify "XEX2" magic
- [ ] Implement `ReadInfoTable()` - parse header entries
- [ ] Implement `Parse()` - extract execution info
- [ ] Add bounds checking and error handling

---

### **Phase 2: File Extraction**

#### 2.1 Root Directory Search (`IsoImage.cpp`)

**Algorithm:**
```cpp
1. Calculate root directory position:
   - offset = rootOffset + (rootDirSector * sectorSize)
2. Read rootDirSize bytes into buffer
3. Parse buffer for GDFX file entries:
   - Read unknown (4 bytes)
   - Read sector (4 bytes)
   - Read size (4 bytes)
   - Read attributes (1 byte)
   - Read nameLen (1 byte)
   - Read name (nameLen bytes)
4. Compare name (case-insensitive) to target filename
5. If found, return sector and size
```

**Tasks:**
- [ ] Implement `FindFileInRoot(const std::string& filename, DWORD* outSector, DWORD* outSize)`
- [ ] Add case-insensitive string comparison
- [ ] Implement `ExtractDefaultXex(const std::string& outputPath)`
- [ ] Implement `GetDefaultXexInfo()` - extract to memory, parse XEX

#### 2.2 Generic File Extraction

**Tasks:**
- [ ] Implement `GetRootDirectory()` - return all file entries
- [ ] Implement `ExtractFile(path, outputPath)` - extract any file
- [ ] Add recursive directory support
- [ ] Add progress callbacks (optional)

---

### **Phase 3: GUI Integration**

#### 3.1 Create ISO Dialog

**Files to Create:**
```
Velocity/
├── isodialog.cpp
├── isodialog.h
└── isodialog.ui
```

**UI Components:**
- **File Info Section:**
  - ISO path (read-only text)
  - ISO type (GDF/XGD3 label)
  - Image size (bytes, formatted)
  - Volume size/sectors
  
- **Game Info Section:**
  - Media ID (hex format: XXXXXXXX)
  - Title ID (hex format: XXXXXXXX)
  - Version (decimal)
  - Base Version (decimal)
  - Platform (label)
  - Disc X of Y
  
- **File Browser:**
  - QTreeWidget with GDFX filesystem
  - Columns: Name, Size, Type, Sector
  - Right-click context menu
  
- **Actions:**
  - "Extract default.xex" button
  - "Extract Selected" button
  - "Close" button

**Tasks:**
- [ ] Create UI file in Qt Designer
- [ ] Implement `IsoDialog` class
- [ ] Add file opening logic
- [ ] Populate ISO/game info fields
- [ ] Implement file tree population
- [ ] Add extraction handlers
- [ ] Add progress dialogs for large files

#### 3.2 MainWindow Integration

**Tasks:**
- [ ] Add menu item: **File → Open ISO Image...**
- [ ] Add ISO file filter to open dialog (`*.iso`)
- [ ] Create `onOpenIso()` slot in MainWindow
- [ ] Launch `IsoDialog` with selected ISO path
- [ ] Add recent ISOs to file menu (optional)

---

### **Phase 4: Build System Integration**

#### 4.1 Update XboxInternals CMakeLists.txt

**Add to `XboxInternals/CMakeLists.txt`:**

```cmake
# ISO module headers
set(ISO_HEADERS
    include/XboxInternals/Iso/IsoDefinitions.h
    include/XboxInternals/Iso/IsoImage.h
    include/XboxInternals/Iso/XexExecutable.h
)

# ISO module sources
set(ISO_SOURCES
    src/Iso/IsoImage.cpp
    src/Iso/XexExecutable.cpp
)

# Add to PUBLIC_HEADERS
list(APPEND PUBLIC_HEADERS ${ISO_HEADERS})

# Add to SOURCES
list(APPEND SOURCES ${ISO_SOURCES})
```

**Tasks:**
- [ ] Update CMakeLists.txt
- [ ] Update `XboxInternals.h` master header to include ISO headers
- [ ] Verify installation exports ISO headers

#### 4.2 Update Velocity CMakeLists.txt

**Add to `Velocity/CMakeLists.txt`:**

```cmake
set(SOURCES
  # ... existing sources ...
  isodialog.cpp
)

set(UI_FORMS
  # ... existing forms ...
  isodialog.ui
)
```

**Tasks:**
- [ ] Add ISO dialog files to build
- [ ] Verify linking with XboxInternals

---

### **Phase 5: Testing & Documentation**

#### 5.1 Testing

**Test ISOs Needed:**
- Small arcade game (~50-200 MB)
- Medium retail game (1-2 GB)
- Large game (6-8 GB)
- Both GDF and XGD3 formats

**Test Cases:**
- [ ] ISO format detection (GDF)
- [ ] ISO format detection (XGD3)
- [ ] Reject original Xbox ISOs (XSF)
- [ ] Reject corrupted/invalid ISOs
- [ ] Extract default.xex successfully
- [ ] Parse XEX metadata correctly
- [ ] Display game info in GUI
- [ ] Browse GDFX filesystem
- [ ] Extract files from ISO
- [ ] Handle large ISOs (6-8 GB)
- [ ] Verify no memory leaks
- [ ] Test on MinGW and MSVC builds

#### 5.2 Documentation

**Tasks:**
- [ ] Update README.md with ISO support section
- [ ] Add API usage examples to spec
- [ ] Document known limitations
- [ ] Add screenshots to README
- [ ] Update CHANGELOG.md

---

## Development Workflow

### **Getting Started:**

```bash
# 1. Ensure you're on the feature branch
git checkout 003-iso-xex-support

# 2. Create module directories
mkdir -p XboxInternals/include/XboxInternals/Iso
mkdir -p XboxInternals/src/Iso

# 3. Start with Phase 1.1 - create header files
# (Begin with IsoDefinitions.h)
```

### **Incremental Commits:**

```bash
# After each sub-phase:
git add <changed files>
git commit -m "feat(iso): <description>"
git push origin 003-iso-xex-support

# Example commits:
# "feat(iso): add IsoDefinitions with types and structs"
# "feat(iso): implement ISO format detection"
# "feat(iso): add XEX parser for metadata extraction"
# "feat(iso): implement default.xex extraction"
# "feat(iso): add ISO dialog GUI"
```

### **Build & Test Cycle:**

```bash
# After each implementation:
cmake --preset windows-mingw-debug  # or windows-msvc-debug
cmake --build --preset windows-mingw-debug
./out/build/windows-mingw-debug/Velocity/VelocityNext.exe

# Test the new functionality
# Fix any issues
# Commit
```

---

## Success Criteria ✅

Implementation is complete when:

1. ✅ Can open GDF and XGD3 ISO files
2. ✅ ISO format detected correctly
3. ✅ Rejects invalid/corrupted ISOs gracefully
4. ✅ Extracts default.xex successfully
5. ✅ Parses XEX metadata (Media ID, Title ID, version, etc.)
6. ✅ GUI displays all game information
7. ✅ Can browse complete GDFX filesystem
8. ✅ Can extract any file from ISO
9. ✅ No crashes with large ISOs (6-8 GB)
10. ✅ No regression in existing SVOD/STFS/FATX features
11. ✅ Builds successfully on MinGW and MSVC
12. ✅ Documentation updated

---

## Key Reference Points

**Specification**: `specs/003-iso-xex-support/spec.md`

**Reference Implementations:**
- Python: `specs/003-iso-xex-support/reference/xbox360iso.py`
- C (extract-xiso): https://github.com/XboxDev/extract-xiso
- Rust (iso2god-rs): https://github.com/iliazeus/iso2god-rs

**Existing Code to Leverage:**
- `XboxInternals/Disc/Gdfx.{h,cpp}` - GDFX structures
- `XboxInternals/IO/FileIO.h` - File I/O abstraction
- `Velocity/svoddialog.*` - Similar dialog pattern

---

## Future Work (After Phase 5)

**Feature 004: ISO ↔ GOD Conversion**
- Convert ISO to SVOD packages
- Convert SVOD back to ISO
- Multi-threaded conversion
- Trim optimization

**Plugin System**
- ISO Patcher plugin (Redump.org compliance)
- Plugin architecture implementation
- Community plugin ecosystem

**Enhancements**
- Game name database lookup
- Multi-disc set handling
- Achievement extraction
- Multi-threaded ISO processing

---

## API Usage Examples

### Opening and Parsing an ISO

```cpp
#include "XboxInternals/Iso/IsoImage.h"
#include <iostream>

// Open an ISO file
XboxInternals::Iso::IsoImage iso;
if (!iso.open("path/to/game.iso")) {
    std::cerr << "Failed to open ISO" << std::endl;
    return false;
}

// Get ISO information
const auto& info = iso.info();
std::cout << "ISO Type: " << (info.type == IsoType::GDF ? "GDF" : "XGD3") << std::endl;
std::cout << "Image Size: " << info.imageSize << " bytes" << std::endl;
std::cout << "Root Sector: " << info.rootDirSector << std::endl;

// List all files
auto entries = iso.listEntries();
for (const auto& entry : entries) {
    if (entry.type == IsoEntryType::File) {
        std::cout << entry.path << " - " << entry.size << " bytes" << std::endl;
    }
}

// Extract a specific file
IsoEntry targetFile;
for (const auto& entry : entries) {
    if (entry.name == "default.xex") {
        targetFile = entry;
        break;
    }
}

if (!iso.extractFile(targetFile, "output/directory")) {
    std::cerr << "Extraction failed" << std::endl;
}

// Extract everything
iso.extractAll("output/directory");
```

### Text Encoding Detection and Decoding

```cpp
#include "Velocity/TextEncoding/EncodingDetector.h"
#include <QFile>
#include <QDebug>

// Read file data
QFile file("path/to/japanese.lua");
if (!file.open(QIODevice::ReadOnly)) return;
QByteArray rawData = file.readAll();

// Auto-detect encoding
auto encoding = Velocity::TextEncoding::EncodingDetector::detect(rawData);
qDebug() << "Detected:" << static_cast<int>(encoding);

// Create decoder
auto decoder = Velocity::TextEncoding::EncodingDetector::createDecoder(encoding);
if (!decoder) {
    qDebug() << "Failed to create decoder";
    return;
}

// Decode to Unicode
QString text = decoder->decode(rawData);
qDebug() << "Decoded text:" << text;

// Check if a character is supported
if (decoder->supports(QChar(0x3042))) {  // Hiragana 'a'
    qDebug() << "Character is supported by this encoding";
}

// Encode back to bytes
QByteArray encoded = decoder->encode(text);
```

### Using TextDialog with Encoding Support

```cpp
#include "Velocity/textdialog.h"

// Simple text display (no encoding UI)
QString simpleText = "Hello, world!";
TextDialog* dlg1 = new TextDialog(simpleText, "example.txt", parentWidget);
dlg1->show();

// Display with encoding detection and override UI
QByteArray rawData = file.readAll();
TextDialog* dlg2 = new TextDialog(rawData, "japanese_file.lua", parentWidget);
dlg2->setAttribute(Qt::WA_DeleteOnClose);
dlg2->show();
// User can now select encoding from dropdown, sees auto-detection status
```

### XEX Metadata Parsing

```cpp
#include "XboxInternals/Iso/XexExecutable.h"
#include <iostream>

// Parse XEX from filename pattern
XboxInternals::Iso::XexExecutable xex;
xex.parseFromFilename("Game_TitleID_415607BE_Disc1of2.iso");

const auto& info = xex.info();
std::cout << "Title ID: " << info.titleId << std::endl;
std::cout << "Disc: " << (int)info.discNumber << " of " << (int)info.discCount << std::endl;

// Parse XEX from file data
if (xex.parseFromFile("path/to/default.xex")) {
    std::cout << "Game Name: " << info.gameName << std::endl;
    std::cout << "Media ID: " << info.mediaId << std::endl;
    std::cout << "Version: " << info.version << std::endl;
}
```

### Custom Encoding Decoder Implementation

```cpp
#include "Velocity/TextEncoding/TextDecoder.h"
#include <QHash>
#include <QMutex>

class CustomDecoder : public Velocity::TextEncoding::TextDecoder {
public:
    QString decode(const QByteArray& data) const override {
        QString result;
        for (int i = 0; i < data.size(); ++i) {
            quint8 byte = static_cast<quint8>(data[i]);
            // Custom decoding logic
            result.append(mapByteToUnicode(byte));
        }
        return result;
    }
    
    QByteArray encode(const QString& text) const override {
        QByteArray result;
        for (const QChar& ch : text) {
            if (ch.unicode() < 0x80) {
                result.append(static_cast<char>(ch.unicode()));
            } else {
                // Use O(1) reverse lookup
                quint8 byte = 0;
                if (lookupReverse(ch.unicode(), byte)) {
                    result.append(static_cast<char>(byte));
                } else {
                    result.append('?');  // Unmappable
                }
            }
        }
        return result;
    }
    
    QString name() const override { return "Custom Encoding"; }
    
    bool supports(QChar ch) const override {
        if (ch.unicode() < 0x80) return true;  // ASCII
        quint8 dummy;
        return lookupReverse(ch.unicode(), dummy);
    }

private:
    // Example: lazy-initialized O(1) reverse cache
    mutable QHash<char32_t, quint8> reverseCache_;
    mutable QMutex cacheMutex_;
    mutable bool cacheInitialized_ = false;
    
    void initializeCache() const {
        QMutexLocker locker(&cacheMutex_);
        if (cacheInitialized_) return;
        
        // Build reverse lookup table
        for (int i = 0; i < 256; ++i) {
            char32_t unicode = mapByteToUnicode(i);
            if (unicode != 0xFFFD) {
                reverseCache_.insert(unicode, static_cast<quint8>(i));
            }
        }
        cacheInitialized_ = true;
    }
    
    bool lookupReverse(char32_t unicode, quint8& byte) const {
        if (!cacheInitialized_) initializeCache();
        auto it = reverseCache_.constFind(unicode);
        if (it != reverseCache_.constEnd()) {
            byte = it.value();
            return true;
        }
        return false;
    }
    
    char32_t mapByteToUnicode(quint8 byte) const {
        // Custom mapping logic
        return byte;  // Placeholder
    }
};
```

---

## Notes

- Follow existing XboxInternals code style (CamelCase, 4-space indent)
- Use Qt idioms (signals/slots, containers)
- Leverage existing I/O abstractions
- Add comprehensive error handling
- Keep GUI responsive (use progress dialogs for long operations)
- Test with real Xbox 360 ISOs
- Maintain GPL-3.0 license compliance

**Current Status**: ✅ **COMPLETED** - All phases implemented and tested

---

**Last Updated**: 2025-10-29  
**Branch**: `003-iso-xex-support`  
**Status**: Production-ready, user-validated
