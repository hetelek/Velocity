# Feature 004: ISO ↔ GOD Conversion

**Status**: Future  
**Priority**: Low  
**Depends On**: 003-iso-xex-support  
**Target Version**: TBD (0.4.0.0+)

## Overview

Add bi-directional conversion between Xbox 360 ISO disc images and GOD (Games-On-Demand) packages. This allows users to convert disc games to digital format for HDD installation, or convert digital packages back to disc format for archival/burning.

## Motivation

**ISO → GOD Benefits:**
- Play disc games without physical discs
- Faster loading times from HDD
- Disc preservation (reduce wear)
- Portable game library
- Xbox 360 console compatibility

**GOD → ISO Benefits:**
- Archival and backup
- Disc burning for physical media
- Game preservation
- Sharing between consoles (via burned discs)

## Dependencies

**Required:**
- Feature 003: ISO/XEX support (ISO parsing, XEX extraction)
- Existing: SVOD package support (already implemented)
- Existing: GDFX filesystem (already implemented)

## Technical Approach

### ISO → GOD Conversion

```
1. Open Xbox 360 ISO
2. Parse GDFX filesystem and extract default.xex
3. Create SVOD package structure:
   - Generate XContentHeader metadata from XEX info
   - Set content type: GameOnDemand or InstalledGame
   - Calculate proper data block layout
4. Copy ISO sectors to SVOD data files:
   - Multi-threaded sector copying
   - Generate hash tables (Level 0 + Master)
   - Apply sector-to-address mapping
5. Sign SVOD package (console or strong signed)
6. Output: Content folder with .data files
```

### GOD → ISO Conversion

```
1. Open SVOD package
2. Read GDFX filesystem from SVOD data files
3. Extract all files to temporary directory OR
4. Direct sector copy with reverse mapping:
   - Read from SVOD data files
   - Convert SVOD addresses back to ISO sectors
   - Write to ISO file with proper offsets (GDF/XGD3)
5. Write GDFX header at correct offset
6. Pad ISO to proper size
7. Output: .iso file
```

## Reference Implementations

**iso2god-rs** (Rust, MIT) - https://github.com/iliazeus/iso2god-rs
- Primary reference for ISO → GOD
- Multi-threaded conversion
- Trim optimization
- Dry-run mode

**Potential UI/UX from iso2god CLI:**
```
Usage: iso2god [OPTIONS] <SOURCE_ISO> <DEST_DIR>

Options:
  --dry-run             Just print title info, don't convert
  --game-title <TITLE>  Override game title
  --trim                Remove unused space
  -j, --num-threads <N> Worker threads
```

## Proposed Features

### Conversion Wizard
- Source selection (ISO file or GOD package)
- Automatic format detection
- Conversion direction selection
- Options:
  - Trim unused space (ISO → GOD)
  - Custom game title override
  - Thread count for performance
  - Console signing vs strong signing
  - Output location selection
- Progress tracking:
  - Overall percentage
  - Current operation (parsing, copying, hashing, signing)
  - ETA and speed (MB/s)
  - Cancel support

### Batch Conversion
- Queue multiple ISOs for conversion
- Background processing
- Pause/resume support
- Error handling per-file
- Summary report

### Validation
- Verify source integrity before conversion
- Post-conversion validation
- Hash verification
- File size sanity checks

## GUI Integration

### Menu Items
- **Tools → Convert ISO to GOD...**
- **Tools → Convert GOD to ISO...**
- **Tools → Batch Conversion...**

### Dialogs
- `conversionwizard.cpp/h/ui` - Main conversion wizard
- `batchconversiondialog.cpp/h/ui` - Batch queue management
- Integration with existing progress dialogs

## Implementation Plan

### Phase 1: Core Conversion
- [ ] ISO → GOD converter class
- [ ] GOD → ISO converter class
- [ ] Sector mapping algorithms
- [ ] Multi-threaded sector copying
- [ ] Unit tests with small test ISOs

### Phase 2: Hash/Signing
- [ ] SVOD hash table generation
- [ ] Integrate with existing Rehash/Resign code
- [ ] Certificate management
- [ ] Validation routines

### Phase 3: GUI
- [ ] Conversion wizard UI
- [ ] Progress tracking integration
- [ ] Error reporting dialogs
- [ ] Settings persistence

### Phase 4: Optimization
- [ ] Trim unused space detection
- [ ] Memory-efficient streaming
- [ ] Thread pool optimization
- [ ] Caching strategies

### Phase 5: Batch & Advanced
- [ ] Batch conversion queue
- [ ] Background processing
- [ ] Pause/resume
- [ ] Conversion profiles/presets

## Success Criteria

1. ✅ Can convert Xbox 360 ISO to playable GOD package
2. ✅ GOD package boots on real Xbox 360 console
3. ✅ Can convert GOD back to valid ISO
4. ✅ Converted ISO matches original (or is trimmed correctly)
5. ✅ Multi-threaded conversion performs efficiently (>50 MB/s)
6. ✅ GUI provides clear progress and error reporting
7. ✅ No regression in existing ISO/SVOD functionality

## Non-Goals

- Original Xbox format conversion
- Compression/encryption beyond standard SVOD
- Game modification or patching
- Region conversion
- DLC package conversion (separate feature)

## Risks & Mitigations

| Risk | Impact | Mitigation |
|------|--------|------------|
| Large file sizes (6-8 GB) | Memory usage, slow conversion | Stream-based I/O, chunk processing |
| Hash/signing errors | GOD won't boot on console | Thorough testing, validation checks |
| ISO padding/alignment issues | Corrupted output | Reference iso2god-rs padding logic |
| Multi-threading race conditions | Data corruption | Thread-safe sector mapping, atomic writes |
| Cancelled conversions | Partial files left behind | Cleanup on cancel, atomic operations |

## Testing Strategy

- Small test ISOs (arcade games ~50-200 MB)
- Medium ISOs (1-2 GB retail games)
- Large ISOs (6-8 GB multi-disc games)
- Test on real Xbox 360 hardware
- Verify hash tables match reference tools
- Round-trip testing (ISO → GOD → ISO)

## Future Extensions

- DLC package conversion
- Title update integration
- Custom avatar items conversion
- Multi-disc set handling
- Cloud upload integration
- FTP transfer to console

## Related Features

- 003-iso-xex-support: Foundation for ISO handling
- 001-baseline-capabilities: SVOD/GDFX support
- Future: Disc burning integration
- Future: Console FTP client

## Notes

- Conversion should preserve game functionality 100%
- Console signing requires keyvault
- Strong signing may not boot on stock consoles
- Trim feature can save significant HDD space
- GOD packages are preferred by Xbox 360 for installed games
- Some games may have protections against conversion (rare)
