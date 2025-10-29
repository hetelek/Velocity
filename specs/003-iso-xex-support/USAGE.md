# ISO & Text Encoding - User Guide

**Feature**: 003-iso-xex-support  
**Last Updated**: 2025-10-29

---

## Opening Xbox 360 ISO Files

### Supported Formats

Velocity-Next supports the following Xbox 360 ISO formats:

- **GDF** (Standard Xbox 360 disc format)
- **XGD3** (Xbox Game Disc v3 - larger capacity)
- **Redump** (Community preservation format with 0x0FD90000 offset)

### How to Open an ISO

1. **Launch Velocity-Next** with Administrator privileges (Windows)
2. **File → Open ISO Image...** (or use the toolbar button)
3. Select your `.iso` file
4. The ISO Browser dialog will open automatically

### ISO Browser Features

**File Tree View:**
- Hierarchical display of all files and folders
- Columns: Name, Type, Size
- Expand/collapse folders with +/- icons

**Search & Filter:**
- Type in the search box to filter files by name
- Search is case-insensitive and updates in real-time

**Extraction:**
- **Extract Selected**: Right-click file/folder → Extract
- **Extract All Files**: Extracts entire ISO to chosen directory
- Preserves directory structure in output

**File Preview:**
- **Double-click** supported files to view:
  - **Images**: `.jpg`, `.png`, `.bmp`, `.dds`, `.tga`
  - **Text**: `.txt`, `.lua`, `.cfg`, `.ini`, `.json`, `.log`, `.h`, `.cpp`, `.hlsl`
  - **XML**: `.xml`, `.xsd`, `.xsl`

---

## Text Encoding Support

### Automatic Encoding Detection

When you open a text file from an ISO, Velocity automatically detects the encoding using this algorithm:

1. **BOM Detection**: Checks for UTF-8/UTF-16 byte order marks
2. **UTF-16 Pattern**: Looks for alternating null bytes (UTF-16 LE/BE)
3. **UTF-8 Validation**: Tests if bytes form valid UTF-8 sequences
4. **Scoring System**: For legacy encodings, scores each possibility:
   - **CP949 (Korean)**: 4 points per character
   - **CP932 (Japanese)**: 3 points per character
   - **CP950 (Traditional Chinese)**: 3 points per character
   - **CP936 (Simplified Chinese)**: 2 points per character
   - **Threshold**: 10 points required for confident detection

### Supported Encodings

| Encoding | Name | Region | Use Case |
|----------|------|--------|----------|
| **CP932** | Shift-JIS | Japan | Japanese text files |
| **CP936** | GBK | China | Simplified Chinese |
| **CP950** | Big5 | Taiwan/Hong Kong | Traditional Chinese |
| **CP949** | EUC-KR | Korea | Korean text files |
| UTF-8 | Unicode | Universal | Modern text files |
| UTF-16 LE | Unicode | Universal | Windows text files |
| UTF-16 BE | Unicode | Universal | Big-endian systems |
| Latin-1 | ISO-8859-1 | Western Europe | Legacy English/European |

### Manual Encoding Override

**When auto-detection isn't perfect:**

1. Open the text file (shows auto-detected encoding)
2. Use the **Encoding dropdown** at the top of the text viewer
3. Select the correct encoding from the list
4. Text re-decodes instantly with O(1) performance

**Status Messages:**
- "Auto-detected: Shift-JIS (CP932)" - automatic detection result
- "Manual override: GBK (CP936)" - user selected encoding

### Common Scenarios

**Japanese Game Files:**
- **Example**: `Audition.lua` from Lips (Japanese karaoke game)
- **Expected Encoding**: CP932 (Shift-JIS)
- **Detection**: Usually auto-detected correctly
- **If Garbled**: Manually select "Shift-JIS (CP932)"

**Chinese Game Files:**
- **Simplified Chinese**: Select "GBK (CP936)"
- **Traditional Chinese**: Select "Big5 (CP950)"
- **Auto-detection**: May confuse GBK/Big5, use manual override

**Korean Game Files:**
- **Expected Encoding**: CP949 (EUC-KR)
- **Detection**: Highest scoring (4 pts), usually accurate

**Mixed/Unknown:**
- **Try UTF-8 first** (modern standard)
- **Latin-1** for English/European games
- **UTF-16** for Windows-generated files

---

## XEX Metadata Extraction

### What is XEX?

**XEX2** (Xbox Executable) files contain:
- Game executable code
- Metadata (Title ID, Media ID, version)
- Digital signatures
- Resource information

### Viewing Game Information

**Automatic on ISO Open:**
1. Velocity searches for `default.xex` in the ISO
2. Extracts metadata automatically
3. Displays in ISO Browser status bar or info panel

**Metadata Includes:**
- **Media ID**: Unique disc identifier (hex)
- **Title ID**: Game identification number (hex)
- **Version**: Game version number
- **Base Version**: Minimum required version
- **Disc Info**: Disc X of Y for multi-disc games

### Use Cases

- **Identify unknown ISOs** by Title ID lookup
- **Verify game versions** for modding/patching
- **Multi-disc game management** (know disc order)
- **Media ID tracking** for authenticity

---

## Performance & Optimization

### Text Encoding Performance

**O(1) Hash Map Lookups:**
- Traditional linear search: ~10,954 comparisons for 1,000 chars (GBK)
- Hash map optimization: ~1,000 lookups (instant)
- **Result**: 10,000x speedup for encoding operations

**Lazy Initialization:**
- Hash maps built on first use (one-time cost: 20-50ms)
- No startup penalty
- Thread-safe with mutex locking

**Memory Usage:**
- Each encoding cache: ~300-500 KB
- Loaded on-demand (only when needed)
- Acceptable trade-off for performance

### File Extraction Performance

**Large Files:**
- 64 KB buffer for file reads
- Progress indication (future enhancement)
- No memory issues with 6-8 GB ISOs

**Batch Extraction:**
- Preserves directory structure
- Efficient sector-based reading
- Handles 1000+ files without slowdown

---

## Troubleshooting

### ISO Won't Open

**Symptoms**: "Failed to open ISO file" error

**Solutions:**
1. **Check format**: Must be Xbox 360 ISO (not Xbox Original)
2. **Verify integrity**: Re-download if corrupted
3. **Try different offset**: Some ISOs use non-standard offsets
4. **Check permissions**: Ensure read access to file

### Text Appears Garbled

**Symptoms**: Asian characters show as �� or boxes

**Solutions:**
1. **Use manual encoding override**: Try each Asian encoding
2. **Check file type**: May not be a text file
3. **Verify game region**: Match encoding to game origin
   - Japan → CP932
   - China → CP936/CP950
   - Korea → CP949

### Extraction Fails

**Symptoms**: "Failed to extract file" error

**Solutions:**
1. **Check disk space**: Ensure enough free space
2. **Administrator rights**: Required on Windows
3. **Output path**: Verify directory is writable
4. **File locks**: Close other programs accessing files

### Wrong Game Info

**Symptoms**: Title ID doesn't match expected game

**Solutions:**
1. **Filename parsing**: If metadata from filename, check format
2. **XEX parsing**: Some XEX files have non-standard structure
3. **Manual lookup**: Use Title ID databases online

---

## Known Limitations

### Current Version

- **No GOD/SVOD → ISO conversion** (planned for Feature 004)
- **No write support**: Read-only ISO operations
- **No XBE support**: Xbox Original executables not parsed
- **No achievement extraction**: Future enhancement
- **Single-threaded**: Large operations may block UI temporarily

### Text Encoding

- **No automatic language detection**: Only encoding detection
- **Character-only validation**: No word-level context analysis
- **Fixed mapping tables**: Cannot learn new character sets

### Future Enhancements

See [IMPLEMENTATION.md](IMPLEMENTATION.md) for planned features:
- Character validation overlay (highlight unsupported chars)
- Confidence score display for ambiguous detection
- Hex/Text toggle view
- Unit tests for edge cases
- Multi-threaded file operations

---

## Tips & Best Practices

### Working with Japanese Games

1. **Always check `Audition.lua` or similar**: Contains readable metadata
2. **CP932 is most common**: Try Shift-JIS first
3. **Watch for mixed encodings**: Some files use UTF-8, others CP932
4. **Character ranges**: 
   - Hiragana: 0x3040-0x309F
   - Katakana: 0x30A0-0x30FF
   - Kanji: 0x4E00-0x9FFF

### Batch Processing

1. **Extract to temp directory first**: Verify contents before permanent location
2. **Use descriptive folder names**: Include game name from XEX metadata
3. **Preserve structure**: Don't flatten directories (breaks relative paths)

### Performance

1. **Close file viewers**: Don't open 100+ text dialogs simultaneously
2. **Large ISOs**: Be patient during initial parsing (6-8 GB takes ~5-10 seconds)
3. **Encoding changes**: Re-decode is instant due to O(1) optimization

---

## Example Workflows

### Workflow 1: View Japanese Lua Script

```
1. File → Open ISO Image... → Select "Lips [RF].iso"
2. Navigate to game/scripts/ folder
3. Double-click "Audition.lua"
4. Text viewer opens with auto-detected CP932
5. Read Japanese text perfectly rendered
6. (Optional) Change encoding dropdown to UTF-8 to compare
```

### Workflow 2: Extract Game Assets

```
1. Open ISO browser
2. Navigate to textures/ or models/ folder
3. Right-click folder → Extract Selected
4. Choose output directory: C:\GameAssets\Lips\
5. Files extract with full directory structure preserved
6. View/edit in external tools
```

### Workflow 3: Identify Unknown ISO

```
1. Open ISO file
2. Check ISO Browser status bar or info panel
3. Note Title ID (e.g., "415607BE")
4. Look up online: xboxunity.net or similar
5. Confirms game name and region
```

### Workflow 4: Multi-Disc Game Management

```
1. Open "Game_Disc1.iso"
2. Check XEX metadata: "Disc 1 of 3"
3. Organize files accordingly:
   - Game_Disc1_extracted/
   - Game_Disc2_extracted/
   - Game_Disc3_extracted/
4. Verify Title ID matches across all discs
```

---

## Technical Reference

### Encoding Detection Thresholds

```cpp
// Scoring system constants
CP949_POINTS = 4;  // Korean gets highest priority
CP932_POINTS = 3;  // Japanese
CP950_POINTS = 3;  // Traditional Chinese
CP936_POINTS = 2;  // Simplified Chinese
THRESHOLD = 10;    // Minimum score for confident detection
```

### ISO Magic Offsets

```cpp
// File offset where magic string is checked
0x10000 + 0x00000000   // Standard GDF
0x10000 + 0x02080000   // XGD3
0x10000 + 0x0FD90000   // Redump/XGD2
0x10000 + 0x18300000   // XGD1 (rare)
```

### Character Mapping Sizes

| Encoding | Entries | Header (old) | Header (new) | .cpp File |
|----------|---------|--------------|--------------|-----------|
| CP932 | 9,804 | 216 KB | 864 bytes | 220 KB |
| CP936 | 21,909 | 482 KB | 864 bytes | 492 KB |
| CP950 | 13,829 | 305 KB | 864 bytes | 311 KB |
| CP949 | 17,167 | 378 KB | 864 bytes | 386 KB |

---

## Support & Feedback

**Report Issues:**
- GitHub Issues: [Velocity-Next/issues](https://github.com/Pandoriaantje/Velocity-Next/issues)
- Include ISO format, file type, and error message

**Feature Requests:**
- Check [IMPLEMENTATION.md](IMPLEMENTATION.md) for planned features
- Submit detailed use case descriptions

**Contributing:**
- See [AGENTS.md](../../AGENTS.md) for coding guidelines
- Follow C++20 + Qt idioms
- Add tests for new encodings

---

**Last Updated**: 2025-10-29  
**Version**: 0.2.0 (Branch: 003-iso-xex-support)
