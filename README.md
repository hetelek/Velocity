Velocity-Next
=============

**Velocity-Next is a continuation of the original [Velocity](https://github.com/hetelek/Velocity) project, which ceased active development some time ago. This fork aims to modernize the codebase, add new features, and maintain compatibility with current operating systems and toolchains.**

### What's New in Velocity-Next

- **MSVC 2022 Support**: Full compatibility with Visual Studio 2022 alongside MinGW
- **Modernized Build System**: CMake 3.20+ with modern target-based patterns
- **Standalone XboxInternals Library**: Can now be distributed and used independently
- **Active Development**: Issues and PRs are welcomed and will be addressed

### Credits

Original Velocity was developed mainly by Stevie Hetelekides (Hetelek) and Adam Spindler (Experiment5X), with contributions from [many others](https://github.com/hetelek/Velocity/graphs/contributors). Velocity-Next continues their work with gratitude and respect for the foundation they built.

## Important: Administrator Privileges Required (Windows)

On **Windows**, Velocity requires **Administrator privileges** to access Xbox 360 drives:

## Running Velocity-Next

- **Right-click** `VelocityNext.exe` → Select **"Run as administrator"**
- This is necessary for detecting and browsing Xbox 360 hard drives and USB devices
- Without Administrator rights, drives will **not be detected** in the Device Viewer

See [BUILD.md](BUILD.md#running-velocity) for Linux/macOS requirements and more details.


About Velocity-Next
-------------------
Velocity-Next is a cross-platform application built using the Qt framework that allows you to browse and edit Xbox 360 files. Velocity-Next's backend is handled by the XboxInternals library, which currently supports the following Xbox file formats:

- Xbox-specific cryptography
- XDBF
- YTGR
- FATX
- STFS
- Xbox disc formats
  - SVOD
  - GDFX
  - **XISO (Xbox 360 ISO)** ⭐ **NEW**
    - GDF, XGD3, and Redump format detection
    - AVL tree directory traversal
    - File extraction with preserved structure
    - XEX metadata parsing (Media ID, Title ID, version info)

### Text Encoding Support ⭐ **NEW**

Velocity-Next includes a custom text encoding module for viewing text files from international game ISOs:

**Supported Encodings:**
- **CP932** (Shift-JIS) - Japanese
- **CP936** (GBK) - Simplified Chinese  
- **CP950** (Big5) - Traditional Chinese
- **CP949** (EUC-KR) - Korean
- UTF-8, UTF-16 LE/BE, Latin-1

**Features:**
- Automatic encoding detection with scoring-based heuristics
- Manual encoding override via dropdown menu
- O(1) hash map lookups for 10,000x performance improvement
- Real-time re-decoding when switching encodings
- Character validation API for UI input

Other individual files specific to the Xbox 360 are supported, including the Account file and other minor ones.

Licensing Information
---------------------
Velocity-Next is licensed and distributed under the GNU General Public License (v3), continuing the license of the original Velocity project.

Velocity-Next is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

If no copy of the GNU General Public License was received with this program (FILE: COPYING), it is available at <http://www.gnu.org/licenses/>.

### Third-Party Components

Velocity-Next includes and statically links the following third-party libraries:

- **Botan** (BSD-2-Clause): Cryptography library embedded as amalgamation build. See [LICENSE-Botan.txt](LICENSE-Botan.txt) for full license text.
- **Qt 6**: Cross-platform application framework (LGPL v3 / GPL v3). Qt is dynamically linked (MSVC builds) or statically linked (MinGW builds).
