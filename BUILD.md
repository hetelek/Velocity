# Building Velocity-Next

## Prerequisites

- **Qt 6.7.3 or higher** (Components: Core, Xml, Widgets, Network)
  - Tested with: Qt 6.8.0 and above
- **CMake 3.20+**
- **C++20 compiler**:
  - **Windows**: MinGW 13.1.0+ or MSVC 2022 (Visual Studio 17.0+)
  - **macOS**: Clang (Xcode Command Line Tools)
  - **Linux**: GCC 11+ or Clang 14+
- **Python 3** (for Botan amalgamation build)

## Quick Start

```bash
# Clone the repository
git clone https://github.com/Pandoriaantje/Velocity
cd Velocity

# Configure (choose one preset)
cmake --preset windows-mingw-release    # Windows MinGW Release
cmake --preset windows-mingw-debug      # Windows MinGW Debug
cmake --preset windows-msvc-release     # Windows MSVC Release
cmake --preset windows-msvc-debug       # Windows MSVC Debug
cmake --preset macos-release            # macOS Release  
cmake --preset macos-debug              # macOS Debug
cmake --preset linux-release            # Linux Release
cmake --preset linux-debug              # Linux Debug

# Build
cmake --build --preset [chosen-preset]
```

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

```bash
# Set Qt path explicitly
cmake -B build -DCMAKE_PREFIX_PATH="/path/to/your/qt"
cmake --build build
```

## Build Options

### Release Builds (Recommended for end users)

```bash
# Windows MinGW Release
cmake --preset windows-mingw-release
cmake --build --preset windows-mingw-release

# Windows MSVC Release
cmake --preset windows-msvc-release
cmake --build --preset windows-msvc-release

# macOS Release
cmake --preset macos-release
cmake --build --preset macos-release

# Linux Release
cmake --preset linux-release
cmake --build --preset linux-release
```

### Debug Builds (For development)

```bash
# Windows MinGW Debug
cmake --preset windows-mingw-debug
cmake --build --preset windows-mingw-debug

# Windows MSVC Debug
cmake --preset windows-msvc-debug
cmake --build --preset windows-msvc-debug

# macOS Debug
cmake --preset macos-debug
cmake --build --preset macos-debug

# Linux Debug
cmake --preset linux-debug
cmake --build --preset linux-debug
```

### Library Configuration

```bash
# Build XboxInternals as shared library (default)
cmake --preset windows-mingw-release -DBUILD_XBOXINTERNALS_SHARED=ON

# Also build static version
cmake --preset windows-mingw-release -DBUILD_XBOXINTERNALS_STATIC=ON

# Build only static library
cmake --preset windows-mingw-release -DBUILD_XBOXINTERNALS_SHARED=OFF -DBUILD_XBOXINTERNALS_STATIC=ON
```

### Botan Version

```bash
# Test with different Botan version (if needed)
cmake --preset windows-mingw-release -DBOTAN_VERSION=3.10.0
```

## Platform-Specific Notes

### Windows

#### MinGW
- **Presets**: `windows-mingw-release` or `windows-mingw-debug`
- **Compiler**: MinGW 13.1.0+ (must match Qt installation)
- **Qt Installation**: Use Qt Online Installer and select MinGW kit
- **Output**: Executable with Windows resource data

#### MSVC (Visual Studio 2022)
- **Presets**: `windows-msvc-release` or `windows-msvc-debug`
- **Requirements**: Visual Studio 2022 (17.0+) with C++ workload
- **Qt Installation**: Use Qt Online Installer and select MSVC 2022 64-bit kit
- **Setup**: Run from **Developer Command Prompt for VS 2022** or **Developer PowerShell for VS 2022**
- **Output**: Executable with Windows resource data

**MSVC Build Example:**
```bash
# Open Developer Command Prompt for VS 2022 or Developer PowerShell for VS 2022
# Then run:
cmake --preset windows-msvc-release
cmake --build --preset windows-msvc-release
```

**Note**: MSVC builds require:
- Visual Studio 2022 or Build Tools for Visual Studio 2022
- C++ CMake tools for Windows component
- Ninja build system (included with Visual Studio)
- Qt compiled for MSVC 2022 (download from Qt installer)

### macOS
- **Bundle**: Creates .app bundle with proper metadata
- **Icons**: Uses velocity.icns for application icon
- **Output**: Velocity.app bundle

#### macOS Distribution

**Prerequisites:**
- Qt 6.7.3 or higher installed on your system
- Xcode command line tools installed
- Build the project first using the presets above
- `app.entitlements` file exists in project root

**Note**: The deployment script will automatically find Qt in:
- Your PATH
- QT6_PREFIX_PATH environment variable  
- Common installation locations (~/Qt, Homebrew, etc.)

If needed, you can explicitly set your Qt path:

```bash
export PATH="$HOME/Qt/[version]/macos/bin:$PATH"
# or
export QT6_PREFIX_PATH="/path/to/your/qt"

# Make the deployment script executable (first time only)
chmod +x deploy_mac.sh

# Deploy and sign (from project root)
./deploy_mac.sh

# For debug builds
./deploy_mac.sh out/build/macos-debug
```

**Note**: Uses ad-hoc signing by default. For distribution, replace `-` with your Developer ID in the script.

### Linux
- **Standard**: Creates standard Linux executable
- **Output**: Velocity binary

## Troubleshooting

### Qt Not Found

```bash
# Verify Qt installation (must be 6.7.3 or higher)
qmake6 --version

# Set Qt path explicitly
cmake -B build -DCMAKE_PREFIX_PATH="/path/to/qt"
```

### Botan Build Issues
```bash
# Clean and rebuild
rm -rf out/build
cmake --preset your-preset

# Ensure Python 3 is available
python --version   # Windows
python3 --version  # macOS/Linux

# Check internet connection (Botan download requires internet)
```
### Compiler Issues
- Ensure your compiler supports C++20
- Match Qt kit with your compiler (MinGW Qt with MingGW compiler)

## Output Locations

Build outputs are organized in:
- `out/build/windows-mingw-release/` - Windows MinGW Release
- `out/build/windows-mingw-debug/` - Windows MinGW Debug
- `out/build/macos-release/` - macOS Release
- `out/build/macos-debug/` - macOS Debug
- `out/build/linux-release/` - Linux Release
- `out/build/linux-debug/` - Linux Debug

## Running Velocity

### Windows - Administrator Privileges Required

**Important**: On Windows, Velocity requires **Administrator privileges** to access physical drives (Xbox 360 hard drives and USB devices).

**To run Velocity with the necessary permissions:**

1. Navigate to the Velocity executable:
   - Release: `out/build/windows-mingw-release/Velocity/Velocity.exe`
   - Debug: `out/build/windows-mingw-debug/Velocity/Velocity.exe`

2. **Right-click** on `Velocity.exe` → Select **"Run as administrator"**

**Why Administrator access is needed:**
- Windows requires elevated privileges to open raw physical disk devices (`\\.\PHYSICALDRIVE#`)
- This is necessary for the Device Viewer to detect and access Xbox 360 drives connected via USB (using SATA-to-USB adapters, etc.)
- Without Administrator rights, Xbox 360 drives will **not be detected** in the Device Viewer

**Alternative - Create Administrator Shortcut:**
1. Right-click `Velocity.exe` → **"Create shortcut"**
2. Right-click the shortcut → **"Properties"**
3. Click **"Advanced..."** button
4. Check **"Run as administrator"**
5. Click **OK** → **OK**

Now you can use this shortcut to always launch Velocity with proper permissions.

### Linux

On Linux, access to raw block devices requires root privileges or membership in the `disk` group.

**Option 1: Run with sudo** (recommended for testing)
```bash
sudo ./out/build/linux-release/Velocity/Velocity
```

**Option 2: Add user to disk group** (for regular use)
```bash
# Add your user to the disk group
sudo usermod -a -G disk $USER

# Log out and log back in for changes to take effect
```

**Note**: Xbox 360 drives connected via USB should appear as `/dev/sd*` devices (e.g., `/dev/sdb`).

### macOS

On macOS, access to raw disk devices typically requires administrator privileges.

**Run with sudo:**
```bash
sudo ./out/build/macos-release/Velocity.app/Contents/MacOS/Velocity
```

**Note**: Xbox 360 drives should appear as `/dev/disk*` or `/dev/rdisk*` devices.

---

For more details, see the project documentation or check the CMakeLists.txt files for advanced configuration options.