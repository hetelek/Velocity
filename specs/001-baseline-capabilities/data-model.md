# Data Model: Velocity Baseline Capabilities# Data Model: Velocity Baseline Capabilities



Date: 2025-10-19Date: 2025-10-19

Branch: 001-baseline-capabilitiesBranch: 001-baseline-capabilities

Spec: specs/001-baseline-capabilities/spec.md

## Entities

## Core Entities

### FATX Source

### FATX Source- Fields: partitions[], fileEntries[], size, path, mountInfo?

**Description**: Local FATX filesystem (Xbox 360 storage device/partition)- Relationships: contains File Entries

**Fields**:- Notes: read-only; extraction supported

- `path`: Filesystem path (string, required)

- `label`: Volume label (string, optional)### STFS Package

- `totalSize`: Partition size in bytes (uint64, required)- Fields: metadata (titleId, displayName, contentId, size), fileEntries[]

- `freeSpace`: Available space in bytes (uint64, required)- Relationships: contains File Entries

- `filesystemType`: FATX/FATX16/FATX32 (enum, required)- Notes: read-only in baseline

- `isReadOnly`: Mount status (bool, default false)

- `rootDirectory`: Directory tree root (DirectoryNode, required)### GPD (XDBF) Profile Data

- Fields: profileId, gamerTag?, achievements[] (achievementId, titleId, name, description, unlocked, timestamp, gamerscore)

**Relationships**:- Relationships: n/a

- Contains many DirectoryNode instances (hierarchical tree)- Notes: exportable to CSV/JSON

- Can be source for ExtractionJob

### Disc Image (SVOD/GDFX)

**Validation Rules**:- Fields: structure, fileEntries[], size, path

- `path` must be valid filesystem path or device path- Relationships: contains File Entries

- `freeSpace` ≤ `totalSize`- Notes: read-only; extraction supported

- `label` max 32 characters (FATX limit)

### Extraction Job

---- Fields: sourceType, selections[], destination, progress (0–100), status (pending|running|canceled|completed|failed), eta?

- Relationships: operates on FATX/STFS/GPD/Disc selections

### STFS Package- Notes: supports cancel; reports errors

**Description**: Xbox 360 STFS container (profile, save, DLC, etc.)

**Fields**:### Validation Indicator

- `path`: File path (string, required)- Fields: status (verified|failed|unknown), message

- `packageType`: Profile/SavedGame/DLC/Theme/GamerPicture (enum, required)- Relationships: associated with open source

- `contentType`: Xbox content type ID (uint32, required)- Notes: non-blocking display when unknown

- `displayName`: Package display name (string, required)

- `titleId`: Associated game title ID (uint32, optional)
- `titleName`: Game title name (string, optional)
- `fileCount`: Number of files in package (uint32, required)
- `totalSize`: Package size in bytes (uint64, required)
- `certificate`: Package certificate data (Certificate, optional)
- `metadata`: Package metadata fields (MetadataDict, required)

**Relationships**:
- May contain one GPDProfile (if packageType == Profile)
- May contain Certificate
- Can be source for ExtractionJob
- Can be target for PackageCreationWizard

**Validation Rules**:
- `path` must exist and be readable
- `packageType` must match `contentType` mapping
- `displayName` max 128 characters
- Certificate must be valid if present (see Certificate validation)

---

### GPD Profile
**Description**: Xbox 360 XDBF/GPD profile data (achievements, settings, stats)
**Fields**:
- `gamertag`: User gamertag (string, required)
- `gamerscore`: Total gamerscore (uint32, required)
- `reputation`: Reputation score (float, required)
- `achievements`: List of achievements (Achievement[], required)
- `gameSettings`: Per-title settings (SettingDict, required)
- `gameStats`: Per-title statistics (StatDict, required)
- `avatar`: Avatar asset references (AvatarAssetList, optional)

**Relationships**:
- Contained within STFSPackage (Profile type)
- Contains many Achievement instances
- References AvatarAssets via asset IDs

**Validation Rules**:
- `gamertag` must be 3-15 characters, alphanumeric + spaces
- `gamerscore` must equal sum of unlocked achievement scores
- `reputation` range: 0.0 to 5.0
- Achievement IDs must be unique within profile

---

### Achievement
**Description**: Single achievement entry in GPD profile
**Fields**:
- `achievementId`: Unique achievement ID (uint32, required)
- `titleId`: Game title ID (uint32, required)
- `name`: Achievement name (string, required)
- `description`: Achievement description (string, required)
- `unlockedDescription`: Unlocked flavor text (string, optional)
- `gamerscore`: Points awarded (uint32, required)
- `imageId`: Icon image ID (uint32, optional)
- `flags`: Achievement flags (uint32, required)
- `unlocked`: Unlock status (bool, required)
- `unlockTime`: Unlock timestamp (DateTime, optional)

**Relationships**:
- Belongs to GPDProfile
- References game via `titleId`

**Validation Rules**:
- `achievementId` must be unique per `titleId`
- `gamerscore` typically 5, 10, 15, 20, 25, 50, 100 (not enforced)
- `unlockTime` required if `unlocked == true`
- `name` and `description` max 256 characters

---

### SVOD Package
**Description**: Secure Video-On-Demand multi-file package
**Fields**:
- `basePath`: Directory containing SVOD data files (string, required)
- `dataFiles`: List of data file paths (string[], required)
- `totalSize`: Total package size (uint64, required)
- `certificate`: Package certificate (Certificate, optional)
- `metadata`: Package metadata (MetadataDict, required)

**Relationships**:
- May contain Certificate
- Can be source for ExtractionJob

**Validation Rules**:
- All `dataFiles` must exist and be readable
- Data file names follow Data####.bin pattern
- Certificate must be valid if present

---

### GDFX Disc Image
**Description**: Xbox 360 disc image in GDFX format
**Fields**:
- `path`: Disc image file path (string, required)
- `volumeLabel`: Disc volume label (string, required)
- `totalSize`: Image size in bytes (uint64, required)
- `fileSystem`: Directory tree root (DirectoryNode, required)

**Relationships**:
- Contains DirectoryNode tree (hierarchical file structure)
- Can be source for ExtractionJob

**Validation Rules**:
- `path` must be valid ISO/GDFX file
- `volumeLabel` max 32 characters
- File structure must conform to GDFX layout

---

### YTGR Theme Package
**Description**: Xbox 360 theme package (YTGR format)
**Fields**:
- `path`: Theme package file path (string, required)
- `themeName`: Theme display name (string, required)
- `backgroundColor`: Background color (Color, required)
- `assets`: List of theme assets (ThemeAsset[], required)
- `metadata`: Package metadata (MetadataDict, required)

**Relationships**:
- Can be source/target for creation wizards
- Contains ThemeAsset instances (images, sounds)

**Validation Rules**:
- `themeName` max 64 characters
- Assets must include required types (background, highlight, etc.)
- Color values must be valid RGB/RGBA

---

### STRB Gamer Picture Pack
**Description**: Xbox 360 gamer picture pack (STRB format)
**Fields**:
- `path`: Pack file path (string, required)
- `packName`: Pack display name (string, required)
- `pictures`: List of gamer pictures (GamerPicture[], required)
- `metadata`: Package metadata (MetadataDict, required)

**Relationships**:
- Can be source/target for creation wizards
- Contains GamerPicture instances

**Validation Rules**:
- `packName` max 64 characters
- Each picture must be 64x64 PNG/JPEG
- Pack must contain at least 1 picture

---

### Profile Package
**Description**: Complete profile package (container + GPD)
**Fields**:
- `stfsPackage`: Outer STFS container (STFSPackage, required)
- `gpdProfile`: Inner GPD profile data (GPDProfile, required)
- `isModified`: Modification flag (bool, default false)
- `backupPath`: Last backup location (string, optional)

**Relationships**:
- Wraps one STFSPackage
- Wraps one GPDProfile
- May reference backup file

**Validation Rules**:
- `stfsPackage.packageType` must be Profile
- `gpdProfile` must be extractable from `stfsPackage`
- Backup path must exist if set

---

### Plugin
**Description**: Loadable plugin extending Velocity functionality
**Fields**:
- `path`: Plugin DLL/SO file path (string, required)
- `name`: Plugin display name (string, required)
- `version`: Plugin version (string, required)
- `description`: Plugin description (string, optional)
- `author`: Plugin author (string, optional)
- `interfaceVersion`: Expected API version (uint32, required)
- `capabilities`: Plugin capabilities (string[], required)
- `consentGiven`: User consent status (bool, default false)
- `enabled`: Plugin enable status (bool, default true)

**Relationships**:
- Registered in PluginRegistry
- May interact with any entity via plugin API

**Validation Rules**:
- `path` must be valid DLL/SO file
- `interfaceVersion` must match app API version
- `consentGiven` must be true before loading
- `name` max 128 characters

---

### Device
**Description**: Physical storage device (USB, HDD, memory unit)
**Fields**:
- `devicePath`: System device path (string, required)
- `deviceName`: User-friendly name (string, required)
- `deviceType`: USB/HDD/MemoryUnit (enum, required)
- `totalCapacity`: Total device capacity (uint64, required)
- `partitions`: List of partitions (Partition[], required)

**Relationships**:
- Contains one or more Partition instances
- Detected by DeviceScanner

**Validation Rules**:
- `devicePath` must be valid system device path
- `totalCapacity` must equal sum of partition sizes
- At least one partition required

---

### Partition
**Description**: Storage partition on device
**Fields**:
- `partitionIndex`: Partition number (uint32, required)
- `partitionName`: Partition label (string, optional)
- `filesystemType`: FATX/FATX16/FATX32/Other (enum, required)
- `startOffset`: Partition start offset (uint64, required)
- `size`: Partition size (uint64, required)
- `usedSpace`: Used space in bytes (uint64, required)

**Relationships**:
- Belongs to Device
- Can be mounted as FATXSource

**Validation Rules**:
- `partitionIndex` must be unique within device
- `usedSpace` ≤ `size`
- `startOffset` + `size` must not exceed device capacity

---

### Extraction Job
**Description**: Background file extraction operation
**Fields**:
- `jobId`: Unique job ID (UUID, required)
- `sourcePath`: Source package/device path (string, required)
- `targetPath`: Extraction destination (string, required)
- `transferFlags`: Extraction options (TransferFlags, required)
- `totalFiles`: Number of files to extract (uint32, required)
- `processedFiles`: Files extracted so far (uint32, required)
- `totalBytes`: Total bytes to extract (uint64, required)
- `processedBytes`: Bytes extracted so far (uint64, required)
- `status`: Running/Completed/Cancelled/Failed (enum, required)
- `startTime`: Job start time (DateTime, required)
- `endTime`: Job end time (DateTime, optional)
- `errorMessage`: Error description if failed (string, optional)

**Relationships**:
- References source entity (FATXSource/STFSPackage/SVODPackage/GDFXDiscImage)
- Monitored by MultiProgressDialog

**Validation Rules**:
- `processedFiles` ≤ `totalFiles`
- `processedBytes` ≤ `totalBytes`
- `endTime` required if status is Completed/Cancelled/Failed
- `errorMessage` required if status is Failed

---

### Transfer Flags
**Description**: Options for file extraction/transfer operations
**Fields**:
- `overwriteExisting`: Overwrite existing files (bool, default false)
- `preserveTimestamps`: Keep original timestamps (bool, default true)
- `createDirectories`: Create parent directories (bool, default true)
- `skipOnError`: Continue on individual file errors (bool, default false)
- `verifyIntegrity`: Hash-check after transfer (bool, default false)

**Relationships**:
- Used by ExtractionJob

**Validation Rules**:
- No complex validation; user preferences

---

### Validation Indicator
**Description**: UI indicator for data validation status
**Fields**:
- `status`: Valid/Invalid/Unchecked (enum, required)
- `iconType`: Checkmark/Warning/Error (enum, required)
- `message`: Status message (string, optional)
- `details`: Detailed validation info (string, optional)

**Relationships**:
- Displayed for any entity with validation rules
- Referenced by UI components

**Validation Rules**:
- `message` max 256 characters
- `details` max 2048 characters

---

### Certificate
**Description**: Xbox 360 package certificate data
**Fields**:
- `certificateId`: Certificate ID (bytes, required)
- `certificateSize`: Certificate size (uint32, required)
- `ownerConsoleId`: Console ID (bytes, optional)
- `ownerConsolePartNumber`: Console part number (string, optional)
- `ownerConsoleType`: Console type flags (uint32, optional)
- `dateGeneration`: Generation date (string, optional)
- `expirationDate`: Expiration date (DateTime, optional)
- `titleId`: Associated title ID (uint32, optional)
- `titleName`: Associated title name (string, optional)
- `signature`: Certificate signature (bytes, optional)

**Relationships**:
- Contained in STFSPackage or SVODPackage
- Displayed in CertificateDialog

**Validation Rules**:
- `certificateId` must be 16 or 32 bytes
- `certificateSize` must match actual certificate data size
- `signature` must be valid if present (cryptographic check via Botan)
- `expirationDate` must be after `dateGeneration` if both present

---

## Supporting Types

### DirectoryNode
**Description**: File or directory in hierarchical tree
**Fields**:
- `name`: File/directory name (string)
- `isDirectory`: Directory flag (bool)
- `size`: File size if not directory (uint64)
- `createdTime`: Creation timestamp (DateTime, optional)
- `modifiedTime`: Modification timestamp (DateTime, optional)
- `children`: Child nodes if directory (DirectoryNode[], optional)

---

### MetadataDict
**Description**: Key-value metadata store
**Fields**:
- `entries`: Map of metadata keys to values (map<string, string>)

---

### SettingDict
**Description**: Game-specific settings
**Fields**:
- `titleId`: Associated title ID (uint32)
- `settings`: Map of setting IDs to values (map<uint32, bytes>)

---

### StatDict
**Description**: Game-specific statistics
**Fields**:
- `titleId`: Associated title ID (uint32)
- `stats`: Map of stat IDs to values (map<uint32, int64>)

---

### AvatarAssetList
**Description**: List of avatar asset references
**Fields**:
- `assets`: List of asset IDs (uint32[])

---

### ThemeAsset
**Description**: Single asset in theme package
**Fields**:
- `assetType`: Background/Highlight/Sound/etc. (enum)
- `data`: Asset binary data (bytes)

---

### GamerPicture
**Description**: Single gamer picture in pack
**Fields**:
- `pictureId`: Unique picture ID (uint32)
- `imageData`: Picture binary data (bytes, 64x64)

---

### Color
**Description**: RGB/RGBA color value
**Fields**:
- `r`: Red component (uint8)
- `g`: Green component (uint8)
- `b`: Blue component (uint8)
- `a`: Alpha component (uint8, optional)

---

## Relationships Diagram

```
Device
  └─> Partition[] ────────┐
                          │
FATXSource <──────────────┘
  └─> DirectoryNode[]
        └─> DirectoryNode[] (recursive)
                                │
                                ├─> ExtractionJob
                                │     ├─> TransferFlags
STFSPackage ────────────────────┤     └─> (status/progress fields)
  ├─> Certificate               │
  ├─> MetadataDict              │
  └─> GPDProfile                │
        ├─> Achievement[]       │
        ├─> SettingDict         │
        ├─> StatDict            │
        └─> AvatarAssetList     │
                                │
SVODPackage ─────────────────────┤
  ├─> Certificate               │
  └─> MetadataDict              │
                                │
GDFXDiscImage ───────────────────┤
  └─> DirectoryNode[]           │
                                │
YTGRThemePackage ────────────────┤
  ├─> ThemeAsset[]              │
  └─> MetadataDict              │
                                │
STRBGamerPicturePack ────────────┘
  ├─> GamerPicture[]
  └─> MetadataDict

Plugin
  └─> (registered in PluginRegistry, interacts with any entity)

ValidationIndicator
  └─> (displayed for any validated entity)

ProfilePackage
  ├─> STFSPackage
  └─> GPDProfile
```

---

## Data Flow Patterns

### Read/Browse Flow
1. User selects source (Device/FATXSource/STFSPackage/etc.)
2. App loads entity with validation
3. ValidationIndicator shows status
4. DirectoryNode tree displayed in UI
5. User navigates hierarchy

### Extraction Flow
1. User selects files/directories from source
2. User chooses destination path
3. User configures TransferFlags
4. App creates ExtractionJob with UUID
5. Background thread processes files, updates job progress
6. MultiProgressDialog monitors job status
7. Job completes or fails with errorMessage

### Edit Flow (with Backup)
1. User opens ProfilePackage (STFSPackage + GPDProfile)
2. User modifies Achievement or profile fields
3. ProfilePackage.isModified set to true
4. On save: App creates backup to backupPath
5. If backup fails: Abort with error (no write)
6. If backup succeeds: Write modified ProfilePackage to original path
7. Update backupPath field

### Plugin Load Flow
1. App scans plugin directory for Plugin entities
2. User enables Plugin in preferences
3. If consentGiven == false: Show consent dialog
4. If user accepts: Set consentGiven = true
5. App loads plugin DLL/SO, validates interfaceVersion
6. Plugin registers capabilities with app
7. Plugin appears in Tools menu or format handlers

---

## Constitution Alignment

### Module Boundaries (Constitution I)
- **XboxInternals entities**: FATXSource, STFSPackage, GPDProfile, Achievement, SVODPackage, GDFXDiscImage, YTGRThemePackage, STRBGamerPicturePack, Certificate, DirectoryNode
  - These must NOT depend on Qt types; use C++ standard library types only
  - File paths are `std::string` or `std::filesystem::path`
  - Timestamps are `std::chrono` types

- **Velocity GUI entities**: ProfilePackage, Plugin, Device, Partition, ExtractionJob, TransferFlags, ValidationIndicator
  - These MAY use Qt types (`QString`, `QDateTime`, `QVariant`, etc.)
  - Bridge between XboxInternals and Qt happens in Velocity layer

### Platform Abstraction (Constitution III-A)
- File I/O abstracted via `std::filesystem` (XboxInternals) or `QFile` (Velocity)
- Timestamps use `std::chrono` (XboxInternals) or `QDateTime` (Velocity)
- Threading via `std::jthread` (XboxInternals) or `QThreadPool` (Velocity)
- Crypto operations via Botan (both layers)

### Data Validation
- All entities include validation rules enforced at construction or modification
- ValidationIndicator provides UI feedback
- Constitution IV encourages testing; validation logic must be unit-testable

---

## Implementation Notes

- **Persistence**: Most entities are ephemeral (loaded from files on demand). Only Plugin preferences and backup paths persist in QSettings.
- **Memory Management**: Prefer value types or `std::unique_ptr`/`QScopedPointer` for ownership. Avoid raw pointers.
- **Concurrency**: ExtractionJob runs in background thread; use `std::atomic` for status flags and Qt signals for progress updates.
- **Extensibility**: Plugin entity allows runtime extension of format support without recompiling core app.

---

## References
- Spec: `specs/001-baseline-capabilities/spec.md`
- Constitution: `.specify/memory/constitution.md`
- Tasks: `specs/001-baseline-capabilities/tasks.md`
