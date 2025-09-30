# Building Velocity

## Prerequisites

- **Qt 6.7.3 or higher** (Components: Core, Xml, Widgets, Network)
  - Tested with: Qt 6.8.0 and above
- **CMake 3.20+**
- **C++20 compiler**
- **Python 3** (for Botan amalgamation build)

## Quick Start

# Clone the repository
git clone https://github.com/Pandoriaantje/Velocity
cd velocity

# Build using CMake presets (choose one)
cmake --preset windows-mingw-release    # Windows MinGW Release
cmake --preset windows-mingw-debug      # Windows MinGW Debug
cmake --preset macos-release            # macOS Release  
cmake --preset macos-debug              # macOS Debug
cmake --preset linux-release            # Linux Release
cmake --preset linux-debug              # Linux Debug

# Compile
cmake --build --preset [chosen-preset]

## Botan Dependency

The cryptography library is automatically handled:

- **Automatic Download**: Botan is automatically downloaded and built
- **No Submodule Required**: No git submodules to initialize
- **Custom Modules**: Uses minimal module set defined in botan_modules.cmake
- **Amalgamation**: Built as single-file amalgamation for better performance

## Qt Configuration

### Qt Version Information

- **Minimum Required**: Qt 6.7.3
- **Tested Versions**: Qt 6.8.0 and above
- **Components Required**: Core, Xml, Widgets, Network

### Automatic Discovery

The build system uses a three-stage process to find Qt automatically:

1. **CMake Native Detection** - Uses standard CMake Qt discovery
2. **qmake Fallback** - Finds qmake6/qmake in PATH and queries installation  
3. **Environment Variable** - Checks QT6_PREFIX_PATH if needed

**Common locations where Qt is found automatically:**

- **Windows**: Standard Qt installer paths (C:/Qt/[version]/*)
- **macOS**: Qt installer locations (~/Qt) and Homebrew installations  
- **Linux**: Qt installer locations (~/Qt) and system package locations

The detection works with any Qt installation as long as:
- Qt is installed in a standard location, OR
- `qmake6` is available in your PATH

If Qt is not found automatically, see "Manual Qt Configuration" below.

### Manual Qt Configuration

If Qt is not found automatically, specify it manually using CMAKE_PREFIX_PATH:

# Set Qt path explicitly
cmake -B build -DCMAKE_PREFIX_PATH="/path/to/your/qt"
cmake --build build

## Build Options

### Release Builds (Recommended for end users)

# Windows MinGW Release
cmake --preset windows-mingw-release
cmake --build --preset windows-mingw-release

# macOS Release
cmake --preset macos-release
cmake --build --preset macos-release

# Linux Release
cmake --preset linux-release
cmake --build --preset linux-release

### Debug Builds (For development)

# Windows MinGW Debug
cmake --preset windows-mingw-debug
cmake --build --preset windows-mingw-debug

# macOS Debug
cmake --preset macos-debug
cmake --build --preset macos-debug

# Linux Debug
cmake --preset linux-debug
cmake --build --preset linux-debug

### Library Configuration

# Build XboxInternals as shared library (default)
cmake --preset windows-mingw-release -DBUILD_XBOXINTERNALS_SHARED=ON

# Also build static version
cmake --preset windows-mingw-release -DBUILD_XBOXINTERNALS_STATIC=ON

# Build only static library
cmake --preset windows-mingw-release -DBUILD_XBOXINTERNALS_SHARED=OFF -DBUILD_XBOXINTERNALS_STATIC=ON

### Botan Version

# Test with different Botan version (if needed)
cmake --preset windows-mingw-release -DBOTAN_VERSION=3.10.0

## Platform-Specific Notes

### Windows
- **MinGW**: Use windows-mingw-release or windows-mingw-debug presets
- **Compiler**: Use MingGW compilers matching your Qt installation
- **Output**: Executable with Windows resource data

### macOS
- **Bundle**: Creates .app bundle with proper metadata
- **Icons**: Uses velocity.icns for application icon
- **Output**: Velocity.app bundle

### Linux
- **Standard**: Creates standard Linux executable
- **Output**: Velocity binary

## Troubleshooting

### Qt Not Found
# Verify Qt installation (must be 6.7.3 or higher)
qmake6 --version

# Set Qt path explicitly
cmake -B build -DCMAKE_PREFIX_PATH="/path/to/qt"

### Botan Build Issues
# Clean and rebuild
rm -rf out/build
cmake --preset your-preset

# Ensure Python 3 is available
python3 --version

# Check internet connection (Botan download requires internet)

### Compiler Issues
- Ensure your compiler supports C++20
- Match Qt kit with your compiler (MingGW Qt with MingGW compiler)

## Output Locations

Build outputs are organized in:
- `out/build/windows-mingw-release/` - Windows MinGW Release
- `out/build/windows-mingw-debug/` - Windows MinGW Debug
- `out/build/macos-release/` - macOS Release
- `out/build/macos-debug/` - macOS Debug
- `out/build/linux-release/` - Linux Release
- `out/build/linux-debug/` - Linux Debug

For more details, see the project documentation or check the CMakeLists.txt files for advanced configuration options.