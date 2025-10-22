# Full Cross-Platform Refactoring Plan
## XboxInternals C++20 Modernization & PIMPL Abstraction

**Date**: 2025-10-19  
**Scope**: Complete XboxInternals library refactoring  
**Goal**: Constitution III-A compliance while maintaining Windows/Linux/macOS functionality  
**Constraint**: Zero functionality loss, zero breakage  

---

## Executive Summary

**Objective**: Refactor XboxInternals to use clean C++20 standard library code where possible, and properly abstract unavoidable platform-specific code using PIMPL idiom across **all three platforms** (Windows, Linux, macOS).

**Key Finding**: Velocity was designed for cross-platform use and XboxInternals already has complete Windows/Linux/macOS implementations. Our job is to **modernize and clean up** the existing platform support, not add it from scratch.

**Estimated Effort**: 5-6 days (40-48 hours)

---

## Cross-Reference Analysis (T005 Complete)

### Public API Usage from Velocity/

**Good News**: Velocity/ GUI layer does **NOT** directly call platform-specific XboxInternals APIs:

✅ **No direct DeviceIO usage** in Velocity/  
✅ **No void* deviceHandle constructor calls** in Velocity/  
✅ **Single entry point**: `FatxDriveDetection::GetAllFatxDrives()` called by deviceviewer.cpp  

**Implication**: We can refactor XboxInternals internals without breaking Velocity, as long as we maintain:
1. `FatxDriveDetection::GetAllFatxDrives()` signature and behavior
2. File-based constructors (path strings)
3. BaseIO-based constructors (for internal use)

---

## Platform-Specific Code Inventory

### Files Requiring Refactoring (Complete List)

| File | Lines | Platforms | Complexity | Priority |
|------|-------|-----------|------------|----------|
| **XboxInternals/IO/DeviceIO.cpp** | ~400 | Win/Linux/Mac | VERY HIGH | CRITICAL |
| **XboxInternals/Fatx/FatxDrive.cpp** | ~1160 | Win/Linux | HIGH | CRITICAL |
| **XboxInternals/Fatx/FatxDriveDetection.cpp** | ~270 | Win/Linux/Mac | VERY HIGH | CRITICAL |
| **XboxInternals/IO/SvodMultiFileIO.cpp** | ~200 | Win/Linux | MEDIUM | HIGH |
| **XboxInternals/Account/Account.cpp** | Unknown | Win/Linux | LOW | MEDIUM |
| **XboxInternals/Gpd/Xdbf.cpp** | Unknown | Win/Linux | LOW | LOW |

---

## Refactoring Strategy by Category

### Category A: Pure C++20 Replacement (No PIMPL Needed)

**Files**: SvodMultiFileIO.cpp (partial), FatxDrive.cpp (partial)

**Operations**:
1. **Directory enumeration**: Replace `FindFirstFile`/`readdir` → `std::filesystem::directory_iterator`
2. **File metadata**: Replace `stat`/`GetFileSize` → `std::filesystem::file_size()`, `exists()`, `last_write_time()`
3. **Path handling**: Replace `std::string`/`std::wstring` → `std::filesystem::path`

**Example Migration**:
```cpp
// BEFORE (Windows)
WIN32_FIND_DATA fi;
HANDLE h = FindFirstFile(L"C:\\Data*", &fi);
if (h != INVALID_HANDLE_VALUE) {
    do {
        // process fi.cFileName
    } while (FindNextFile(h, &fi));
    FindClose(h);
}

// AFTER (C++20 cross-platform)
for (const auto& entry : std::filesystem::directory_iterator("C:\\")) {
    if (entry.path().filename().string().starts_with("Data")) {
        // process entry.path()
    }
}
```

**Estimated Time**: 6-8 hours  
**Risk**: LOW (std::filesystem well-tested)

---

### Category B: Large File I/O (>4GB Support)

**Files**: FatxDrive.cpp `RestoreFromBackup()`, `CreateBackup()`

**Current Issue**: Comment at line 576:
> "fstream is trash. It will only handle files up to 2GB or 4GB"

**Investigation Required**:
1. Test modern std::fstream (C++20 with MinGW 13.1) for >4GB support
2. If YES → Replace Windows/POSIX code with std::fstream
3. If NO → Keep platform-specific code but abstract behind PIMPL

**Test Plan**:
```cpp
// Test harness (5 minutes)
std::ofstream test("test_5gb.bin", std::ios::binary);
std::vector<char> buffer(1024*1024, 'X');  // 1MB
for (int i = 0; i < 5000; i++) {  // 5GB total
    test.write(buffer.data(), buffer.size());
    if (test.fail()) {
        std::cout << "Failed at " << (i+1) << " MB\n";
        return;
    }
}
test.close();
std::cout << "5GB write successful\n";
```

**Decision Tree**:
- **If test passes**: Replace with std::fstream (2-3h refactoring)
- **If test fails**: Implement `ILargeFileIO` PIMPL (6-8h)

**Estimated Time**: 1h test + 2-8h implementation = **3-9 hours**  
**Risk**: MEDIUM (depends on std::fstream capability)

---

### Category C: Raw Device I/O (PIMPL Required)

**Files**: DeviceIO.cpp

**Platforms**: Windows / Linux / macOS (all three implemented)

**Operations**:
- Open physical device (`\\.\PhysicalDrive0`, `/dev/rdisk0`, `/dev/sda`)
- Read/write raw sectors
- Get device geometry (`IOCTL_DISK_GET_DRIVE_GEOMETRY`, `ioctl DKIOCGETBLOCKCOUNT`)
- Seek to specific offsets

**Current Implementation**: Already has three-way split with `#ifdef _WIN32 / __APPLE__ / __linux__`

**PIMPL Design**:
```cpp
// DeviceIO.h (public, platform-agnostic)
class XBOXINTERNALSSHARED_EXPORT DeviceIO : public BaseIO {
public:
    DeviceIO(const std::filesystem::path& devicePath);  // String constructor
    DeviceIO(const std::wstring& devicePath);  // Legacy wstring constructor
    ~DeviceIO() override;
    
    // BaseIO interface (already abstract)
    void SetPosition(UINT64 position, std::ios_base::seekdir dir) override;
    UINT64 GetPosition() override;
    UINT64 Length() override;
    void ReadBytes(BYTE* outBuffer, DWORD len) override;
    void WriteBytes(BYTE* buffer, DWORD len) override;
    void Close() override;
    void Flush() override;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// DeviceIO_win.cpp
#include <windows.h>
class DeviceIO::Impl {
    HANDLE deviceHandle;
    LARGE_INTEGER offset;
    // ... Windows implementation
};

// DeviceIO_posix.cpp (Linux + macOS shared code)
#include <fcntl.h>
#include <unistd.h>
class DeviceIO::Impl {
    int deviceFd;
    off_t offset;
    // ... POSIX implementation
};

// DeviceIO_mac.cpp (macOS-specific geometry)
#include <sys/disk.h>
#include <sys/ioctl.h>
// Override geometry method for macOS DKIOCGETBLOCKCOUNT
```

**CMakeLists.txt Update**:
```cmake
if(WIN32)
    target_sources(XboxInternals PRIVATE IO/DeviceIO_win.cpp)
elseif(APPLE)
    target_sources(XboxInternals PRIVATE 
        IO/DeviceIO_posix.cpp  # Shared POSIX base
        IO/DeviceIO_mac.cpp    # macOS overrides
    )
elseif(UNIX)  # Linux
    target_sources(XboxInternals PRIVATE IO/DeviceIO_posix.cpp)
endif()
```

**Migration Steps**:
1. Create Impl forward declaration in DeviceIO.h (30 min)
2. Extract Windows implementation to DeviceIO_win.cpp (2-3h)
3. Extract POSIX implementation to DeviceIO_posix.cpp (2-3h)
4. Handle macOS-specific ioctl in DeviceIO_mac.cpp (1-2h)
5. Update DeviceIO.cpp to delegate to pImpl (1h)
6. Remove platform includes from DeviceIO.h (15 min)
7. Test on Windows, Linux (WSL), macOS (if available) (2-3h)

**Estimated Time**: **12-16 hours**  
**Risk**: MEDIUM (existing code works, just refactoring structure)

---

### Category D: Device Enumeration (PIMPL Required - Most Complex)

**Files**: FatxDriveDetection.cpp

**Platforms**: Windows / Linux / macOS (all three implemented)

**Operations**:
- **Physical drives**: Enumerate `\\.\PHYSICALDRIVE*`, `/dev/sd*`, `/dev/disk*`
- **Logical drives**: Find Xbox360 folders on `C:\`, `/Volumes/*`, mounted filesystems
- **Multi-file detection**: Search for `Data0000`, `Data0001`, `Data0002` pattern

**Current Implementation**: Three-way platform split with complex logic

**PIMPL Design**:
```cpp
// FatxDriveDetection.h (public, platform-agnostic)
class XBOXINTERNALSSHARED_EXPORT FatxDriveDetection {
public:
    static std::vector<FatxDrive*> GetAllFatxDrives();  // Keep signature!
    
private:
    static std::vector<std::wstring> getLogicalDrives();
    static std::vector<DeviceIO*> getPhysicalDisks();
    
    // Platform-specific helpers (PIMPL)
    class PlatformImpl;
    static std::unique_ptr<PlatformImpl> CreatePlatformImpl();
};

// FatxDriveDetection_win.cpp
#include <windows.h>
class FatxDriveDetection::PlatformImpl {
public:
    std::vector<std::wstring> EnumerateLogicalDrives();
    std::vector<DeviceIO*> EnumeratePhysicalDevices();
};
// Implement using GetLogicalDrives(), FindFirstFile(), CreateFile()

// FatxDriveDetection_linux.cpp
#include <mntent.h>
#include <dirent.h>
class FatxDriveDetection::PlatformImpl {
public:
    std::vector<std::wstring> EnumerateLogicalDrives();  // /proc/mounts
    std::vector<DeviceIO*> EnumeratePhysicalDevices();   // /dev/sd*
};

// FatxDriveDetection_mac.cpp
#include <dirent.h>
#include <sys/disk.h>
class FatxDriveDetection::PlatformImpl {
public:
    std::vector<std::wstring> EnumerateLogicalDrives();  // /Volumes/
    std::vector<DeviceIO*> EnumeratePhysicalDevices();   // /dev/rdisk*
};
```

**Migration Steps**:
1. Create PlatformImpl interface (1h)
2. Extract Windows enumeration to _win.cpp (3-4h)
3. Extract Linux enumeration to _linux.cpp (3-4h)
4. Extract macOS enumeration to _mac.cpp (3-4h)
5. Integrate with std::filesystem for Data* search (1-2h)
6. Test on all three platforms (2-3h)

**Estimated Time**: **13-18 hours**  
**Risk**: HIGH (three platforms, complex logic, used by Device Viewer)

---

### Category E: Minor Platform Code (Low Priority)

**Files**: Account.cpp, Xdbf.cpp

**Platforms**: Windows / Linux (likely file path differences)

**Strategy**: 
1. Audit actual usage (T006 - 30 min each)
2. Replace with std::filesystem::path if simple (1h each)
3. If complex, apply PIMPL (defer to end)

**Estimated Time**: **2-4 hours**  
**Risk**: LOW (likely simple path handling)

---

## Phased Implementation Plan

### Phase 2A: Quick Wins & Testing (Day 1)
**Duration**: 6-8 hours

**Tasks**:
- [ ] **T008-1**: Replace all `std::string` file paths with `std::filesystem::path` (2-3h)
  - Files: FileIO.h/cpp, FatxDrive.cpp, all callers
  - Test: Verify paths work on Windows
  
- [ ] **T008-2**: Test large file (>4GB) support with modern std::fstream (1h)
  - Create 5GB test file
  - Decision: Keep or replace platform code

- [ ] **T008-3**: Replace `FindFirstFile`/`readdir` with `std::filesystem::directory_iterator` (2-3h)
  - File: SvodMultiFileIO.cpp (Windows `FindFirstFile`)
  - File: FatxDriveDetection.cpp (Data* file search)
  - Test: Multi-file SVOD still works

- [ ] **T008-4**: Replace `stat` with `std::filesystem::file_size()` (1h)
  - File: FatxDrive.cpp `InjectFile()`

**Deliverables**:
- ~40% platform-specific code eliminated
- Large file strategy decided
- All changes committed and built successfully

---

### Phase 2B: Large File Abstraction (Day 2 Morning)
**Duration**: 3-9 hours (depends on std::fstream test results)

**Scenario A: std::fstream works** (3-4h)
- [ ] **T009-1**: Replace `CreateFile`/`ReadFile`/`WriteFile` with `std::fstream` in `RestoreFromBackup()`
- [ ] **T009-2**: Replace POSIX `open`/`read`/`write` with `std::fstream` in same function
- [ ] **T009-3**: Test >4GB backup restore on Windows

**Scenario B: std::fstream fails** (6-9h)
- [ ] **T010-1**: Create `ILargeFileIO` interface
- [ ] **T011-1**: Implement `LargeFileIO_win.cpp`
- [ ] **T011-2**: Implement `LargeFileIO_posix.cpp`
- [ ] **T011-3**: Integrate into `FatxDrive::RestoreFromBackup()`/`CreateBackup()`

**Deliverables**:
- Large file operations work on all platforms
- No 2GB/4GB limits

---

### Phase 2C: DeviceIO PIMPL (Day 2 Afternoon + Day 3)
**Duration**: 12-16 hours

**Day 2 Afternoon** (4-5h):
- [ ] **T010-2**: Create DeviceIO::Impl forward declaration
- [ ] **T011-4**: Extract Windows implementation to `DeviceIO_win.cpp`
- [ ] **T011-5**: Remove `#include <windows.h>` from DeviceIO.cpp (core file)
- [ ] **T012-1**: Build and test on Windows

**Day 3 Morning** (4-5h):
- [ ] **T011-6**: Extract POSIX implementation to `DeviceIO_posix.cpp`
- [ ] **T011-7**: Create macOS-specific overrides in `DeviceIO_mac.cpp` (if needed)
- [ ] **T012-2**: Build and test on Linux (WSL or VM)

**Day 3 Afternoon** (4-6h):
- [ ] **T012-3**: Test on macOS (if available) or request community testing
- [ ] **T012-4**: Smoke test Device Viewer with physical Xbox drives (if available)
- [ ] **T012-5**: Verify DeviceIO public header has NO platform includes

**Deliverables**:
- DeviceIO fully abstracted
- Works on Windows/Linux/macOS
- Public headers clean

---

### Phase 2D: FatxDriveDetection PIMPL (Day 4 + Day 5 Morning)
**Duration**: 13-18 hours

**Day 4** (8h):
- [ ] **T010-3**: Design PlatformImpl interface for device enumeration
- [ ] **T011-8**: Implement Windows enumeration (`FatxDriveDetection_win.cpp`)
  - `GetLogicalDrives()` → drive letters
  - `CreateFile()` → probe PHYSICALDRIVE*
  - `FindFirstFile()` → already replaced with std::filesystem in Phase 2A
  
- [ ] **T011-9**: Implement Linux enumeration (`FatxDriveDetection_linux.cpp`)
  - `/proc/mounts` → logical drives
  - `/dev/sd*` → physical devices

**Day 5 Morning** (5-10h):
- [ ] **T011-10**: Implement macOS enumeration (`FatxDriveDetection_mac.cpp`)
  - `/Volumes/` → logical drives
  - `/dev/rdisk*` → physical devices
  
- [ ] **T012-6**: Build and test Device Viewer on all three platforms
  - Windows: Should detect Xbox 360 USB/HDD
  - Linux: Should detect mounted Xbox storage
  - macOS: Should detect /Volumes/*/Xbox360/

**Deliverables**:
- Device enumeration works on all platforms
- `GetAllFatxDrives()` signature unchanged
- Device Viewer fully functional

---

### Phase 2E: Cleanup & Minor Files (Day 5 Afternoon)
**Duration**: 2-4 hours

- [ ] **T006**: Audit Account.cpp and Xdbf.cpp platform code (30 min each)
- [ ] **T008-5**: Replace with std::filesystem if simple (1h each)
- [ ] **T010/T011**: Apply PIMPL if complex (defer if time constrained)

**Deliverables**:
- All identified platform code addressed
- Documentation updated

---

### Phase 2F: Final Verification (Day 6)
**Duration**: 4-6 hours

- [ ] **T012-7**: Header audit - verify NO platform includes in public headers
  ```powershell
  grep -r "#include <windows.h>" XboxInternals/**/*.h
  grep -r "#include <sys/" XboxInternals/**/*.h
  grep -r "HANDLE\|void\*" XboxInternals/**/*.h
  ```

- [ ] **T012-8**: Build from scratch on all platforms
  ```powershell
  # Windows
  rm -r out/build/windows-mingw-debug
  cmake --preset windows-mingw-debug
  cmake --build --preset windows-mingw-debug
  
  # Linux (WSL or VM)
  cmake -B build-linux -DCMAKE_BUILD_TYPE=Debug
  cmake --build build-linux
  
  # macOS (if available)
  cmake -B build-mac -DCMAKE_BUILD_TYPE=Debug
  cmake --build build-mac
  ```

- [ ] **T012-9**: Comprehensive smoke tests (per quickstart.md)
  - Open FATX device image (all platforms)
  - Open STFS package (all platforms)
  - Extract files (all platforms)
  - Device Viewer enumerate drives (all platforms)
  - Create/restore backup (Windows - if >4GB test files available)

- [ ] **T012-10**: Update plan.md Constitution Check
  - Platform Abstraction: ⚠️ → ✅ PASS

**Deliverables**:
- Constitution III-A fully compliant
- All platforms verified working
- Documentation complete

---

## Risk Mitigation

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| **std::fstream can't handle >4GB** | MEDIUM | MEDIUM | Have PIMPL fallback ready (Scenario B) |
| **macOS-specific bugs** | MEDIUM | MEDIUM | Keep existing macOS code structure, just refactor |
| **Linux `/proc/mounts` parsing breaks** | LOW | MEDIUM | Extensive testing on WSL + Linux VM |
| **Device Viewer stops working** | LOW | HIGH | Test after each phase; keep working fallback |
| **Build breaks on non-Windows** | MEDIUM | HIGH | Test builds on Linux early (Day 3) |
| **Performance regression** | LOW | LOW | Existing code well-optimized; C++20 should match |

---

## Testing Strategy

### Per-Phase Testing
- **After each phase**: Build on Windows, run affected smoke tests
- **After Phase 2C**: Build on Linux, test DeviceIO
- **After Phase 2D**: Test Device Viewer on all platforms
- **After Phase 2F**: Full regression suite

### Platform Test Matrix

| Feature | Windows | Linux | macOS | Notes |
|---------|---------|-------|-------|-------|
| **Open FATX file image** | ✅ Local | ✅ Linux | ⏳ macOS untested | File-based, should work everywhere |
| **Open STFS package** | ✅ Local | ✅ Linux | ⏳ macOS untested | File-based, should work everywhere |
| **Extract files** | ✅ Local | ✅ Linux | ⏳ macOS untested | File-based, should work everywhere |
| **Device Viewer (USB)** | ✅ Local | ✅ Linux w/ HDD | ⏳ macOS untested | Xbox 360 HDD with USB adapter available! |
| **Device Viewer (HDD)** | ✅ Local | ✅ Linux w/ HDD | ⏳ macOS untested | Xbox 360 HDD with USB adapter available! |
| **Create backup >4GB** | ✅ Local | ✅ WSL | ⏳ Request test | If std::fstream works |
| **Restore backup >4GB** | ✅ Local | ✅ WSL | ⏳ Request test | If std::fstream works |

**Legend**:
- ✅ = Can test locally
- ⏳ = Needs community/CI or deferred
- ⚠️ = Known limitation

---

## Success Criteria

### Functional
- [ ] All smoke tests pass on Windows (primary platform)
- [ ] All file-based operations work on Linux/macOS
- [ ] Device Viewer successfully enumerates drives on Windows
- [ ] No functionality removed or degraded
- [ ] Backup/restore works for >4GB files

### Code Quality (Constitution III-A)
- [ ] Zero platform includes in public headers (`XboxInternals/**/*.h`)
- [ ] All platform code hidden behind PIMPL or replaced with C++20
- [ ] No `void*` type punning in public API
- [ ] Clean separation: platform code in `*_win.cpp`, `*_posix.cpp`, `*_mac.cpp`

### Build
- [ ] Clean build on Windows (MinGW)
- [ ] Clean build on Linux (GCC or Clang)
- [ ] Clean build on macOS (if available)
- [ ] CMake properly selects platform implementations

### Documentation
- [ ] `platform-audit.md` updated with final state
- [ ] `plan.md` Constitution Check shows ✅ PASS
- [ ] Code comments explain platform differences where PIMPL used
- [ ] README.md updated if build steps changed

---

## Timeline Summary

| Phase | Duration | Days | Deliverable |
|-------|----------|------|-------------|
| **2A: Quick Wins** | 6-8h | Day 1 | C++20 replacements, 40% reduction |
| **2B: Large Files** | 3-9h | Day 2 AM | >4GB support resolved |
| **2C: DeviceIO PIMPL** | 12-16h | Day 2 PM + Day 3 | Raw device I/O abstracted |
| **2D: FatxDriveDetection PIMPL** | 13-18h | Day 4 + Day 5 AM | Device enumeration abstracted |
| **2E: Cleanup** | 2-4h | Day 5 PM | Minor files handled |
| **2F: Verification** | 4-6h | Day 6 | Full platform testing |
| **TOTAL** | **40-61h** | **5-7 days** | Constitution III-A ✅ |

**Realistic Estimate**: **5-6 days** (assuming 8h days, normal debugging)

---

## Open Questions - ANSWERED

1. **macOS Testing**: Do you have access to a Mac for testing, or should we request community testing?
   - ✅ **ANSWER**: NO Mac available → macOS code will be refactored but marked as "untested, community verification needed"

2. **Linux Physical Device Access**: Do you have a Linux system with Xbox 360 drives for Device Viewer testing?
   - ✅ **ANSWER**: YES - Linux system available + Xbox 360 HDD with USB adapter → Full Device Viewer testing possible on Linux!

3. **Large File Priority**: Should we prioritize making std::fstream work (cleaner), or go straight to PIMPL (more conservative)?
   - ✅ **STRATEGY**: Try std::fstream first (1h test), fall back to PIMPL if needed

4. **Breaking Changes**: Are you OK with removing the `FatxDrive(void* deviceHandle)` constructor from public API?
   - ✅ **SAFE**: Cross-reference analysis confirmed Velocity doesn't use it → safe to remove

---

## Next Action

**Ready to proceed with Phase 2A (Day 1)?**

This will start with:
1. Replace `std::string` paths → `std::filesystem::path` (2-3h)
2. Test >4GB std::fstream support (1h)
3. Replace directory enumeration APIs (2-3h)

**Estimated Day 1 completion**: ~40% platform code eliminated, large file strategy decided

**Shall I begin Phase 2A now?**
