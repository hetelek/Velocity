# Changelog

## Velocity-Next v0.2.0 - 2025-10-29 (Branch: 003-iso-xex-support)

### 🎮 Xbox 360 ISO Support
- **XISO Parser**: Full support for GDF, XGD3, and Redump ISO formats
- **Format Detection**: Automatic magic string detection at multiple known offsets (0x10000, 0x02080000, 0x0FD90000)
- **AVL Tree Traversal**: Proper parsing of Xbox 360's binary tree filesystem structure
- **File Extraction**: Single file or batch export with preserved directory structure
- **XEX Metadata**: Extract Media ID, Title ID, version, disc info from default.xex
- **ISO Browser UI**: Tree view with file details, context menu extraction, search/filter

### 🌏 Text Encoding Module
- **4 Asian Encodings**: CP932 (Shift-JIS), CP936 (GBK), CP950 (Big5), CP949 (EUC-KR)
- **Auto-Detection**: BOM check → UTF-16 alternating pattern → UTF-8 validation → scoring-based legacy encoding
- **Performance Optimization**: O(1) hash map lookups (10,000x faster than linear search)
- **Lazy Initialization**: No startup penalty, thread-safe with QMutex
- **UI Integration**: Encoding dropdown + status bar in text viewer
- **Character Validation**: `supports(QChar)` API for input validation
- **Header Optimization**: 99.8% size reduction (1,383 KB → 3.4 KB) via header/impl split

### 📚 Technical Improvements
- **XboxInternals Library**: New `Iso/` module with IsoImage, GodImage, XexExecutable classes
- **Velocity UI**: isodialog, textdialog with encoding support
- **Build System**: Updated CMakeLists.txt for new modules
- **Testing**: test_iso_magic.cpp, test_iso_parse.cpp for validation

### 📝 Documentation
- **Implementation Roadmap**: specs/003-iso-xex-support/IMPLEMENTATION.md
- **Reference Notes**: XGDTool C++ implementation notes
- **Python Reference**: xbox360iso.py preserved for algorithm comparison

## Velocity Setup v1.1 - 2025-09-28
- CMake superproject for Velocity 1.0.0
- Integrated Botan 3.9.0 amalgamation build (no system Botan ever used)
- Minimal Botan modules list consolidated
- XboxInternals defaults to SHARED library, optional STATIC
- Qt6 Core + Xml required
- Added CMakePresets for MinGW & MSVC
- Added .gitmodules (botan submodule on release-3 branch; recommend tag 3.9.0)
