# TextEncoding Module

## Overview
This module provides **read-only decoding** of Xbox 360–era text files into `QString`.  
It supports all encodings found in Xbox 360 assets, game scripts, and profiles.

✅ Designed for **viewing only** (no saving/round-tripping required).  
✅ Fully **cross-platform** (no Windows APIs or ICU).  
✅ Integrated with Qt 6 text widgets.

---

## Supported Encodings

| Encoding     | Codepage | Usage on Xbox 360                          |
|--------------|----------|--------------------------------------------|
| UTF-8        | –        | Modern configs, XML, JSON                  |
| UTF-16 LE/BE | –        | Profiles, gamertags, STFS metadata         |
| CP932        | Shift-JIS | Japanese game scripts (Lua, menus)         |
| CP936        | GBK      | Simplified Chinese localizations            |
| CP949        | EUC-KR / UHC | Korean localizations                  |
| CP950        | Big5     | Traditional Chinese (Taiwan/HK releases)    |
| Latin-1      | ISO-8859-1 | Western/ASCII fallback                   |

---

## Project Structure

```
TextEncoding/
 ├── EncodingDetector.h / .cpp   # BOM + heuristic + scoring auto-detection
 ├── TextDecoder.h / .cpp        # Base interface
 ├── ShiftJIS/CP932Decoder.h/.cpp, CP932Mapping.h/.cpp
 ├── GBK/CP936Decoder.h/.cpp,    CP936Mapping.h/.cpp
 ├── EUCKR/CP949Decoder.h/.cpp,  CP949Mapping.h/.cpp
 └── Big5/CP950Decoder.h/.cpp,   CP950Mapping.h/.cpp
```

- **Detector** chooses the encoding (Auto or user override).  
- **Decoder** converts `QByteArray` → `QString` with replacement char (`U+FFFD`) on errors.  
- **Mapping files** provide constexpr lookup tables for CP932/936/949/950.  
  - Arrays live in `.cpp` for compile-time performance.  
  - Headers only declare `extern` + lookup helpers.

---

## Detection Logic

1. **Check BOM**  
   - UTF-8 BOM → UTF-8  
   - UTF-16LE/BE BOM → UTF-16LE/BE  
   - UTF-32LE/BE BOM → UTF-32LE/BE  

2. **Scan first 8KB if no BOM**  
   - Detect alternating nulls (UTF-16 heuristic).  
   - Validate UTF-8 strictly.  
   - Apply scoring system for CP932/936/949/950:  
     - CP949: 4 points / valid pair  
     - CP932, CP950: 3 points  
     - CP936: 2 points  
     - Threshold: 10 points minimum  

3. **Fallback**  
   - If no encoding matches confidently → ISO-8859-1.

---

## UI Integration

- **Auto detection** runs on file load.  
- **Status bar** shows detected encoding.  
- **Dropdown** allows override:  
  `[Auto | UTF-8 | UTF-16LE | UTF-16BE | CP932 | CP936 | CP949 | CP950 | Latin-1]`  
- **Fonts**: use CJK-capable fonts (MS Gothic, SimSun, Malgun Gothic, MingLiU) for clarity.

---

## Error Handling

- Orphan lead bytes → `U+FFFD` replacement char.  
- Invalid trail bytes → `U+FFFD`.  
- Detection confidence < threshold → fallback to Latin-1.  
- No exceptions thrown; always returns a `QString`.

---

## Extensibility

- To add another encoding:
  1. Generate mapping file from Unicode.org data (`.h/.cpp`).  
  2. Add `Decoder` implementing `TextDecoder`.  
  3. Register in `EncodingDetector`.  
  4. Add to UI dropdown.

---

## Developer Notes

- All decoders are **stateless** → safe for multi-thread use.  
- Mapping tables are large (10–20k entries) → kept in `.cpp` to reduce compile overhead.  
- Module is **decode-only** → no encode/reverse mapping implemented.  

---

✅ This module is **production-ready** for text viewing in Velocity or any Qt 6 project that needs legacy Xbox 360 encoding support.  
