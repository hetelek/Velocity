# Phase 2: Platform Abstraction - Strategy Review

**Date**: 2025-10-19  
**Priority**: TOP PRIORITY (BLOCKING for all other work)  
**Constitutional Mandate**: Constitution III-A  
**Tasks**: T004-T012 (9 tasks total)  
**Estimated Duration**: 2-3 days

## Executive Summary

### Current State Assessment (**AUDIT COMPLETE - T004**)

**⚠️ CORRECTION**: Initial grep was INCOMPLETE. Comprehensive audit reveals SIGNIFICANT platform dependencies:

**The Good**:
- ✅ No direct platform includes in `.h` **headers** (public API headers are clean)
- ✅ No Qt dependencies found in XboxInternals
- ✅ Uses `std::fstream` in BaseIO/FileIO for regular file I/O

**The Bad**:
- ❌ **4 .cpp files** have `#include <Windows.h>` or `<windows.h>`
- ❌ **20+ instances** of `#ifdef _WIN32` conditional compilation
- ❌ Heavy Windows API usage: `CreateFile`, `ReadFile`, `WriteFile`, `SetFilePointer`, `DeviceIoControl`, etc.
- ❌ Heavy POSIX API usage: `open`, `read`, `write`, `lseek`, `stat`, `ioctl`, etc.
- ❌ Public API contamination: `FatxDrive(void* deviceHandle)` constructor exposes platform types

**Files Requiring Refactoring**:
1. **XboxInternals/IO/DeviceIO.cpp** - Raw device I/O (16-20h effort, PIMPL required)
2. **XboxInternals/Fatx/FatxDrive.cpp** - Large file I/O (6-8h effort, PIMPL likely)
3. **XboxInternals/Fatx/FatxDriveDetection.cpp** - Device enumeration (4-6h effort, PIMPL required)
4. **XboxInternals/IO/SvodMultiFileIO.cpp** - Directory enumeration (2-3h effort, use std::filesystem)

**Updated Estimate**: 3-5 days (27-38 hours) - **aggressive timeline confirmed accurate**

**See**: `platform-audit.md` for complete findings

### Constitutional Requirements (III-A)

XboxInternals and shared library code MUST:
1. **Prefer C++20 standard library** over platform-specific APIs
2. **Abstract unavoidable platform code** behind PIMPL idiom
3. **Hide platform dependencies** from public headers
4. **Enable cross-platform builds** without OS-specific types leaking
5. **Maintain original functionality** during migration

**Preference Order**:
1. C++20 std library (always preferred)
2. Platform-specific code with PIMPL (only when C++20 inadequate)

## Phase 2 Task Breakdown

### Stage 1: Audit & Discovery (T004-T007) - Day 1 Morning

**Parallel Execution Possible**: T005, T006, T007 can run concurrently

#### T004: Audit Public Headers (1-2 hours)
**Goal**: Scan all `XboxInternals/**/*.h` files for platform-specific types

**Search Targets**:
- Windows types: `HANDLE`, `HWND`, `HMODULE`, `SOCKET`, `OVERLAPPED`
- POSIX types: `int fd`, `DIR*`, `pthread_t`, `pthread_mutex_t`
- Platform includes: `<windows.h>`, `<unistd.h>`, `<sys/types.h>`, `<pthread.h>`
- Non-standard types that might hide platform code

**Deliverable**: `platform-audit.md` documenting:
- List of headers exposing platform types
- Specific line numbers and type names
- Severity (public API surface vs. private implementation detail)

**Initial Findings**: Likely CLEAN (preliminary grep showed no issues)

#### T005: File I/O Hotspots (1-2 hours, parallel)
**Goal**: Identify all file operations and categorize by refactoring strategy

**Search Targets**:
- `fopen`, `fread`, `fwrite`, `fseek`, `FILE*` (C-style)
- Platform-specific: `CreateFile`, `ReadFile`, `WriteFile` (Windows), `open`, `read`, `write` (POSIX)
- Current C++ usage: `std::fstream`, `std::ifstream`, `std::ofstream`
- Path handling: char*/wchar_t* paths vs. `std::filesystem::path`

**Categories**:
- ✅ **Already C++20**: Using `std::fstream` (minimal/no work)
- 🔄 **Replaceable**: Can migrate to `std::filesystem` + `std::fstream`
- 🔒 **Requires PIMPL**: Device access, raw sector I/O, memory-mapped files

**Initial Findings**: `BaseIO`/`FileIO` already use `std::fstream`; likely minimal work needed

**Deliverable**: Hotspot inventory with categorization in `platform-audit.md`

#### T006: Threading/Sync Hotspots (1-2 hours, parallel)
**Goal**: Identify concurrency primitives and categorize

**Search Targets**:
- Platform-specific: `CreateThread`, `WaitForSingleObject`, `CRITICAL_SECTION` (Windows), `pthread_create`, `pthread_mutex_lock` (POSIX)
- Current C++ usage: `std::thread`, `std::mutex`, `std::condition_variable`, `std::atomic`
- Lock primitives, semaphores, events

**Categories**:
- ✅ **Already C++20**: Using `std::thread`/`std::mutex`
- 🔄 **Replaceable**: Can migrate to C++20 concurrency
- 🔒 **Requires PIMPL**: Platform-specific synchronization (unlikely)

**Initial Findings**: No grep matches for platform threading; likely already clean or not used

**Deliverable**: Threading inventory in `platform-audit.md`

#### T007: Crypto/Botan Integration (1 hour, parallel)
**Goal**: Verify Botan integration doesn't expose platform dependencies

**Search Targets**:
- Botan includes leaking into public headers
- Platform-specific crypto APIs (CNG, OpenSSL with platform ifdefs)
- Random number generation (`/dev/urandom`, `BCryptGenRandom`)

**Deliverable**: Crypto integration assessment in `platform-audit.md`

**Expected Outcome**: Botan abstracts platform crypto; likely no issues

---

### Stage 2: Refactoring (T008-T011) - Day 1 Afternoon to Day 2

**Sequential Execution**: Each builds on audit results

#### T008: Replace Platform File I/O (4-6 hours)
**Goal**: Migrate remaining C-style or platform-specific file I/O to C++20

**Actions**:
- Replace `fopen`/`fclose` with `std::fstream` RAII
- Replace `char*` paths with `std::filesystem::path`
- Use `std::filesystem::file_size`, `std::filesystem::exists`, `std::filesystem::directory_iterator`
- Ensure error handling uses C++ exceptions or error codes (not errno directly)

**Files to Review** (based on XboxInternals structure):
- `XboxInternals/IO/FileIO.cpp` (already uses `std::fstream`, may need path upgrade)
- `XboxInternals/Fatx/FatxDriveDetection.cpp` (has `/proc/mounts` call - POSIX-specific)
- Any device/partition enumeration code

**Risk**: Device-level operations (raw partition access) may not be portable via std::filesystem alone

**Deliverable**: Updated .cpp files using C++20 file APIs

#### T009: Replace Platform Threading (2-4 hours)
**Goal**: Migrate to `std::thread`, `std::mutex`, `std::condition_variable`, `std::atomic`

**Actions**:
- Replace platform thread creation with `std::thread`
- Replace mutexes/critical sections with `std::mutex`, `std::lock_guard`, `std::unique_lock`
- Replace condition variables with `std::condition_variable`
- Replace interlocked/atomic ops with `std::atomic<T>`

**Files to Review**:
- Search results from T006 audit
- Long-running operations that might spawn threads (package extraction, device enumeration)

**Deliverable**: Updated .cpp files using C++20 concurrency

**Expected Outcome**: Likely minimal work if threading is currently minimal/absent

#### T010: Create Abstract Interfaces (2-3 hours)
**Goal**: For unavoidable platform code, define clean C++ interfaces

**Candidates** (based on audit):
- **Device enumeration**: `IDeviceEnumerator` (Windows: `SetupDiGetClassDevs`, POSIX: `/dev`, `/proc/mounts`)
- **Raw sector I/O**: `IRawDiskAccess` (Windows: `CreateFile` with `\\.\PhysicalDrive`, POSIX: `/dev/sdX`)
- **Platform paths**: Likely replaceable with `std::filesystem::path` (no interface needed)

**Interface Design Principles**:
- Pure virtual base class (no platform types in signature)
- Return `std::expected<T, std::error_code>` or throw exceptions (no errno, GetLastError)
- Use `std::vector<std::byte>`, `std::span<std::byte>` for buffers (not `BYTE*`, `void*`)
- Document contracts in header comments

**Example**:
```cpp
// XboxInternals/IO/IDeviceEnumerator.h
class IDeviceEnumerator {
public:
    struct DeviceInfo {
        std::filesystem::path path;
        std::string friendlyName;
        uint64_t sizeBytes;
    };
    
    virtual ~IDeviceEnumerator() = default;
    virtual std::vector<DeviceInfo> EnumerateRemovableDevices() = 0;
};
```

**Deliverable**: New interface headers in `XboxInternals/` (abstract, no platform code)

#### T011: Implement PIMPL Wrappers (4-6 hours)
**Goal**: Implement platform-specific code behind PIMPL idiom

**Pattern**:
```cpp
// XboxInternals/IO/DeviceEnumerator.h (public, cross-platform)
class DeviceEnumerator : public IDeviceEnumerator {
public:
    DeviceEnumerator();
    ~DeviceEnumerator() override;
    std::vector<DeviceInfo> EnumerateRemovableDevices() override;
private:
    class Impl; // forward declaration
    std::unique_ptr<Impl> pImpl;
};

// XboxInternals/IO/DeviceEnumerator_win.cpp (Windows-specific)
#include <windows.h>
#include <setupapi.h>
class DeviceEnumerator::Impl {
    // Windows implementation details
};

// XboxInternals/IO/DeviceEnumerator_posix.cpp (POSIX-specific)
#include <unistd.h>
#include <sys/types.h>
class DeviceEnumerator::Impl {
    // POSIX implementation details
};
```

**CMake Integration**:
```cmake
if(WIN32)
    target_sources(XboxInternals PRIVATE IO/DeviceEnumerator_win.cpp)
else()
    target_sources(XboxInternals PRIVATE IO/DeviceEnumerator_posix.cpp)
endif()
```

**Files to Create**:
- Platform-agnostic headers (no platform includes)
- `*_win.cpp` (Windows implementation)
- `*_posix.cpp` (Linux/macOS implementation)
- Update `XboxInternals/CMakeLists.txt` for conditional compilation

**Deliverable**: PIMPL implementation files, updated CMake config

---

### Stage 3: Verification (T012) - Day 3

#### T012: Clean Compilation & Smoke Test (2-4 hours)
**Goal**: Ensure refactoring didn't break functionality

**Actions**:
1. **Header Scan**: Verify no platform includes remain in public headers
   ```powershell
   grep -r "#include <windows.h>" XboxInternals/**/*.h
   grep -r "HANDLE\|HWND" XboxInternals/**/*.h
   ```

2. **Clean Build**: Rebuild from scratch
   ```powershell
   rm -r out/build/windows-mingw-debug
   cmake --preset windows-mingw-debug
   cmake --build --preset windows-mingw-debug
   ```

3. **Smoke Test Affected Flows**:
   - Open FATX device image (file I/O + device enumeration if changed)
   - Open STFS package (file I/O)
   - Extract files (file I/O + threading if used)
   - Check error handling (platform error codes → C++ exceptions)

4. **Velocity Compilation**: Verify GUI still compiles cleanly
   ```powershell
   # Should complete without errors
   cmake --build --preset windows-mingw-debug --target Velocity
   ```

**Success Criteria**:
- ✅ No platform includes in `XboxInternals/**/*.h`
- ✅ Clean compilation on Windows (MinGW)
- ✅ FATX/STFS/GPD smoke tests pass
- ✅ No runtime errors or crashes
- ✅ Performance not degraded (check large file operations)

**Deliverable**: Updated `plan.md` Constitution Check (Platform Abstraction ⚠️ → ✅)

---

## Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| **Hidden platform dependencies** | Low | High | Thorough T004-T007 audit with multiple search patterns |
| **Device I/O not portable via std lib** | Medium | Medium | Expected; use PIMPL per T010-T011 |
| **Performance regression** | Low | Medium | Benchmark before/after; C++20 std lib should be equivalent |
| **Breaking existing functionality** | Low | High | Comprehensive smoke tests in T012; RAII reduces bugs |
| **Scope creep** (over-refactoring) | Medium | Low | Stick to audit findings; don't refactor unrelated code |

## Success Metrics

### Must-Have (BLOCKING)
- [ ] Zero platform includes in `XboxInternals/**/*.h` public headers
- [ ] Clean compilation on Windows MinGW
- [ ] All smoke tests pass (FATX, STFS, GPD open/extract)
- [ ] Constitution III-A compliance verified

### Should-Have (Quality)
- [ ] `platform-audit.md` documents all findings and decisions
- [ ] CMake properly handles conditional platform sources
- [ ] Error handling uses C++ exceptions/error_code (not errno)
- [ ] Code follows C++20 idioms (RAII, smart pointers, std::filesystem)

### Nice-to-Have (Stretch)
- [ ] Cross-platform build verified (Linux or macOS if available)
- [ ] Performance benchmarks show no regression
- [ ] Public API simplified due to cleaner abstractions

## Open Questions

1. **Device Enumeration**: Does Velocity currently enumerate removable devices, or only open files via dialogs?
   - **Impact**: If only file dialogs, no device enumeration code needed (reduces scope)
   - **Action**: Check for device picker UI in Velocity/

2. **FatxDriveDetection.cpp**: Is the `/proc/mounts` code actively used on Windows?
   - **Impact**: If Windows-only, can stub out POSIX code or remove if unused
   - **Action**: Review call sites in T005

3. **Threading**: Does XboxInternals spawn threads, or is threading handled by Velocity/ GUI layer?
   - **Impact**: If no threading, T006/T009 are trivial
   - **Action**: Search for thread creation in audit

4. **Existing Cross-Platform Support**: Was the codebase ever built on Linux/macOS?
   - **Impact**: If yes, PIMPL code might already exist (just needs cleanup)
   - **Action**: Check git history or build scripts

## Preliminary Recommendations

Based on initial grep results showing clean headers and std::fstream usage:

### Conservative Approach (Recommended)
1. **Run full audit** (T004-T007) to confirm preliminary findings
2. **Focus on FatxDriveDetection.cpp** (only identified platform-specific code)
3. **Upgrade path handling** to `std::filesystem::path` where still using `std::string`
4. **Light PIMPL** for device enumeration if confirmed necessary
5. **Document "already compliant"** areas in audit (celebrate good existing architecture)

**Estimated Time**: 1-2 days (faster than initial 2-3 day estimate)

### Aggressive Approach (If Audit Reveals Issues)
1. **Full PIMPL refactoring** for all platform-specific code
2. **Comprehensive std::filesystem migration** (paths, file metadata, directory iteration)
3. **Threading abstraction** if platform threads found
4. **Multi-platform build testing** (requires Linux VM or CI setup)

**Estimated Time**: 3-4 days

## Next Steps

**Immediate Action**: Execute T004 (audit public headers) to gather data

```powershell
# Start audit
cd C:\Users\holvo\Documents\gitkraken\Velocity

# Search for platform includes in headers
grep -r "#include <windows.h>" XboxInternals/**/*.h
grep -r "#include <unistd.h>" XboxInternals/**/*.h
grep -r "pthread" XboxInternals/**/*.h

# Search for platform types
grep -r "HANDLE\|HWND\|HMODULE" XboxInternals/**/*.h
grep -r "pthread_t\|pthread_mutex" XboxInternals/**/*.h

# Document findings in platform-audit.md
```

After T004 completes, we'll have hard data to choose conservative vs. aggressive approach.

---

**Decision Point**: Should we proceed with T004 audit now, or would you like to review/adjust the strategy first?
