# Feature Specification: Velocity Baseline Capabilities

**Feature Branch**: `001-baseline-capabilities`  
**Created**: 2025-10-19  
**Status**: Draft - Expanded to include all existing functionality  
**Input**: User description: "Complete specification of Velocity's current functionality and audience. Velocity is a desktop application for browsing, editing, and creating Xbox 360 files and storage, supporting Xbox-specific cryptography, XDBF (GPD), YTGR, STRB (Avatar Assets), FATX, STFS, and Xbox disc formats (SVOD, GDFX). Primary users are game modders, digital archivists, and power users who need to inspect, extract, edit, and create content from storage devices, profiles, achievements, themes, gamer pictures, and content packages with responsive UI, progress feedback, and safety mechanisms (backups, validation). Includes creation wizards, profile management tools, device/FATX utilities, plugin system, and comprehensive editing capabilities."

## Clarifications

### Session 2025-10-19

- Q: Diagnostics and user-accessible logs → A: In-app "View Logs" dialog with search and a "Save Support Bundle" (zip) export.
- Q: Accessibility scope for the baseline → A: Basic keyboard navigation and focus order for core dialogs and lists; readable contrast in default theme.
- Q: Extraction conflict policy → A: On first filename conflict, prompt with Overwrite / Skip / Rename and "Apply to all" checkbox.
- Q: Sample file sourcing for smoke tests → A: Document where users can obtain their own samples (e.g., legally owned device backups) and provide checksum/format references.
- Q: Cryptographic validation detail level → A: Show specific failure reasons in the UI (e.g., "signature mismatch", "missing certificate") alongside the indicator.
- Q: When automatic backup creation fails (disk full, permission denied), what should happen to the pending write operation? → A: Abort write operation immediately; show error; require user to resolve backup issue (disk space, permissions) before retrying.
- Q: Where should automatic backups be stored and how should retention be managed? → A: User-configurable backup location set in Preferences; defaults to Dedicated Backups/ folder in Velocity app data directory.
- Q: What level of isolation/sandboxing should the plugin system provide for security and stability? → A: Plugins run in same process with full memory access; show prominent warning on first plugin load that plugins can crash Velocity and access sensitive data; require explicit user consent.
- Q: What performance targets should apply to write operations (edit, save, create) on typical files? → A: No hard time limits; progress must appear within 0.5 seconds; operations remain cancelable; warn if >30 seconds elapsed.

## Implementation Notes

### Session 2025-10-20 to 2025-10-21 - Package Viewer GUI Modernization

**Code Quality Improvements:**
- Fixed spelling errors throughout codebase:
  - "Verison" → "Version" in metadata.cpp (2 instances)
  - "blockSeperation" → "blockSeparation" in StfsDefinitions.h/cpp, StfsPackage.cpp (20+ instances)
  - "begining" → "beginning" in multiple files (9 instances)
  - "calcualte" → "calculate" in MultiFileIO.h/cpp, FatxIO.cpp (4 instances)
- Fixed missing CMakeLists.txt entry for svodvolumedescriptordialog.cpp

**Package Viewer File Type Support - NEW IMPLEMENTATIONS:**

1. **XML File Viewer** (`xmldialog.h/.cpp/.ui`):
   - Read-only QTextEdit with monospace font
   - Shows filename in title bar ("XML Viewer - filename.xml")
   - 800x600 default window size
   - BOM (Byte Order Mark) detection for proper encoding:
     - UTF-8 with BOM (0xEF 0xBB 0xBF)
     - UTF-16 LE with BOM (0xFF 0xFE)
     - UTF-16 BE with BOM (0xFE 0xFF)
     - Falls back to UTF-8 → Latin1 if no BOM
   - Handles Xbox 360/Windows encoding standards correctly

2. **Text File Viewer** (`textdialog.h/.cpp/.ui`):
   - Generic text viewer for configuration and log files
   - Supports: .txt, .ini, .cfg, .json, .log, .toc extensions
   - Same encoding detection as XML viewer
   - Identical UI pattern for consistency

3. **ZIP Archive Viewer** (`zipviewer.h/.cpp/.ui`):
   - Uses Qt 6.8's private QZipReader API (zero external dependencies)
   - Table view showing: file path, size (human-readable), compression status
   - Custom `NumericSizeItem` class for proper numeric sorting (fixes "94 KB > 938 KB" bug)
   - Extract selected files or extract all functionality
   - File size display: bytes, KB (>1KB), MB (>1MB)
   - ZIP magic number detection: 0x504B0304, 0x504B0506, 0x504B0708
   - Added Qt6Core private include paths to CMakeLists.txt

**Image Viewer Enhancements** (`imagedialog.h/.cpp`):
   - Added optional `fileName` parameter to constructor
   - Window title shows "Image Viewer - filename.ext"
   - Changed from `setFixedSize()` to `resize()` for resizable windows
   - Updated all callers: packageviewer.cpp, xdbfdialog.cpp, svoddialog.cpp

**File Type Detection Improvements** (`qthelpers.cpp`):
   - Added case-insensitive extension detection (.toLower())
   - Fixes: "DUKE3D.CFG" now detected correctly alongside "autoexec.cfg"
   - Added detection for .xml, .txt, .ini, .cfg, .json, .log, .toc, .zip
   - ZIP magic number detection added to binary signature checks

**Encoding Support** (`packageviewer.h/.cpp`):
   - Replaced deprecated QTextCodec with Qt 6's QStringDecoder
   - BOM-aware encoding detection cascade:
     1. Check for UTF-8/UTF-16 LE/BE BOM markers
     2. Skip BOM bytes if present
     3. Fall back to UTF-8 without BOM
     4. Fall back to Latin1 if replacement characters detected
   - Detects encoding failures by checking for QChar::ReplacementCharacter
   - Prioritizes Windows/Xbox 360 encoding standards

**Files Created:**
- `Velocity/xmldialog.h` (27 lines)
- `Velocity/xmldialog.cpp` (32 lines)
- `Velocity/xmldialog.ui` (XML UI definition)
- `Velocity/textdialog.h` (27 lines)
- `Velocity/textdialog.cpp` (32 lines)
- `Velocity/textdialog.ui` (XML UI definition)
- `Velocity/zipviewer.h` (44 lines with NumericSizeItem class)
- `Velocity/zipviewer.cpp` (145 lines)
- `Velocity/zipviewer.ui` (XML UI definition)

**Files Modified:**
- `Velocity/CMakeLists.txt` (added new sources, UI files, Qt private includes)
- `Velocity/packageviewer.h` (added includes for new dialogs)
- `Velocity/packageviewer.cpp` (added handlers for XML, Text, ZIP; encoding detection)
- `Velocity/imagedialog.h` (added fileName parameter)
- `Velocity/imagedialog.cpp` (show filename in title, resizable)
- `Velocity/qthelpers.cpp` (case-insensitive extensions, new file type detection)
- Multiple spelling fixes across 15+ files

**Package Viewer Now Supports 9 File Types:**
1. ✅ Images (.png, .jpg, .jpeg, .bmp) - Enhanced with filename display
2. ✅ XML files (.xml) - NEW with BOM detection
3. ✅ Text files (.txt, .ini, .cfg, .json, .log, .toc) - NEW
4. ✅ ZIP archives (.zip) - NEW with numeric sorting
5. ✅ GPD/Xdbf files (0x58444246 magic) - Existing
6. ✅ STRB avatar assets (0x53545242 magic) - Existing
7. ✅ PEC embedded packages - Existing
8. ✅ STFS nested packages - Existing
9. ✅ Unknown files (can extract) - Existing

**Technical Implementation Notes:**
- All new viewers follow consistent dialog pattern (800x600, filename in title, WA_DeleteOnClose)
- Zero new external dependencies (uses Qt 6.8 built-in features)
- BOM detection ensures correct display of Xbox 360/Windows files
- Custom sorting implementation for numeric data in tables
- Case-insensitive file detection improves cross-platform compatibility

**Build System:**
- Successfully builds with MinGW 13.1.0 + Qt 6.8.0
- C++20 standard compliance maintained
- All changes integrate with existing XboxInternals library



## User Scenarios & Testing *(mandatory)*

### User Story 1 - Browse and extract Xbox content (Priority: P1)

As a power user, I can open an Xbox storage source (device image, content package, or disc image) and browse its contents in a safe, read-only mode, so I can quickly locate and extract files without risking accidental changes.

**Why this priority**: Browsing and safe extraction are the core purposes for archivists and modders and are the most common workflows.

**Independent Test**: Using public sample images/packages, open each supported format and extract a chosen file/folder to a local directory; verify no writes occur to the source and the UI remains responsive with progress feedback.

**Acceptance Scenarios**:

1. Given a FATX device image, when I open it, then I see partitions and a file tree and can extract selected items with progress and cancel.
2. Given an STFS package (e.g., save/content), when I open it, then I see metadata and its file tree and can extract files to disk.
3. Given a disc image (SVOD or GDFX), when I open it, then I can browse its structure and extract selected files without modifying the image.

---

### User Story 2 - Inspect profiles and achievements (Priority: P2)

As an archivist, I can open profile/GPD (XDBF) files to review account/profile metadata and achievements, enabling documentation and non-destructive analysis.

**Why this priority**: Profiles and achievements are high-value data for preservation and modding; read-oriented review supports many use cases.

**Independent Test**: Open representative GPD files and confirm key fields and achievements are viewable and can be exported in a common format for analysis.

**Acceptance Scenarios**:

1. Given a GPD file, when I open it, then I can view profile identifiers and a list of achievements with basic details.
2. Given the list of achievements, when I choose export, then both CSV and JSON exports are available with the visible fields, including at minimum: titleId, achievementId, name, description, unlocked (true/false), timestamp (if present), and gamerscore.

---

### User Story 3 - Perform safe edits with explicit confirmation (Priority: P3)

As an experienced user, I can make edits (metadata changes, profile modifications, file operations) with automatic backup protection and explicit confirmation, so that I can modify content without risking data loss. If backup creation fails, the application prevents the write operation to ensure I never lose the original file.

**Why this priority**: Editing capabilities are essential for modders and power users; mandatory backup-or-abort policy protects against data loss while enabling modification workflows.

**Independent Test**: Attempt an edit on a test copy, confirm a backup is created automatically before changes, verify changes are applied with validation, and test that backup failure prevents the write operation entirely.

**Acceptance Scenarios**:

1. Given an STFS package, when I edit metadata, then the application creates a backup automatically before applying changes; if backup fails, the edit is aborted with clear guidance.
2. Given a GPD file in the profile editor, when I modify achievements or profile data, then changes are validated and backed up before saving; backup failure prevents any write.
3. Given a FATX device image, when I perform file operations, then the application creates backups before write operations; users can restore from backups via Tools menu.

---

### User Story 4 - Create Xbox 360 content from scratch (Priority: P2)

As a modder or content creator, I can create new Xbox 360 packages, profiles, themes, and gamer picture packs using guided wizards, so that I can produce custom content for personal use or distribution.

**Why this priority**: Creation tools enable modding community and custom content generation, core value proposition for power users.

**Independent Test**: Use each creation wizard to generate a valid package/profile/theme/gamer picture pack and verify it opens correctly in Xbox 360 or Velocity.

**Acceptance Scenarios**:

1. Given the Package Creator wizard, when I select files and configure metadata, then a valid STFS package is created and can be opened.
2. Given the Profile Creator wizard, when I set gamertag and avatar options, then a valid profile package with GPD files is created.
3. Given the Theme Creator wizard, when I configure colors and upload assets, then a valid theme package is created.
4. Given the Gamer Picture Pack Creator, when I add images, then a valid gamer picture package is created.

---

### User Story 5 - Manage and modify profiles (Priority: P2)

As a user managing Xbox 360 profiles, I can use specialized tools to add games, clean unwanted data, and edit profile details comprehensively, enabling profile maintenance and customization.

**Why this priority**: Profile management is a frequent workflow for modders and archivists; specialized tools streamline complex operations.

**Independent Test**: Open a profile, use Game Adder to add a title, use Profile Cleaner to remove data, verify profile remains valid.

**Acceptance Scenarios**:

1. Given the Game Adder tool, when I search for a title and add it to a profile, then the game appears in the profile with proper metadata and zero achievements.
2. Given the Profile Cleaner wizard, when I select data categories to remove, then the selected data is deleted safely with backup.
3. Given the Profile Editor, when I open a profile package, then I can view and edit comprehensive profile data including achievements, games, avatar, settings, and titles.

---

### User Story 6 - Work with device images and raw FATX volumes (Priority: P2)

As an advanced user working with device images or raw Xbox 360 storage, I can use the Device Viewer to browse partitions and files, and use FATX utilities to generate file paths and perform cluster-level operations.

**Why this priority**: Device-level access is essential for archiving physical hardware and forensic analysis.

**Independent Test**: Open a device image, browse partitions, extract files, use FATX path tool to generate paths.

**Acceptance Scenarios**:

1. Given the Device Viewer, when I open a raw device or image, then I can see all partitions (System, Content, etc.) and browse their FATX file trees.
2. Given the FATX File Path Tool, when I provide file metadata, then I can generate the correct FATX path for locating files on device.
3. Given cluster-level tools, when I inspect or modify clusters, then I can perform low-level FATX operations with warnings.

---

### User Story 7 - Extend functionality with plugins (Priority: P3)

As a developer or advanced user, I can load game-specific or profile-specific modder plugins to extend Velocity's capabilities for specific titles or custom workflows.

**Why this priority**: Plugin system enables community extensions and game-specific tooling without core codebase changes.

**Independent Test**: Install a plugin, verify it appears in the Plugins menu, use it to modify a game save or profile.

**Acceptance Scenarios**:

1. Given a game modder plugin, when I load it, then it appears under Plugins → Game Modders and can operate on compatible save files.
2. Given a profile modder plugin, when I load it, then it appears under Plugins → Profile Modders and can operate on profile packages.
3. Given the plugin system, when a plugin completes, then Velocity returns control and reflects any changes made by the plugin.

---

### User Story 8 - Inspect and work with specialized Xbox formats (Priority: P3)

As a user handling specialized Xbox 360 file formats, I can open and work with YTGR (avatar assets), STRB (avatar assets), SVOD disc images, and view/edit format-specific properties like certificates, security sectors, volume descriptors, licensing data, and transfer flags.

**Why this priority**: Specialized formats and properties are needed for advanced modding, preservation, and debugging.

**Independent Test**: Open each specialized format, view format-specific dialogs, verify data is correctly displayed.

**Acceptance Scenarios**:

1. Given a YTGR file, when I open it, then I can view avatar asset metadata and structure.
2. Given an STRB file, when I open it, then I can view and potentially extract avatar asset data.
3. Given an STFS package, when I view its certificate, then I can see certificate details, issuer, and validity.
4. Given an STFS package, when I view licensing data, then I can see license type, device IDs, and restrictions.
5. Given an SVOD system, when I open it, then I can browse the disc structure and view volume descriptors and file info.

---

### User Story 9 - Discover and organize content with utilities (Priority: P4)

As a user working with Xbox 360 content, I can use utility tools like Title ID Finder to look up game information, address converters for technical operations, and preferences to customize Velocity's behavior.

**Why this priority**: Utilities streamline workflows and improve discoverability; lower priority as they support rather than enable core tasks.

**Independent Test**: Use Title ID Finder to search for a game, adjust preferences, verify settings persist.

**Acceptance Scenarios**:

1. Given the Title ID Finder, when I search for a game name or title ID, then I can view matching titles with IDs and metadata.
2. Given the Address Converter dialog, when I input addresses or offsets, then I can convert between different Xbox 360 addressing schemes.
3. Given Preferences, when I adjust settings (file paths, UI options), then settings persist across sessions.

---

### Edge Cases

- Corrupted or partially downloaded images/packages should open in a guarded mode with clear warnings; operations that rely on missing data are disabled; editing operations are blocked.
- Very large images/packages should stream or page content to keep the UI responsive; long-running operations expose progress and cancel; creation wizards should validate size limits before processing.
- Missing cryptographic material should not block read-only browsing; validation indicators display "unknown/unverified" with guidance; editing operations should warn about invalidated signatures.
- Unsupported/rare variants of formats should fail gracefully with actionable messages and links to logs or documentation; plugins should handle unknown formats without crashing.
- File system permission issues on extraction/write destinations should be handled with clear prompts to choose a different location or elevate privileges where appropriate.
- Filename conflicts during extraction should prompt the user on first occurrence with Overwrite / Skip / Rename options and an "Apply to all" checkbox for batch behavior.
- Profile editing operations that would create invalid states (e.g., duplicate achievement IDs, invalid title IDs) should be blocked with clear validation messages.
- Plugin load failures should be logged and reported without preventing other plugins or core functionality from loading.
- Plugin execution errors (crashes, exceptions) should be caught where possible; the main application should remain stable and provide error details to the user. Uncaught plugin exceptions may still crash Velocity since plugins run in-process.
- Device write operations should verify FATX integrity before and after writes; failed writes should restore from backup automatically.
- If backup creation fails before a write operation (disk full, permissions), the operation must abort immediately with clear guidance on resolving the backup issue (free disk space, adjust permissions, select different backup location).
- Theme/gamer picture creation with invalid image formats or sizes should reject inputs early with clear guidance on requirements.
- Profile creation with invalid gamertags should validate character limits and allowed characters before allowing submission.
- Update checks should fail gracefully when network is unavailable and not block application startup.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The application MUST open and browse FATX device images and display partitions and file trees in read-only mode by default.
- **FR-002**: The application MUST open STFS content packages, display primary metadata, and present their internal file trees.
- **FR-003**: The application MUST open GPD/XDBF files and display profile and achievement information suitable for review.
- **FR-004**: The application MUST open Xbox disc images (SVOD, GDFX) for browsing and selection.
- **FR-005**: Users MUST be able to extract selected files/folders from any opened source to a local directory, with visible progress and the ability to cancel. On filename conflict, the application MUST prompt with Overwrite / Skip / Rename and an "Apply to all" checkbox.
- **FR-006**: When cryptographic validation is possible, the application SHOULD indicate verification status (verified / failed / unknown) with specific reasons displayed in the UI (e.g., "signature mismatch", "missing certificate"); when materials are missing, it MUST show a non-blocking warning with guidance.
- **FR-007**: Long-running operations (open, enumerate, extract, edit, save, create) MUST keep the UI responsive. Progress indicators MUST appear within 0.5 seconds of operation start. Progress updates MUST occur at least every 0.5 seconds thereafter with current status (percentage, file count, or estimated time remaining). Operations MUST remain cancelable at all times. If an operation exceeds 30 seconds, the application SHOULD display a warning or status update to inform the user of continued processing.
- **FR-008**: Errors MUST be presented in clear, user-understandable language with guidance to resolve or collect diagnostic information.
- **FR-009**: Preferences MUST persist basic UX settings such as last-opened directories and window state.
- **FR-010**: Exports for profile/achievement views MUST support both CSV and JSON. Minimum fields: titleId, achievementId, name, description, unlocked (true/false), timestamp (if present), and gamerscore; additional visible fields MAY be included when present.
- **FR-011**: Observability & Diagnostics: Provide an in-app "View Logs" dialog accessible from the Help menu with basic search/filter and a "Save Support Bundle" export that collects recent logs into a single file for sharing. Error flows SHOULD guide users to open the logs dialog or export a bundle when reporting issues.
- **FR-012**: Accessibility Baseline: Core dialogs, tables/lists, and navigation MUST support keyboard traversal with a predictable focus order; default theme text/controls MUST meet readable contrast (at least WCAG AA for text in default theme).
- **FR-013**: The application MUST open YTGR (avatar asset) files and display their structure and metadata. (US8)
- **FR-014**: The application MUST open STRB (avatar asset) files and display asset data. (US8)
- **FR-015**: The Profile Editor MUST allow comprehensive viewing and editing of profile packages including achievements, games played, avatar data, and settings. (US2, US5)
- **FR-016**: The Game Adder tool MUST allow users to search for Xbox 360 titles and add them to profiles with proper metadata initialization. (US5)
- **FR-017**: The Profile Cleaner wizard MUST provide options to remove specific data categories from profiles with backup. (US5)
- **FR-018**: All write operations MUST attempt automatic backup creation before modifying original files. Backup MUST complete successfully before any write occurs. If backup creation fails (disk full, permission denied, invalid backup path), the write operation MUST abort immediately with a clear error message requiring the user to resolve the backup issue (free disk space, check permissions, verify backup location in Preferences) before retrying. Validation MUST occur before backup attempt and after write completion. (US3)
- **FR-019**: Users MUST receive clear confirmation prompts before any destructive operation with details about what will be changed. (US3)
- **FR-020**: The Package Creator wizard MUST allow users to create new STFS packages by selecting files and configuring metadata. (US4)
- **FR-021**: The Profile Creator wizard MUST allow users to create new Xbox 360 profiles by specifying gamertag and avatar options. (US4)
- **FR-022**: The Theme Creator wizard MUST allow users to create theme packages by uploading assets and selecting colors. (US4)
- **FR-023**: The Gamer Picture Pack Creator MUST allow users to compile multiple images into a valid package. (US4)
- **FR-024**: The Achievement Creation feature MUST allow users to define custom achievements with full metadata and thumbnails. (US4)
- **FR-025**: The Device Viewer MUST open raw device files and display all FATX partitions with browsable file trees. (US6)
- **FR-026**: The FATX File Path Tool MUST generate correct FATX file paths from file metadata. (US6)
- **FR-027**: Device operations MUST support reading from and writing to FATX volumes with warnings and backups. (US6)
- **FR-028**: The Certificate Dialog MUST display STFS package certificate details and validation status. (US8)
- **FR-029**: The Licensing Data Dialog MUST display license information including type, device IDs, and restrictions. (US8)
- **FR-030**: The Security Sector Dialog MUST display security sector data for disc images. (US8)
- **FR-031**: The Volume Descriptor dialogs MUST display STFS and SVOD volume descriptor details. (US8)
- **FR-032**: The Transfer Flags Dialog MUST display and allow editing of STFS transfer flags. (US8)
- **FR-033**: The SVOD tool dialogs MUST provide SVOD-specific operations including extraction and metadata viewing. (US8)
- **FR-034**: The application MUST support loading external plugins via defined interfaces (IGameModder, IGPDModder). Plugins run in the same process with full memory access. (US7)
- **FR-035**: On first plugin load (per session or ever), the application MUST display a prominent security warning explaining that plugins can crash Velocity and access sensitive data, and require explicit user consent before loading any plugins. (US7)
- **FR-036**: Game modder plugins MUST appear in the Plugins → Game Modders menu with appropriate context. (US7)
- **FR-037**: Profile modder plugins MUST appear in the Plugins → Profile Modders menu with appropriate context. (US7)
- **FR-038**: The application MUST handle plugin failures gracefully without crashing the main application where possible; unhandled plugin exceptions should be caught, logged, and reported to the user with plugin name, error message, and option to disable the plugin. On plugin exception, display error dialog and log full stack trace for support. (US7)
- **FR-039**: Preferences MUST persist user settings including last-opened directories, window positions, UI customizations, and backup location across sessions. The backup location MUST be user-configurable (via Preferences dialog with path picker and validation) and default to a dedicated `Backups/` folder in the Velocity application data directory. An "Open Backup Folder" action MUST be available in the Tools menu. (US9)
- **FR-040**: The Title ID Finder MUST allow searching for Xbox 360 titles by name or ID with metadata display. (US9)
- **FR-041**: The Address Converter Dialog MUST provide conversion between different Xbox 360 addressing schemes. (US9)
- **FR-042**: The application MUST support update checks to notify users of new versions. (US9)
- **FR-043**: The application MUST use a multi-document interface (MDI) allowing multiple files open simultaneously with intuitive window management. (US1)
- **FR-044**: The application MUST support drag-and-drop of files onto the main window to open them. (US1)

### Key Entities *(include if feature involves data)*

- **FATX Source**: A storage device image with partitions and file entries; supports enumeration, extraction, and write operations.
- **STFS Package**: A content package with metadata and an internal file tree; supports viewing, extraction, editing, and creation. Includes certificate, licensing data, transfer flags, and volume descriptor.
- **GPD (XDBF) Profile Data**: Profile and achievement records viewable and editable in tabular form; supports export to CSV/JSON and comprehensive editing via Profile Editor.
- **Disc Image (SVOD/GDFX)**: Readable disc image structure for browsing and extraction; SVOD includes volume descriptors, security sectors, and file-level metadata.
- **YTGR File**: Avatar asset file containing asset metadata and structure.
- **STRB File**: Avatar asset file containing binary asset data.
- **Profile Package**: A complete Xbox 360 profile including GPD files, avatar data, settings, and game history. Can be created, edited, cleaned, and extended.
- **Achievement**: A game achievement with ID, title, description, gamerscore, unlock status, timestamp, and thumbnail image. Can be created and edited.
- **Theme Package**: An STFS package containing theme assets (background images, colors, metadata) for Xbox 360 dashboard customization.
- **Gamer Picture Pack**: An STFS package containing multiple gamer picture images for avatar customization.
- **Plugin**: An external module implementing IGameModder or IGPDModder interface to extend functionality for specific games or profiles.
- **Device/Partition**: A raw storage device or image with FATX partitions (System, Content, Cache) exposing low-level file system operations.
- **Extraction Job**: A user-initiated operation tracking selected items, destination, progress, cancel, and results with conflict resolution.
- **Validation Indicator**: A status reflecting whether integrity/cryptographic checks passed, failed, or are unknown. The indicator MUST display specific failure reasons (e.g., "signature mismatch", "missing certificate") in the UI to guide troubleshooting.
- **Certificate**: Cryptographic certificate embedded in STFS packages containing public key, signature, console ID, and validation data.
- **Transfer Flags**: STFS package flags controlling content transferability (move-only, device transfer, profile transfer).

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: A new user can open each supported source type (FATX, STFS, GPD, SVOD/GDFX, YTGR, STRB) and extract at least one file within 3 minutes without external instructions.
- **SC-002**: For 95% of operations (open, extract, edit, save, create) on representative samples, visible progress appears within 0.5 seconds and remains responsive; users can cancel at any time prior to completion.
- **SC-003**: 100% of edit and creation operations require explicit confirmation and create restorable backups before changes are applied (where applicable). Operations exceeding 30 seconds display status updates or warnings to keep users informed.
- **SC-004**: Error messages enable self-resolution for common failures (missing permissions, destination full, unsupported variant, invalid inputs), reducing follow-up support questions for these cases by 50% compared to baseline.
- **SC-005**: During long operations (open, extract, create, edit), the UI remains interactive (window can be moved, menus open) with no perceived freeze longer than 1 second.
- **SC-006**: Users can access logs within two clicks (Help → View Logs) and export a support bundle in under 10 seconds on representative systems.
- **SC-007**: Keyboard-only navigation completes primary flows (open source, browse tree, start extract, export achievements, create package, edit profile) without mouse; default theme achieves AA contrast for text.
- **SC-008**: Creation wizards (Package, Profile, Theme, Gamer Picture) produce valid outputs that open successfully in Xbox 360 consoles or Velocity itself on first attempt for well-formed inputs.
- **SC-009**: Profile tools (Game Adder, Profile Cleaner) complete operations on typical profiles (100-500 achievements) within 10 seconds with progress feedback.
- **SC-010**: Plugin loading completes within 5 seconds for typical plugin collections (<20 plugins); failed plugins are reported but do not block other plugins or core functionality.
- **SC-011**: Device Viewer opens and displays partition structure for typical device images (<500GB) within 30 seconds with browsable file trees.
- **SC-012**: Backup creation for all write operations completes automatically before modification; users can locate backups via clear naming convention (e.g., filename.bak with timestamp) in the configured backup location (default: Velocity app data `Backups/` folder).

### Assumptions

- Users provide their own device images, packages, and content files; cryptographic materials may be unavailable, but operations remain functional with clear verification status and warnings about invalidated signatures after edits.
- All operations (view, extract, edit, create) write to local disk only; network access is required only for optional features (update checks, Title ID Finder lookups if cloud-based).
- Very large sources (multi-GB device images, large packages) may take extended time for initial open and processing; progress and cancel controls are sufficient to keep the experience acceptable.
- Sample files for smoke testing are user-sourced (legally owned device backups, self-created content). Documentation SHOULD provide guidance on obtaining representative samples and list expected checksums/formats for validation.
- Plugins are third-party or community-developed; Velocity provides stable interfaces but does not guarantee plugin quality or compatibility. Plugins run in-process with full memory access and can crash Velocity or access sensitive data. Users must explicitly consent to these risks before loading plugins.
- Xbox 360 content formats and cryptography are well-documented via reverse engineering; Velocity operates on publicly known specifications and does not circumvent active DRM or encryption when prohibited.
- Users understand editing Xbox 360 content may violate terms of service or result in console bans; Velocity provides tools but users are responsible for usage and consequences.
- Backups are stored locally in a user-configurable location (defaulting to Velocity app data `Backups/` folder); users are responsible for backup management and retention policies (manual cleanup or external backup solutions).

### Technical Debt & Refactoring Notes

- **Platform Abstraction**: XboxInternals currently includes platform-specific API calls that should be abstracted behind interfaces using the PIMPL idiom (per constitution III-A). Baseline work SHOULD identify hotspots (file I/O, threading, crypto) and refactor incrementally to hide platform dependencies from public headers.

## Future Enhancements *(deferred)*

These features are identified but not scheduled for the baseline implementation. They may be added in future feature branches based on user demand and priority.

### FE-001: Package Viewer GUI Modernization

**Priority**: High  
**Effort**: 2-4 hours per enhancement  
**Description**: Continue enhancing the Package Viewer with modern file type support and user experience improvements.

**Completed in this session**:
- ✅ XML file viewer (.xml) with BOM detection
- ✅ Text file viewer (.txt, .ini, .cfg, .json, .log, .toc)
- ✅ ZIP archive viewer (.zip) with Qt's QZipReader
  - Table view of ZIP contents with file path, size, and compression info
  - Extract selected files or extract all functionality
  - Numeric sorting for file sizes (fixes text-based sorting issues)
  - Human-readable size display (bytes/KB/MB)
  - Zero external dependencies (uses Qt private API)
- ✅ Image viewer enhancements (filename in title, resizable windows)
- ✅ Case-insensitive file extension detection
- ✅ Proper Windows/Xbox 360 encoding detection (UTF-8, UTF-16 LE/BE with BOM, Latin1 fallback)

**Additional enhancements to consider**:
- Hex viewer for binary/unknown files (see FE-005)
- HTML viewer using QTextBrowser for .html/.htm files
- SVG image support for vector graphics
- Syntax highlighting in XML/text viewers (optional)
- Search functionality within text/XML/ZIP viewers
- Export visible content to clipboard or file
- File comparison view (diff two files side-by-side)
- Thumbnail preview in file tree for images
- Preview files inside ZIP archives using existing viewers

**Use Cases**:
- Quick inspection of package contents without extraction
- Understanding file structure and relationships
- Debugging content issues in Xbox 360 packages
- Educational tool for learning Xbox 360 file formats
- Improved workflow for content creators and modders

**Benefits**:
- Reduced workflow friction (no extract-open-close cycle)
- Better understanding of package contents
- More professional and modern UI
- Competitive with similar tools

**Status**: Partially implemented. Core viewers working well (9 file types now supported: Images, XML, Text, ZIP, GPD, STRB, PEC, STFS, Unknown). Additional enhancements can be added incrementally based on user feedback and common file types discovered in Xbox 360 packages.

---

### FE-002: Achievement and Avatar Download System Refactor

**Priority**: High (broken functionality — deferred to future branch, NOT baseline-blocking)  
**Effort**: 12-24 hours  
**Description**: Refactor and fix the existing achievement GPD and avatar award download system to be fully functional and maintainable. **Note**: While important, this is deferred to a dedicated future feature branch focused on network reliability; baseline functionality does not depend on the download system.

**Current Implementation**:
- `GpdDownloader` class downloads game GPD and award GPD files from GitHub repository
- `AvatarAssetDownloader` class downloads avatar award assets (v1/v2 skeleton versions)
- Base URL: `https://raw.githubusercontent.com/Pandoriaantje/xbox360-gpd-files/main`
- Used in Game Adder dialog and Profile Editor
- Downloads are currently **not fully functional** (reported by user)

**Known Issues**:
- Download reliability problems (needs investigation)
- Error handling may be insufficient
- Network timeouts not properly handled
- GitHub repository availability/structure dependencies
- Spelling: "Unfortunatly" → "Unfortunately" in warning dialog
- Limited feedback during download progress
- No retry mechanism for failed downloads
- Unclear error messages for users

**Proposed Improvements**:
- **Error Handling**: Implement robust error handling with detailed user feedback
- **Retry Logic**: Add automatic retry with exponential backoff for transient failures
- **Progress Feedback**: Show download progress (bytes downloaded, speed, ETA)
- **Timeout Configuration**: Make network timeouts configurable
- **Offline Mode**: Cache previously downloaded files, support offline operation
- **Alternative Sources**: Support multiple mirror URLs or fallback repositories
- **Validation**: Verify downloaded GPD files are valid before importing
- **User Cancellation**: Allow users to cancel in-progress downloads
- **Rate Limiting**: Respect GitHub API rate limits, add throttling
- **Modern Async**: Consider using Qt's modern async APIs instead of signals/slots

**Implementation Tasks**:
1. Audit current download code paths in `GpdDownloader` and `AvatarAssetDownloader`
2. Identify specific failure modes and root causes
3. Add comprehensive error logging and user-facing error messages
4. Implement download progress UI with cancel button
5. Add retry logic with configurable max attempts
6. Implement download validation (file size, magic numbers, GPD parsing)
7. Add network timeout configuration in preferences
8. Consider caching mechanism for frequently downloaded files
9. Test with various network conditions (slow, intermittent, offline)
10. Update documentation with troubleshooting guide

**Use Cases**:
- Download achievement definitions for games not in local collection
- Obtain avatar awards without manual extraction from packages
- Populate profiles with game data for testing/modding
- Batch download multiple game GPD files in Game Adder
- Recover missing achievement data

**Benefits**:
- Reliable achievement and avatar data acquisition
- Better user experience with clear feedback
- Reduced support burden from download failures
- More robust network error handling
- Professional-grade download management

**Deferred Reason**: Current implementation exists but needs significant reliability improvements. Requires investigation of failure modes, testing infrastructure for network scenarios, and careful refactoring of async code. High priority but needs dedicated focus to implement properly.

---

### FE-003: Dark Theme Support

**Priority**: High (visual polish)  
**Effort**: 4-8 hours  
**Description**: Add dark mode UI theme with automatic or manual theme switching.

**Functionality**:
- Dark color palette for all dialogs, windows, and widgets
- Automatic detection of system theme preference (Windows 10/11)
- Manual theme toggle in Preferences dialog (Light/Dark/Auto)
- Custom icons and assets optimized for dark backgrounds
- Maintain AA contrast requirements for accessibility
- Preserve all current UI layouts and functionality
- Apply theme consistently across all dialogs and viewers

**Use Cases**:
- Reduced eye strain during extended use
- Better visual comfort in low-light environments
- Matches modern application UI trends
- Accessibility benefit for light-sensitive users
- Professional appearance for streaming/recording workflows

**Implementation**:
- Create dark.qss stylesheet or use Qt palette API
- Add theme preference to settings storage
- Implement theme switching without restart (live update)
- Test all dialogs (Package Viewer, Profile Editor, Device Viewer, etc.)
- Update icons/images for dark background visibility
- Validate text/background contrast ratios meet AA standards

**Benefits**:
- Improved user experience for long sessions
- Modern UI appearance
- Reduced screen brightness for battery savings on laptops
- Popular feature request in similar tools

**Deferred Reason**: Current default theme is functional and accessible. Dark theme is aesthetic enhancement rather than functional requirement. Requires comprehensive UI testing across all 140+ source files and dialogs to ensure consistent appearance and readability.

---

### FE-004: MSVC Build Compatibility

**Priority**: Low (developer tooling, not user-facing)  
**Effort**: 8-16 hours  
**Description**: Add full Microsoft Visual C++ (MSVC) compiler support alongside the existing MinGW toolchain.

**Functionality**:
- Create MSVC-specific CMake presets (Debug/Release configurations)
- Resolve MSVC-specific compilation warnings and errors
- Ensure Botan builds correctly with MSVC
- Update CMakeLists.txt with MSVC-specific flags if needed
- Test all XboxInternals and Velocity modules with MSVC compiler
- Document MSVC setup requirements in BUILD.md

**Use Cases**:
- Windows developers using Visual Studio IDE
- Better integration with Visual Studio debugger and profiler
- Potential performance improvements with Microsoft's optimizer
- Compatibility with Windows-specific development tools
- Easier contribution setup for developers familiar with VS

**Implementation**:
- Add `windows-msvc-debug` and `windows-msvc-release` to CMakePresets.json
- Address compiler-specific issues (name mangling, standard library differences)
- Test with Visual Studio 2022 (17.0+) and MSVC toolset v143
- Ensure Qt 6.7.3+ MSVC builds are detected correctly
- Validate Botan cryptography works identically across both compilers

**Benefits**:
- Broader developer accessibility
- Official Microsoft toolchain support
- Better Windows ecosystem integration
- Alternative compiler for testing C++20 standard compliance

**Deferred Reason**: Current MinGW build is stable and functional. MSVC support would benefit contributors using Visual Studio, but is not critical for end-user functionality. Requires testing infrastructure and validation across both toolchains.

---

### FE-005: Hex Viewer for Binary Files

**Priority**: Low (advanced feature, nice-to-have)  
**Effort**: 2-4 hours  
**Description**: Add a hex viewer dialog for inspecting unknown or binary file types within packages.

**Functionality**:
- Display format: `offset | hex bytes (16 per row) | ASCII preview`
- Read-only viewer using QTextEdit with monospace font (Courier/Consolas)
- Format: `00000000  48 65 6C 6C 6F 20 57 6F  72 6C 64 21 0D 0A 00 00  |Hello World!.....|`
- Size limit: 1MB for performance (truncate with warning for larger files)
- Shows printable ASCII (0x20-0x7E), dots for non-printable bytes
- Reuses dialog pattern from XmlDialog/TextDialog (800x600, filename in title)

**Use Cases**:
- View raw bytes of encrypted/compressed files
- Debug corrupted files or unknown formats
- Inspect save game data structures
- Verify file signatures and magic numbers (LIVE, CON, PIRS)
- Fallback viewer for unrecognized file types

**Implementation**:
- Create `hexdialog.h/.cpp/.ui` (similar to textdialog)
- Add helper function to format bytes into hex dump string
- Wire up in packageviewer.cpp for unknown file types or via right-click "View as Hex"
- Add to CMakeLists.txt

**Benefits**:
- Useful for debugging and low-level inspection
- No external dependencies (pure Qt)
- Educational tool for understanding Xbox 360 file formats

**Deferred Reason**: Current file viewers cover the most common file types (.xml, .txt, .cfg, .ini, .json, .log, .toc, images, GPD, STRB). Hex viewing is valuable but serves a smaller audience (advanced users doing format research).

