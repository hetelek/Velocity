# Feature 003: ISO/XEX Support

**Status**: Planned  
**Target Version**: 0.3.0.0

## Quick Summary

Add Xbox 360 ISO image and XEX executable parsing to enable game disc analysis and metadata extraction.

## Files

- **spec.md** - Full technical specification
- **reference/** - Python reference implementation (MIT licensed)
  - xbox360iso.py - Original parser by Rob Lambell
  - LICENSE - MIT license text

## Current Capabilities

XboxInternals currently supports:
- ✅ SVOD packages (Games on Demand)
- ✅ GDFX filesystem (through SVOD)
- ✅ STFS packages
- ✅ FATX filesystems

## Planned Additions

This feature will add:
- 📋 Direct ISO parsing (GDF/XGD3 formats)
- 📋 XEX2 executable metadata extraction
- 📋 Game identification (Media ID, Title ID)
- 📋 ISO file browser in GUI
- 📋 File extraction from ISOs

## Key Differences from Reference

The C++ implementation will:
- Use existing GDFX parsing code
- Integrate with XboxInternals I/O abstraction
- Provide Qt-based GUI dialogs
- Use proper type-safe structs vs Python dicts
- Defer CSV game lookup to future feature

## Implementation Status

Not yet started. See spec.md for detailed implementation plan.

## References

- Python implementation: `reference/xbox360iso.py` (MIT)
- extract-xiso (C): https://github.com/XboxDev/extract-xiso
- iso2god-rs (Rust): https://github.com/iliazeus/iso2god-rs (MIT)
- patch_xiso (Python): https://github.com/Qubits01/patch_xiso (MIT) - **Plugin candidate**
- abgx360 project: http://abgx360.net
- Redump.org: http://redump.org (ISO verification database)
- Free60 wiki (XEX format): https://free60.org/wiki/XEX
- GDFX existing code: `XboxInternals/Disc/Gdfx.{h,cpp}`
