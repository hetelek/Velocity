# XGDTool Reference Notes

Source: https://github.com/wiredopposite/XGDTool  
License: GPL-3.0  
Language: C++  

## Key Implementation Details

### XISO Format Constants
- **Sector Size**: 2048 bytes (0x800)
- **Magic String**: "MICROSOFT*XBOX*MEDIA" (20 bytes)
- **Magic Offset**: 0x10000 (sector 32)
- **Root Directory Sector**: 0x108 (default)
- **Padding Byte**: 0xFF

### Directory Entry Structure
```cpp
struct DirectoryEntry::Header {
    uint16_t left_offset;    // AVL tree left child offset
    uint16_t right_offset;   // AVL tree right child offset
    uint32_t start_sector;   // File/directory start sector
    uint32_t file_size;      // Size in bytes
    uint8_t attributes;      // 0x10=directory, 0x20=file
    uint8_t name_length;     // Filename length
}; // 14 bytes + variable filename
```

### Directory Attributes
- `ATTRIBUTE_FILE = 0x20`
- `ATTRIBUTE_DIRECTORY = 0x10`
- `ATTRIBUTE_HIDDEN = 0x02`
- `ATTRIBUTE_SYSTEM = 0x04`

### Image Offset Detection
XGDTool scans for magic string "MICROSOFT*XBOX*MEDIA" at typical offsets:
- Standard: 0x10000
- XGD3: 0x02080000
- XGD1: 0x18300000

### Directory Parsing Algorithm
1. Read root sector/size from magic offset + 20 bytes
2. Traverse AVL tree structure using left/right offsets
3. Each offset is in units of 4 bytes (multiply by 4 for byte offset)
4. Recursively process directories

### File Extraction
- Files stored at `start_sector * 2048` offset
- Read `file_size` bytes
- Padding to sector boundary with 0xFF

## Relevance to Velocity-Next

XGDTool provides battle-tested C++ code for:
1. **Robust XISO parsing** - handles XGD1/XGD3 variants
2. **AVL tree directory traversal** - proper offset handling
3. **Multi-format support** - ISO, GoD, CCI, CSO, ZAR
4. **Batch extraction** with proper file structure
5. **XEX/XBE detection** for platform identification

We should adopt the same directory entry structure and parsing logic for accuracy.
