# XboxInternals Library

**Xbox 360 file format processing library**

XboxInternals is a C++20 library for working with Xbox 360 file formats, including:

- **FATX** - Xbox 360 filesystem
- **STFS** - Secure Transient File System (packages, saves, DLC)
- **GPD** - Game Profile Data (achievements, settings, stats)
- **XDBF** - Xbox Database Format
- **Account** - Xbox 360 profile/account files
- **AvatarAsset** - Avatar clothing and customization
- **Disc Formats** - SVOD, GDFX (Xbox 360 disc images)

## Building and Installing

### Prerequisites

- **C++20 compiler** (MinGW 13.1.0+, MSVC 2022, GCC 11+, or Clang 14+)
- **CMake 3.20+**
- **Qt 6.7.3+** (Core, Xml components)
- **Python 3** (for Botan amalgamation build)

### Build as Standalone Library

```bash
# Configure with CMake
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_XBOXINTERNALS_SHARED=ON

# Build
cmake --build build --config Release

# Install to system or custom prefix
cmake --install build --prefix /usr/local
# or
cmake --install build --prefix C:/MyLibraries
```

### Build Options

- `BUILD_XBOXINTERNALS_SHARED=ON` - Build as shared library (default)
- `BUILD_XBOXINTERNALS_SHARED=OFF` - Build as static library
- `BUILD_XBOXINTERNALS_STATIC=ON` - Also build static variant alongside shared

## Using in Your Project

### Important: Compiler Compatibility

**⚠️ Toolchain Requirement:** Your project must use the same compiler family as the installed library:

- **MinGW Build** → Consumer must use MinGW (g++)
- **MSVC Build** → Consumer must use MSVC (cl.exe)
- **GCC/Clang (Linux/macOS)** → Consumer uses matching toolchain

**Why?** Different compilers on Windows produce incompatible binaries (different C++ ABI, runtime libraries, and calling conventions). Cross-compiler usage (e.g., MSVC consuming MinGW-built libraries) is not supported.

**Recommendation:** Install separate builds for each toolchain you need to support.

### CMake Integration (Recommended)

After installing XboxInternals, use it in your CMake project:

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyXboxApp)

# Find XboxInternals package
find_package(XboxInternals REQUIRED)

# Create your executable/library
add_executable(MyXboxApp main.cpp)

# Link against XboxInternals
target_link_libraries(MyXboxApp PRIVATE XboxInternals::XboxInternals)

# C++20 is automatically propagated from XboxInternals
```

### Example Code

```cpp
#include <XboxInternals/Stfs/StfsPackage.h>
#include <XboxInternals/Gpd/GameGpd.h>
#include <XboxInternals/Fatx/FatxDrive.h>
#include <iostream>

int main() {
    // Open an STFS package (e.g., saved game)
    StfsPackage pkg("profile.gpd");
    
    // Read GPD data
    GameGpd gpd(&pkg);
    
    // Access achievements, settings, etc.
    std::cout << "Gamertag: " << gpd.gamertag << std::endl;
    
    return 0;
}
```

## Dependencies

### Embedded Dependencies

- **Botan 3.9.0** (BSD-2-Clause) - Cryptography library, statically embedded
  - See [LICENSE-Botan.txt](../LICENSE-Botan.txt) for license terms
  - No separate installation needed

### External Dependencies

- **Qt 6.7.3+** (LGPL v3 / GPL v3) - Must be installed on consumer's system
  - Required components: Core, Xml
  - Linked privately (not exposed in public API)

## License

XboxInternals is licensed under **GPL-3.0** (inherits from Velocity project).

See [COPYING](../COPYING) for the full GPL-3.0 license text.

### Third-Party Licenses

- Botan: BSD-2-Clause - [LICENSE-Botan.txt](../LICENSE-Botan.txt)
- Qt: LGPL v3 / GPL v3 (user-installed, not distributed)

## Installation Layout

After installation, the library is organized as:

```
<prefix>/
  include/
    XboxInternals/
      Account/
      AvatarAsset/
      Cryptography/
      Disc/
      Fatx/
      Gpd/
      IO/
      Stfs/
      Export.h
      TypeDefinitions.h
      XboxInternals.h (convenience header)
  lib/
    libXboxInternals.so / XboxInternals.dll / libXboxInternals.a
    libBotanAmalgamation.a (embedded)
    cmake/
      XboxInternals/
        XboxInternalsConfig.cmake
        XboxInternalsConfigVersion.cmake
        XboxInternalsTargets.cmake
  share/
    doc/
      XboxInternals/
        LICENSE-Botan.txt
```

## API Documentation

All public headers are in `include/XboxInternals/`. Key entry points:

- `XboxInternals.h` - Includes all public headers
- `Stfs/StfsPackage.h` - STFS package reading/writing
- `Fatx/FatxDrive.h` - FATX filesystem access
- `Gpd/GameGpd.h` - Game profile data
- `Account/Account.h` - Xbox 360 account files

For detailed usage, see the [Velocity application](../Velocity/) which uses this library extensively.

## Contributing

XboxInternals is part of the [Velocity](https://github.com/Pandoriaantje/Velocity) project.

For issues, feature requests, or contributions, please visit the main repository.
