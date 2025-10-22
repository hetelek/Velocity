# Implementation Plan: Velocity Baseline Capabilities

**Branch**: `001-baseline-capabilities` | **Date**: 2025-10-21 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/001-baseline-capabilities/spec.md`

**Status**: Plan complete; Phase 0 (research.md), Phase 1 (data-model.md, contracts/, quickstart.md) artifacts exist; ready for Phase 2 (tasks.md generation via /speckit.tasks)

## Summary

Velocity is a desktop application for browsing, editing, and creating Xbox 360 files and storage formats. The baseline capabilities feature establishes comprehensive functionality across 9 user stories (browse/extract, profile inspection, safe editing with backups, content creation, profile management, device/FATX utilities, plugin system, specialized format support, and utility tools). Platform abstraction work is complete (T004-T012), with C++20 standard library replacing Windows-specific APIs where possible and PIMPL wrapping unavoidable platform dependencies. Recent GUI modernization (2025-10-20 to 2025-10-21) added XML, Text, and ZIP file viewers with proper encoding detection. Implementation follows backup-or-abort safety policy: all write operations require successful backup creation before proceeding.

## Technical Context

**Language/Version**: C++20 (modern C++ with Qt idioms)  
**Primary Dependencies**: Qt 6.7.3+ (GUI, widgets, core), Botan 3.x (cryptography), MinGW 13.1.0+ (primary Windows compiler)  
**Storage**: Filesystem-based (FATX device images, STFS packages, GPD files, disc images); QSettings for application preferences  
**Testing**: Manual smoke tests (FATX, STFS, GPD flows); CTest framework available for future test suites; Qt Test or GoogleTest for unit tests  
**Target Platform**: Windows (primary), cross-platform capable (macOS/Linux via Qt)  
**Project Type**: Desktop application (CMake superproject: Qt GUI client + reusable C++ library)  
**Performance Goals**: UI responsiveness (progress feedback <0.5s), cancelable operations, 10-second target for 100-500 achievement profile operations (SC-009)  
**Constraints**: Xbox 360 format compliance (FATX, STFS, GPD, XDBF, SVOD, GDFX), cryptographic validation (signatures, certificates), offline-capable (no required network dependencies), backup-or-abort safety policy (never write without backup)  
**Scale/Scope**: ~140 source files across Velocity/ (GUI) and XboxInternals/ (library), 44 functional requirements, 9 user stories, 78 implementation tasks, 20+ specialized dialogs/wizards, plugin system for community extensions

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

### I. Superproject & Module Boundaries ✅ PASS

**Evidence**: 
- CMake superproject structure intact: `Velocity/` (Qt GUI), `XboxInternals/` (pure C++ library), `cmake/` (helper macros)
- XboxInternals remains Qt-independent per Constitution I requirement
- Platform abstraction completed (T004-T012): C++20 std lib replaces platform APIs; PIMPL isolates unavoidable platform code
- Build artifacts confined to `out/build/<preset>/` (never committed)
- Git commits: 9a91471 (SvodMultiFileIO), 83ab1e0 (FatxDrive), 6d6431e (FatxDriveDetection), 495d07b (DeviceIO PIMPL verified)

**Assessment**: No violations. Module boundaries respected; library remains reusable for CLI/server contexts.

---

### II. Reproducible Builds via Presets ✅ PASS

**Evidence**:
- CMakePresets.json defines `windows-mingw-debug` and `windows-mingw-release` presets
- Qt 6.7.3+ auto-detection works via `find_package(Qt6 REQUIRED COMPONENTS Core Widgets)`
- `compile_commands.json` exported for clangd/tooling support
- Clean builds prune `out/build/<preset>/` without touching sources
- BUILD.md documents preset usage and Qt validation (`qmake6 --version`)

**Assessment**: No violations. Preset-based workflow operational.

---

### III. C++20 with Qt Idioms ✅ PASS

**Evidence**:
- Code follows four-space indentation, braces on same line, CamelCase/lowerCamelCase naming
- Qt containers (QList, QString, QByteArray) and signals/slots used throughout
- qthelpers.cpp provides QFile operation wrappers (e.g., case-insensitive extension detection)
- Recent GUI modernization maintains consistent coding style (xmldialog, textdialog, zipviewer implementations)

**Assessment**: No violations. Style guide consistently applied.

---

### III-A. Platform Abstraction and Implementation Hiding ✅ PASS

**Evidence**:
- **Preference order respected**: C++20 std lib → Qt (GUI only) → PIMPL (unavoidable platform code)
- **XboxInternals migration complete**: `std::filesystem` replaces Win32 file I/O; DeviceIO uses existing PIMPL pattern (commits a792e92, 495d07b)
- **Public headers clean**: TypeDefinitions.h is only legitimate platform conditional; no leaking Windows handles or POSIX file descriptors
- **Velocity/ uses Qt abstractions**: QFile, QThread, QMutex for GUI-specific needs
- **Tasks T004-T012 marked complete** with git commit references

**Assessment**: No violations. Platform abstraction work complete per Constitution III-A requirements.

---

### IV. Testing Mindset and Coverage Growth ✅ PASS (with known gap)

**Evidence**:
- Manual smoke tests documented for FATX, STFS, GPD flows (quickstart.md, tasks T026)
- CTest infrastructure available (no test suites currently registered)
- Constitution acknowledges "tree currently ships without automated coverage"
- Test naming convention specified: `ComponentScenario_ExpectedBehavior`
- Fixtures location documented: `XboxInternals/*/fixtures/`

**Assessment**: No violations. Current manual testing acceptable per Constitution IV; automated coverage deferred to future work.

---

### V. Security, Dependencies, and Release Hygiene ✅ PASS

**Evidence**:
- Botan submodule present; CMake can fetch if absent
- No `botan_all.cpp` committed to git (only in build artifacts)
- Qt validation documented (`qmake6 --version` for releases)
- Cryptographic features verified functional at runtime (mainwindow.cpp, certificates, signatures)
- Backup-or-abort policy enforces safety (FR-017: backup failure aborts write operations)

**Assessment**: No violations. Security and dependency management follow constitution guidelines.

---

### Additional Constraints & Standards ✅ PASS

**Evidence**:
- **Platform and Tooling**: Windows primary; MinGW 13.1.0; Qt 6.7.3+ baseline
- **Performance & UX Discipline**: Progress dialogs for long operations (multiprogressdialog), cancelable workflows, 0.5s feedback threshold
- **Source Organization**: Shared functionality in XboxInternals/ with clear headers; UI helpers in Velocity/
- **Documentation**: README.md, BUILD.md, AGENTS.md updated; commit messages follow style (present tense, <=72 chars)

**Assessment**: No violations. Additional standards met.

---

## Constitution Compliance Summary

| Principle | Status | Evidence |
|-----------|--------|----------|
| I. Module Boundaries | ✅ PASS | Platform abstraction complete (T004-T012), XboxInternals Qt-independent |
| II. Reproducible Builds | ✅ PASS | CMake presets operational, compile_commands.json exported |
| III. C++20 + Qt Idioms | ✅ PASS | Consistent style, Qt containers, qthelpers.cpp wrappers |
| III-A. Platform Abstraction | ✅ PASS | C++20 std lib migration complete, PIMPL for unavoidable platform code |
| IV. Testing Coverage | ✅ PASS | Manual smoke tests documented, CTest available for future suites |
| V. Security & Dependencies | ✅ PASS | Botan managed correctly, backup-or-abort policy enforced |
| Additional Standards | ✅ PASS | Documentation current, performance targets defined |

**Overall Result**: ✅ ALL GATES PASS — No constitution violations; ready for implementation.

## Project Structure

### Documentation (this feature)

```
specs/[###-feature]/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

```
Velocity/                           # Qt 6 desktop GUI application
├── main.cpp                        # Application entry point
├── mainwindow.{cpp,h,ui}          # MDI parent window, menu bar, plugin system
├── packageviewer.{cpp,h,ui}       # STFS package browser (file tree, metadata)
├── deviceviewer.{cpp,h,ui}        # FATX device/partition viewer
├── profileeditor.{cpp,h,ui}       # GPD/profile comprehensive editor
├── qthelpers.{cpp,h}              # Qt utilities (file I/O wrappers, encoding detection)
│
├── Dialogs (specialized viewers)
│   ├── xmldialog.{cpp,h,ui}       # XML viewer with BOM detection
│   ├── textdialog.{cpp,h,ui}      # Text file viewer (.txt, .ini, .cfg, .json, .log)
│   ├── zipviewer.{cpp,h,ui}       # ZIP archive viewer (QZipReader, extract)
│   ├── imagedialog.{cpp,h,ui}     # Image viewer (PNG, JPG, BMP)
│   ├── xdbfdialog.{cpp,h,ui}      # GPD/XDBF viewer
│   ├── ytgrdialog.{cpp,h,ui}      # YTGR avatar asset viewer
│   ├── strbdialog.{cpp,h,ui}      # STRB avatar asset viewer
│   ├── certificatedialog.{cpp,h,ui}            # X.509 certificate viewer
│   ├── licensingdatadialog.{cpp,h,ui}          # License info display
│   ├── securitysectordialog.{cpp,h,ui}         # Security data inspector
│   ├── stfsvolumedescriptordialog.{cpp,h,ui}   # STFS volume descriptor
│   ├── svodvolumedescriptordialog.{cpp,h,ui}   # SVOD volume descriptor
│   └── transferflagsdialog.{cpp,h,ui}          # Transfer flags editor
│
├── Wizards (creation tools)
│   ├── creationwizard.{cpp,h,ui}              # STFS package creator
│   ├── profilecreatorwizard.{cpp,h,ui}        # Profile package creator
│   ├── themecreationwizard.{cpp,h,ui}         # Theme package creator
│   ├── achievementcreationwizard.{cpp,h,ui}   # Achievement creator
│   └── gamerpicturepackdialog.{cpp,h,ui}      # Gamer picture pack creator
│
├── Tools (profile/device utilities)
│   ├── gameadderdialog.{cpp,h,ui}       # Add games to profiles
│   ├── profilecleanerwizard.{cpp,h,ui}  # Remove profile data categories
│   ├── fatxpathgendialog.{cpp,h,ui}     # FATX path generator
│   ├── clustertooldialog.{cpp,h,ui}     # Low-level FATX cluster ops
│   ├── titleidfinderdialog.{cpp,h,ui}   # Title ID lookup
│   ├── addressconverterdialog.{cpp,h,ui}# Xbox 360 address converter
│   └── preferencesdialog.{cpp,h,ui}     # Application settings
│
├── Progress/Feedback
│   ├── multiprogressdialog.{cpp,h,ui}   # Multi-file operation progress
│   ├── singleprogressdialog.{cpp,h,ui}  # Single operation progress
│   └── about.{cpp,h,ui}                 # About dialog
│
├── Downloaders (network-dependent, partially broken - deferred to FE-002)
│   ├── gpddownloader.{cpp,h}            # GPD file downloader (GitHub)
│   └── avatarassetdownloader.{cpp,h}    # Avatar asset downloader
│
├── Plugin System
│   └── PluginInterfaces/
│       ├── igamemodder.h                # Game save modder interface
│       └── igpdmodder.h                 # Profile modder interface
│
├── Resources
│   ├── Resources.qrc                    # Qt resource file (icons, images)
│   ├── velocity.rc                      # Windows resource file
│   ├── velocity.icns                    # macOS icon
│   └── Images/, Binaries/               # Assets and embedded binaries
│
└── CMakeLists.txt                       # Qt GUI build configuration

XboxInternals/                    # Pure C++20 library (NO Qt dependency)
├── TypeDefinitions.h             # Platform-agnostic type definitions
├── XboxInternals_global.h        # Library export macros
├── winnames.h                    # Windows name conflict mitigation
│
├── Fatx/                         # FATX filesystem (Xbox 360 storage)
│   ├── FatxDrive.{cpp,h}         # FATX volume abstraction
│   ├── FatxIO.{cpp,h}            # File I/O operations
│   ├── FatxFileEntry.{cpp,h}     # File/directory entries
│   ├── FatxHelpers.{cpp,h}       # Path utilities, helpers
│   └── FatxDriveDetection.{cpp,h}# Device detection (PIMPL for platform code)
│
├── Stfs/                         # STFS package format
│   ├── StfsPackage.{cpp,h}       # STFS package reader/writer
│   ├── StfsMetadata.{cpp,h}      # Package metadata (title, content ID)
│   ├── StfsDefinitions.{cpp,h}   # Constants, enums, structures
│   ├── SvodMultiFileIO.{cpp,h}   # SVOD multi-file handling (std::filesystem)
│   └── XContentHeader.{cpp,h}    # Xbox content header parsing
│
├── Gpd/                          # GPD/XDBF profile files
│   ├── GpdBase.{cpp,h}           # Base GPD structure
│   ├── DashboardGpd.{cpp,h}      # Dashboard profile GPD
│   ├── GameGpd.{cpp,h}           # Game-specific GPD
│   ├── XdbfHelpers.{cpp,h}       # XDBF utilities
│   └── Xdbf.{cpp,h}              # XDBF container format
│
├── Disc/                         # Xbox 360 disc formats
│   ├── Svod.{cpp,h}              # SVOD disc image
│   ├── Gdfx.{cpp,h}              # GDFX disc image
│   └── GdfxHelpers.{cpp,h}       # Disc utilities
│
├── AvatarAsset/                  # Avatar asset formats
│   ├── AssetHelpers.{cpp,h}      # Avatar asset utilities
│   ├── AssetSubcategory.h        # Asset taxonomy
│   └── AvatarAsset.{cpp,h}       # Avatar asset container
│
├── Account/                      # Account/profile structures
│   ├── Account.{cpp,h}           # Xbox 360 account data
│   └── ProfileCreationWizard.h   # Profile creation metadata
│
├── Cryptography/                 # Botan wrappers (Xbox crypto)
│   ├── Xbox360Crypto.{cpp,h}     # Signature verification, certificates
│   └── Rc4.{cpp,h}               # RC4 implementation (Botan wrapper)
│
├── IO/                           # Cross-platform I/O abstractions
│   ├── BaseIO.{cpp,h}            # Abstract I/O interface
│   ├── DeviceIO.{cpp,h}          # Raw device I/O (PIMPL: DeviceIO_win.cpp)
│   ├── FileIO.{cpp,h}            # File-based I/O (std::fstream)
│   └── MemoryIO.{cpp,h}          # In-memory I/O
│
└── CMakeLists.txt                # Library build configuration

cmake/                            # Build system helpers
├── FindBotan.cmake               # Botan library detection
└── botan_modules.cmake           # Botan module configuration

specs/001-baseline-capabilities/  # Feature documentation (this branch)
├── spec.md                       # Requirements, user stories, FRs
├── plan.md                       # This file (implementation plan)
├── research.md                   # Phase 0: Research decisions
├── data-model.md                 # Phase 1: Entity model
├── quickstart.md                 # Phase 1: Validation guide
├── contracts/                    # Phase 1: API contracts
└── tasks.md                      # Phase 2: Implementation roadmap (78 tasks)

out/build/<preset>/               # Generated build artifacts (not committed)
├── Velocity.exe                  # Windows executable
├── XboxInternals.dll/.a          # Shared/static library
└── botan artifacts               # Botan crypto library (runtime required)
```

**Structure Decision**: CMake superproject with strict module separation. **Velocity/** is the Qt 6 GUI client (140+ source files: dialogs, wizards, tools, viewers, progress). **XboxInternals/** is the pure C++20 library (NO Qt) providing Xbox format support (FATX, STFS, GPD, XDBF, SVOD, GDFX, cryptography via Botan). Platform abstraction complete: C++20 `std::filesystem` replaces Win32 APIs; unavoidable platform code (DeviceIO) uses PIMPL pattern (`DeviceIO_win.cpp` hidden from public headers). Build artifacts in `out/build/<preset>/` never committed. This structure enables XboxInternals reuse for CLI tools, servers, or non-GUI contexts while keeping GUI complexity in Velocity/.

## Complexity Tracking

*No violations detected — this section is not applicable.*

All constitution checks passed. No complexity justifications needed.

---

## Phase Status

### ✅ Phase 0: Research Complete

**Artifact**: [research.md](./research.md) (91 lines)

**Key Decisions Documented**:
- Export formats (CSV + JSON for achievements)
- Sample data curation strategy
- Platform abstraction approach (C++20 std lib → PIMPL, three-tier preference order)
- Backup storage and retention policy (user-configurable location, no automatic retention)
- Backup failure handling (abort-or-backup, safety-first)
- Plugin security model (in-process with consent warning)
- Write operation performance (0.5s feedback, cancelable, >30s warning)

**Status**: All NEEDS CLARIFICATION items from Technical Context have been resolved.

---

### ✅ Phase 1: Design & Contracts Complete

**Artifacts**: 
- [data-model.md](./data-model.md) (594 lines) — Entities (FATX Source, STFS Package, GPD Profile, Disc Image, etc.)
- [quickstart.md](./quickstart.md) (725 lines) — Validation procedures for all 9 user stories
- [contracts/](./contracts/) — API contract specifications (if applicable)

**Key Entities**:
- **FATX Source**: partitions[], fileEntries[], size, path, mountInfo
- **STFS Package**: metadata (titleId, displayName, contentId), fileEntries[], validation status
- **GPD Profile Data**: profileId, gamerTag, achievements[] (achievementId, titleId, name, unlocked, gamerscore)
- **Disc Image**: structure, fileEntries[], size, path (SVOD/GDFX)
- **Extraction Job**: source, destination, selectedFiles[], progress, cancelToken
- **Backup Record**: originalPath, backupPath, timestamp, operation

**Status**: Data model extracted from requirements; quickstart validation guide complete for all user stories.

---

### ⏳ Phase 2: Tasks Generation (Next Step)

**Command**: Run `/speckit.tasks` (or `speckit.tasks.prompt.md`) to generate tasks.md

**Current Status**: 
- [tasks.md](./tasks.md) already exists with 78 tasks (T001-T078)
- Tasks validated: proper format (checkboxes, sequential IDs, [P] parallel markers, [US#] labels)
- 9 tasks complete (T004-T012: Platform abstraction)
- 69 tasks remaining across 12 phases
- Ready for implementation execution

**Note**: tasks.md creation is handled by separate `/speckit.tasks` command per speckit.plan.prompt.md instructions. This plan.md represents completion of `/speckit.plan` workflow.

---

## GUI Modernization (2025-10-20 to 2025-10-21)

**Recent Implementation Session**:

During specification development, GUI enhancements were implemented alongside documentation work:

1. **XML File Viewer** (`xmldialog.{h,cpp,ui}`): BOM-aware encoding detection (UTF-8, UTF-16 LE/BE), monospace display, 800x600 dialog
2. **Text File Viewer** (`textdialog.{h,cpp,ui}`): Generic viewer for .txt, .ini, .cfg, .json, .log, .toc with same encoding cascade
3. **ZIP Archive Viewer** (`zipviewer.{h,cpp,ui}`): Qt 6 QZipReader API, numeric size sorting, extract selected/all functionality
4. **Image Viewer Enhancements** (`imagedialog.{h,cpp}`): Filename in title bar, resizable windows (was fixed-size)
5. **Encoding Detection** (`packageviewer.cpp`): Replaced deprecated QTextCodec with Qt 6 QStringDecoder, BOM-aware cascade
6. **File Type Detection** (`qthelpers.cpp`): Case-insensitive extension matching (.toLower()), expanded format support
7. **Spelling Fixes**: "Verison" → "Version", "blockSeperation" → "blockSeparation", "begining" → "beginning", "calcualte" → "calculate" (15+ files)

**Package Viewer Now Supports 9 File Types**:
1. Images (.png, .jpg, .jpeg, .bmp) — Enhanced
2. XML files (.xml) — NEW
3. Text files (.txt, .ini, .cfg, .json, .log, .toc) — NEW
4. ZIP archives (.zip) — NEW
5. GPD/Xdbf files (magic 0x58444246) — Existing
6. STRB avatar assets (magic 0x53545242) — Existing
7. PEC embedded packages — Existing
8. STFS nested packages — Existing
9. Unknown files (extract only) — Existing

**Git Commit**: b7f9259 (197 files changed, 4363 insertions, 82 deletions)

---

## Implementation Priorities

Per tasks.md and user story priorities:

**Phase 1 (Setup)**: T001-T003 — Verify build environment (all parallelizable)

**Phase 2 (Foundational)**: T004-T018  
- ✅ T004-T012: Platform abstraction COMPLETE  
- ⏳ T013-T018: UI foundation work (progress dialogs, validation indicators, error messages, logs/support bundle)

**Phase 3 (US1 — Priority P1)**: T019-T026, T077-T078 — Browse and extract Xbox content

**Phases 4-11 (US2-US9)**: T027-T067 — Profile inspection (P2), safe edits (P3), creation (P2), management (P2), device tools (P2), plugins (P3), specialized formats (P3), utilities (P4)

**Phase N (Polish)**: T068-T076 — Accessibility, documentation, obsolete code removal, Botan verification

**Parallel Execution Opportunities**: 43 tasks marked with [P] for concurrent implementation

