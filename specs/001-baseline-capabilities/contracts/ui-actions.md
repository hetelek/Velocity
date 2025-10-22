# UI Action Contracts: Velocity Complete Functionality# UI Contracts: Baseline Actions



Date: 2025-10-19Date: 2025-10-19

Branch: 001-baseline-capabilitiesBranch: 001-baseline-capabilities

Spec: specs/001-baseline-capabilities/spec.md

## Open Source

## US1: Browse & Extract (P1)- Inputs: path (file/image)

- Preconditions: path exists; readable

### Open Source- UI State: busy with progress; cancel available

**Inputs**: `path` (file/image/device path)- Outputs: tree populated; validation indicator set (verified|failed|unknown)

**Preconditions**: - Errors: unsupported/corrupt → message + disable dependent actions

- `path` exists and is readable

- Format supported (FATX/STFS/GPD/SVOD/GDFX)## Extract Selection

**UI State**: - Inputs: selections[], destination

- Busy cursor with progress indicator- Preconditions: destination writable; selections non-empty

- "Opening..." status text- UI State: progress dialog; cancel available

- Cancel button available if >0.5s- Outputs: files written to destination; summary of successes/failures

**Outputs**: - Errors: permission denied; disk full; path conflicts → actionable messages

- Directory tree populated in left pane

- Metadata displayed in properties panel## Export Achievements

- Validation indicator shown (checkmark/warning/error)- Inputs: format (CSV|JSON), visible fields

**Postconditions**:- Preconditions: a GPD file is open; achievements loaded

- Source is loaded and browsable- UI State: progress (if large); cancel available

- ValidationIndicator status set (Valid/Invalid/Unchecked)- Outputs: file written with required fields (titleId, achievementId, name, description, unlocked, timestamp?, gamerscore)

**Errors**:- Errors: permission denied; invalid path → prompt fix

- Unsupported format → "Unsupported file type" dialog + disable dependent actions

- Corrupt/invalid → "File appears corrupted" dialog + show partial data if recoverable## Attempt Edit (Baseline)

- Permission denied → "Access denied" dialog with elevation prompt (Windows)- Inputs: requested edit action

**Performance**: <30s for device open; <5s for package open- Policy: baseline is read-only

- Behavior: block action; show guidance and optional link to documentation

---

### Browse Hierarchy
**Inputs**: `selectedNode` (DirectoryNode)
**Preconditions**: Source is loaded
**UI State**: 
- Selected node highlighted
- Right pane shows contents (files/subdirectories)
**Outputs**: 
- Node children displayed with name, size, modified date
- Thumbnail previews for images if applicable
**Postconditions**: User can navigate into subdirectories or select files
**Errors**: (No typical errors; read-only operation)

---

### Extract Selection
**Inputs**: 
- `selections[]` (list of DirectoryNode)
- `destination` (filesystem path)
- `transferFlags` (TransferFlags)
**Preconditions**: 
- `selections` is non-empty
- `destination` is writable
- Source is loaded
**UI State**: 
- Multi-progress dialog visible
- Per-file progress bars
- Overall progress percentage
- Cancel button available
- Time remaining estimate
**Outputs**: 
- Files/directories written to `destination`
- Extraction log with successes/failures
- Summary notification ("23 files extracted, 1 failed")
**Postconditions**: 
- ExtractionJob status = Completed/Cancelled/Failed
- User can open destination folder or view log
**Errors**:
- Destination permission denied → "Cannot write to [path]. Choose different location?"
- Disk full → "Insufficient disk space. Need X MB, have Y MB."
- Path conflicts (overwrite) → Honor `transferFlags.overwriteExisting`; prompt if false
- Individual file errors → Honor `transferFlags.skipOnError`; show in log
**Performance**: Progress updates every 0.5s; cancellation responsive at checkpoints

---

### Export Achievements
**Inputs**: 
- `format` (CSV | JSON)
- `visibleFields` (list of field names)
**Preconditions**: 
- GPD profile is loaded
- Achievements list is populated
**UI State**: 
- "Exporting..." progress if >0.5s
- Cancel available for large exports
**Outputs**: 
- File written to user-selected path
- Minimum fields: titleId, achievementId, name, description, unlocked (bool), timestamp (if available), gamerscore
**Postconditions**: Export file exists and is valid CSV/JSON
**Errors**:
- Permission denied → "Cannot write to [path]. Try different location?"
- Invalid path → Prompt for valid filename
**Performance**: <5s for typical profile (100-500 achievements)

---

### View Validation Status
**Inputs**: (automatic on source open)
**Preconditions**: Source is loaded
**UI State**: 
- ValidationIndicator icon visible (checkmark/warning/error)
- Tooltip on hover shows validation message
- "View Details" button if validation failed
**Outputs**: 
- Quick visual feedback (icon + color)
- Optional detailed validation report dialog
**Postconditions**: User understands source integrity status
**Errors**: (No user-triggered errors; displays validation results)

---

## US2: Profile & Achievements (P2)

### Open Profile Viewer
**Inputs**: `profilePackagePath` (STFS profile package)
**Preconditions**: 
- Path is valid STFS package
- Package contains GPD data
**UI State**: 
- Busy with "Loading profile..." message
- Progress indicator if >0.5s
**Outputs**: 
- Profile overview tab populated (gamertag, gamerscore, reputation)
- Achievements tab populated with filterable list
- Game settings tab populated
**Postconditions**: User can browse profile data
**Errors**:
- Not a profile package → "This package does not contain profile data"
- Corrupt GPD → "Profile data is corrupted" + show recoverable fields
**Performance**: <10s for profile open

---

### Filter Achievements
**Inputs**: 
- `filterCriteria` (locked/unlocked/game title/keyword)
- `sortOrder` (date/name/gamerscore)
**Preconditions**: Profile is loaded
**UI State**: 
- Achievement list updates in real-time (no blocking)
**Outputs**: 
- Filtered/sorted achievement list
- Result count ("Showing 12 of 450 achievements")
**Postconditions**: User can focus on relevant achievements
**Errors**: (No typical errors; UI filter operation)

---

### View Achievement Details
**Inputs**: `achievement` (Achievement)
**Preconditions**: Achievement is selected in list
**UI State**: 
- Detail panel shows full info
- Achievement icon displayed
**Outputs**: 
- All achievement fields visible (name, description, unlock date, gamerscore, flags)
- Related game information (title, titleId)
**Postconditions**: User can see complete achievement metadata
**Errors**: (No typical errors; read-only display)

---

## US3: Safe Edits with Backup (P3)

### Edit Achievement (with Backup)
**Inputs**: 
- `achievement` (Achievement to modify)
- `newValues` (fields to change: unlocked, unlockTime, etc.)
**Preconditions**: 
- Profile is loaded
- User has accepted edit warning (first-time dialog)
**UI State**: 
- "Creating backup..." progress
- "Saving changes..." progress
- Total time estimate shown if >5s expected
**Outputs**: 
- Backup created at `backupPath` (default: %APPDATA%/Velocity/Backups/[timestamp]_[filename])
- Modified profile written to original path
- ProfilePackage.isModified = true
- Success notification with "Undo" option
**Postconditions**: 
- Achievement values updated in file
- Backup available for restore
**Errors**:
- Backup failed (disk full/permissions) → **ABORT** with "Backup failed: [reason]. Changes NOT saved."
- Write failed after backup → "Original file could not be updated. Backup preserved at [path]."
- Validation failed → "Changes would create invalid profile. Aborting."
**Performance**: 
- Progress shown at 0.5s
- User warned if operation >30s (unlikely for single achievement)
- Cancellation supported before write phase

---

### Rehash/Resign Package
**Inputs**: `stfsPackage` (modified STFS package)
**Preconditions**: 
- Package is loaded
- Package has been modified
**UI State**: 
- "Rehashing package..." progress
- Botan crypto operations status
**Outputs**: 
- Package hash recalculated
- Package signature updated (if applicable)
- Certificate updated with new signature
**Postconditions**: Package passes Xbox 360 integrity checks
**Errors**:
- Crypto failure → "Could not rehash package: [Botan error]"
- Missing certificate → "Package requires certificate for signing"
**Performance**: <10s for typical package

---

### Restore from Backup
**Inputs**: `backupPath` (path to backup file)
**Preconditions**: 
- Backup file exists and is valid
- Original file location is writable
**UI State**: 
- "Restoring from backup..." progress
- Confirmation dialog before overwrite
**Outputs**: 
- Original file overwritten with backup copy
- Success notification
**Postconditions**: File restored to pre-edit state
**Errors**:
- Backup corrupt → "Backup file is corrupted or incomplete"
- Original path not writable → "Cannot restore to [path]. Check permissions."
**Performance**: <5s for typical profile

---

### Manage Backups
**Inputs**: (opens backup management UI)
**Preconditions**: Backup directory exists
**UI State**: 
- List of backups with timestamp, original filename, size
- "Open Backup Folder" button
- "Restore" and "Delete" buttons for selected backup
**Outputs**: 
- User can browse, restore, or delete backups
- Disk space usage shown
**Postconditions**: User controls backup retention manually
**Errors**:
- Backup folder inaccessible → "Cannot access backup folder at [path]"

---

## US4: Creation Wizards (P2)

### Create STFS Package (Wizard)
**Inputs**: 
- `sourceFiles[]` (files to package)
- `metadata` (titleId, displayName, contentType, etc.)
- `certificateOptions` (optional: console ID, signature)
**Preconditions**: 
- Source files exist and are readable
- Metadata is valid (displayName ≤128 chars, etc.)
**UI State**: 
- Multi-step wizard dialog
- Step 1: Select files
- Step 2: Configure metadata
- Step 3: Certificate options
- Step 4: Destination path
- Progress bar on "Creating package..." step
**Outputs**: 
- New STFS package created at destination
- Package contains all source files with correct structure
- Metadata embedded
- Certificate generated if requested
**Postconditions**: Package is valid and can be opened in Velocity
**Errors**:
- Invalid metadata → "Display name too long" (highlight field)
- File too large → "Package would exceed STFS limits"
- Write failure → "Could not create package at [path]"
**Performance**: Progress updates at 0.5s; cancellation available; warn if >30s

---

### Create Achievement (Wizard)
**Inputs**: 
- `profilePackage` (target profile)
- `achievementData` (titleId, name, description, gamerscore, etc.)
**Preconditions**: 
- Profile is loaded
- titleId is valid
- achievementId is unique within profile
**UI State**: 
- Achievement creation dialog with form fields
- Image picker for achievement icon
- Preview pane
**Outputs**: 
- New achievement added to profile
- Profile modified and saved (with backup per US3)
**Postconditions**: Achievement appears in profile viewer
**Errors**:
- Duplicate achievementId → "Achievement ID already exists for this game"
- Invalid gamerscore → "Gamerscore must be positive integer"
- Backup failure → **ABORT** per US3 policy
**Performance**: <5s for achievement creation

---

### Create Theme Package (Wizard)
**Inputs**: 
- `themeAssets[]` (background, highlight images, sounds)
- `themeName` (display name)
- `backgroundColor` (Color)
**Preconditions**: 
- All required asset types provided
- Assets meet format/size requirements (dimensions, file types)
**UI State**: 
- Multi-step wizard with asset upload
- Preview of theme appearance
- "Building package..." progress
**Outputs**: 
- New YTGR theme package created
- Assets embedded with correct structure
**Postconditions**: Theme package valid for Xbox 360
**Errors**:
- Missing required assets → "Background image required"
- Invalid image dimensions → "Background must be 1280x720"
- Write failure → "Could not create theme at [path]"
**Performance**: <10s for typical theme

---

### Create Gamer Picture Pack (Wizard)
**Inputs**: 
- `pictures[]` (64x64 images)
- `packName` (display name)
**Preconditions**: 
- At least one picture provided
- All pictures are 64x64 PNG/JPEG
**UI State**: 
- Picture upload dialog with grid preview
- "Creating pack..." progress
**Outputs**: 
- New STRB gamer picture pack created
- Pictures embedded
**Postconditions**: Pack valid for Xbox 360
**Errors**:
- Invalid dimensions → "All pictures must be 64x64 pixels"
- Unsupported format → "Pictures must be PNG or JPEG"
**Performance**: <5s for typical pack

---

## US5: Profile Management Tools (P2)

### Profile Cleaner (Wizard)
**Inputs**: 
- `profilePackage` (profile to clean)
- `cleaningOptions` (remove unused titles, orphaned data, etc.)
**Preconditions**: 
- Profile is loaded
- User has accepted warning about potential data loss
**UI State**: 
- Multi-step wizard
- Step 1: Scan for cleanable data
- Step 2: Review findings (show what will be removed)
- Step 3: Confirm cleaning
- "Cleaning..." progress with backup creation
**Outputs**: 
- Cleaned profile written (with backup per US3)
- Summary report ("Removed 5 unused game entries, freed 2 KB")
**Postconditions**: Profile is smaller and tidier
**Errors**:
- Backup failure → **ABORT** per US3 policy
- Cleaning would corrupt profile → "Cleaning aborted: profile structure would be invalid"
**Performance**: <10s for typical profile; progress at 0.5s

---

### Profile Creator (Wizard)
**Inputs**: 
- `gamertag` (string, 3-15 chars)
- `profileOptions` (reputation, initial settings)
**Preconditions**: 
- Gamertag is valid format
**UI State**: 
- Profile creation dialog with form fields
- "Creating profile..." progress
**Outputs**: 
- New profile package created
- GPD initialized with default structure
- STFS container created and signed
**Postconditions**: Profile valid and can be opened
**Errors**:
- Invalid gamertag → "Gamertag must be 3-15 alphanumeric characters"
- Write failure → "Could not create profile at [path]"
**Performance**: <10s for profile creation

---

### Transfer Profile to Device
**Inputs**: 
- `profilePackage` (profile to transfer)
- `targetDevice` (Device with FATX partition)
- `transferFlags` (TransferFlags)
**Preconditions**: 
- Device is connected and accessible
- Target partition has sufficient space
**UI State**: 
- Device selection dialog
- "Transferring..." progress with speed/ETA
- Cancel available
**Outputs**: 
- Profile copied to device Content directory
- Transfer log
**Postconditions**: Profile available on Xbox 360
**Errors**:
- Device disconnected → "Device no longer available"
- Insufficient space → "Device needs X MB, has Y MB"
- Write failure → "Could not write to device"
**Performance**: Progress updates at 0.5s; cancellation supported

---

## US6: Device & FATX Tools (P2)

### Scan for Devices
**Inputs**: (automatic on app launch; manual refresh available)
**Preconditions**: None
**UI State**: 
- "Scanning for devices..." progress
- List updates as devices found
**Outputs**: 
- Device list populated with Device entities
- Each device shows partitions, capacity, filesystem type
**Postconditions**: User can select device for operations
**Errors**:
- Platform API failure → "Could not scan devices: [error]" (warn but don't block app)
**Performance**: <5s for device scan

---

### Open Device Partition
**Inputs**: 
- `device` (Device)
- `partition` (Partition)
**Preconditions**: 
- Device is connected
- Partition is FATX-formatted
**UI State**: 
- "Opening device..." busy status
- Elevated permissions prompt if needed (Windows)
**Outputs**: 
- Partition mounted as FATXSource
- Directory tree populated
- User can browse/extract files
**Postconditions**: Partition accessible for read/write operations
**Errors**:
- Permission denied → "Administrator privileges required. Restart as admin?"
- Partition corrupt → "Partition appears corrupted" + show recoverable data
- Device disconnected → "Device no longer available"
**Performance**: <30s for device partition open

---

### Format FATX Partition
**Inputs**: 
- `device` (Device)
- `partition` (Partition)
- `formatOptions` (label, cluster size)
**Preconditions**: 
- User has confirmed destructive operation
- Device is writable
**UI State**: 
- "Formatting partition..." progress
- Warning dialog with checkbox "I understand this will erase all data"
**Outputs**: 
- Partition formatted as FATX
- Empty filesystem structure created
**Postconditions**: Partition is blank and ready for use
**Errors**:
- Device write-protected → "Device is write-protected. Check hardware switch."
- Format failure → "Could not format partition: [error]"
**Performance**: <30s for typical partition; warn if >30s expected

---

### View/Edit Partition Metadata
**Inputs**: `partition` (Partition)
**Preconditions**: Partition is mounted
**UI State**: 
- Partition dialog with editable fields (label, etc.)
- "Saving..." progress on apply
**Outputs**: 
- Partition metadata updated
- Changes visible immediately in device list
**Postconditions**: Partition metadata reflects user changes
**Errors**:
- Invalid label → "Label must be ≤32 characters"
- Write failure → "Could not update partition metadata"

---

## US7: Plugin System (P3)

### Scan for Plugins
**Inputs**: (automatic on app launch; manual refresh available)
**Preconditions**: Plugin directory exists (e.g., `%APPDATA%/Velocity/Plugins/`)
**UI State**: 
- "Scanning plugins..." status in Preferences
**Outputs**: 
- Plugin list populated with Plugin entities
- Each plugin shows name, version, description, author, enabled status
**Postconditions**: User can enable/disable plugins
**Errors**:
- Invalid plugin DLL → "Could not load plugin [name]: [error]" (skip and continue)

---

### Enable Plugin (with Consent)
**Inputs**: `plugin` (Plugin to enable)
**Preconditions**: 
- Plugin is valid
- Plugin.interfaceVersion matches app API version
**UI State**: 
- If first-time: Consent dialog with warning
  - "Plugins have full access to your files. Only enable plugins from trusted sources."
  - Show plugin source, capabilities, author
  - Checkbox: "I understand and trust this plugin"
- "Loading plugin..." progress
**Outputs**: 
- Plugin loaded into app process
- Plugin.consentGiven = true
- Plugin.enabled = true
- Plugin features available (new menu items, format handlers, etc.)
**Postconditions**: Plugin functionality active
**Errors**:
- ABI mismatch → "Plugin was built for API version X, expected Y"
- Plugin init failure → "Plugin initialization failed: [error]"
- User declined consent → Plugin remains disabled
**Performance**: <5s for plugin loading

---

### Disable Plugin
**Inputs**: `plugin` (Plugin to disable)
**Preconditions**: Plugin is enabled
**UI State**: 
- "Unloading plugin..." brief status
**Outputs**: 
- Plugin.enabled = false
- Plugin features removed from UI
**Postconditions**: Plugin no longer active (requires restart for full unload)
**Errors**: (No typical errors; may require restart for clean unload)

---

### Configure Plugin
**Inputs**: 
- `plugin` (Plugin)
- `configOptions` (plugin-specific settings)
**Preconditions**: Plugin is enabled
**UI State**: 
- Plugin-specific configuration dialog
- Settings saved to QSettings under plugin namespace
**Outputs**: 
- Plugin configuration persisted
- Plugin behavior reflects new settings
**Postconditions**: Plugin uses user-configured options
**Errors**:
- Invalid settings → Plugin-specific validation errors

---

## US8: Specialized Formats (P3)

### View Theme Package (YTGR)
**Inputs**: `ytgrPath` (theme package path)
**Preconditions**: Path is valid YTGR file
**UI State**: 
- "Loading theme..." progress
- Theme viewer dialog opens
**Outputs**: 
- Theme assets displayed (background, colors, sounds)
- Theme metadata shown (name, etc.)
- User can preview theme appearance
**Postconditions**: User can view/extract/edit theme
**Errors**:
- Invalid YTGR → "Theme package is corrupted or unsupported"

---

### View Gamer Picture Pack (STRB)
**Inputs**: `strbPath` (gamer picture pack path)
**Preconditions**: Path is valid STRB file
**UI State**: 
- "Loading gamer pictures..." progress
- Picture pack viewer dialog opens
**Outputs**: 
- Grid of gamer pictures displayed (64x64 thumbnails)
- User can select pictures for extraction
**Postconditions**: User can view/extract pictures
**Errors**:
- Invalid STRB → "Gamer picture pack is corrupted"

---

### Import/Export Theme Assets
**Inputs**: 
- Import: `assetFiles[]` (images, sounds from disk)
- Export: `selectedAssets[]` (from loaded theme)
**Preconditions**: 
- Import: Assets meet format requirements
- Export: Theme is loaded
**UI State**: 
- "Importing assets..." / "Exporting assets..." progress
**Outputs**: 
- Import: Assets added to theme package (with backup per US3)
- Export: Assets written to disk
**Postconditions**: Theme updated or assets available on disk
**Errors**:
- Invalid asset format → "Asset must be PNG/JPEG"
- Backup failure (import) → **ABORT** per US3 policy

---

## US9: Utilities (P4)

### Address Converter
**Inputs**: 
- `inputAddress` (hex string)
- `conversionMode` (physical↔virtual, FATX cluster↔offset, etc.)
**Preconditions**: Input is valid hex format
**UI State**: 
- Real-time conversion as user types
**Outputs**: 
- Converted address displayed
- Explanation of conversion shown
**Postconditions**: User understands address mapping
**Errors**:
- Invalid input → "Enter valid hexadecimal address"

---

### View Certificate Details
**Inputs**: `certificate` (Certificate from STFS/SVOD)
**Preconditions**: Certificate is loaded
**UI State**: 
- Certificate dialog with all fields displayed
- Validation status icon (Botan signature check)
**Outputs**: 
- All certificate fields visible (console ID, title ID, dates, signature, etc.)
- Validation indicator shows signature validity
**Postconditions**: User can inspect certificate metadata
**Errors**:
- Invalid signature → Validation indicator shows "Invalid signature"

---

### Download Avatar Assets
**Inputs**: `avatarAssetIds[]` (from profile)
**Preconditions**: 
- Network connection available
- Xbox Live services accessible
**UI State**: 
- "Downloading assets..." progress
- List of assets with download status
**Outputs**: 
- Avatar assets downloaded to cache
- Assets displayed in profile viewer
**Postconditions**: Avatar assets available offline
**Errors**:
- Network failure → "Could not connect to Xbox Live"
- Asset not found → "Asset ID X not found on server"

---

### View GitHub Commits
**Inputs**: (opens GitHub commits dialog)
**Preconditions**: Network connection available
**UI State**: 
- "Fetching commits..." progress
- Commit list populates
**Outputs**: 
- Recent commits displayed with message, author, date
- Links to GitHub for full details
**Postconditions**: User can see project changelog
**Errors**:
- Network failure → "Could not fetch commits"

---

## Cross-Cutting Contracts

### Progress Feedback (FR-024)
**Applies to**: All long-running operations
**Behavior**: 
- If operation >0.5s: Show progress indicator
- Progress bar with percentage and/or time remaining
- Cancel button available at checkpoints
- Status text describing current operation phase

---

### Cancellation Support (FR-026)
**Applies to**: Extraction, device operations, plugin loading, transfers
**Behavior**: 
- "Cancel" button enabled during operation
- Operation checks cancellation flag at safe checkpoints
- On cancel: Clean up partial work, restore to safe state
- User notified of cancellation ("Operation cancelled by user")

---

### Performance Targets (FR-025, per clarification Q4)
**Targets**:
- Source open: <5s (packages), <30s (devices)
- Profile tools: <10s (profile open, cleaner, creator)
- Plugin loading: <5s
- Progress feedback: 0.5s threshold
- Status updates: Every 0.5s during long operations
- Warning: >30s expected duration warned before start

---

### Keyboard Navigation (FR-027)
**Applies to**: All dialogs and forms
**Behavior**: 
- Tab order follows logical flow
- Enter submits forms; Escape cancels dialogs
- Arrow keys navigate lists and trees
- Access keys for primary actions (Alt+O for Open, etc.)

---

### Validation Display
**Applies to**: All data entities
**Behavior**: 
- ValidationIndicator icon with status (checkmark/warning/error)
- Tooltip on hover with brief message
- "View Details" action for full validation report
- Non-blocking (user can proceed with invalid data at own risk)

---

## Error Handling Patterns

### Backup Failure (US3, per clarification Q1)
**Pattern**: **ABORT IMMEDIATELY**
- Never proceed with write operation if backup fails
- Show clear error: "Backup failed: [reason]. Changes NOT saved."
- Suggest fixes: check disk space, permissions, backup location preference

### Network Failures (US9)
**Pattern**: Inform and degrade gracefully
- Show error message: "Network operation failed: [reason]"
- Allow retry
- Offline features remain functional

### Device Disconnection (US6)
**Pattern**: Detect and notify
- Monitor device connection during operations
- If disconnect detected: Cancel operation safely
- Notify user: "Device was disconnected. Operation aborted."

### Permission Denied (cross-cutting)
**Pattern**: Offer remediation
- Windows: "Administrator privileges required. Restart as admin?"
- Provide "Open Backup Folder" or alternative paths
- Suggest checking file permissions

---

## References
- Spec: `specs/001-baseline-capabilities/spec.md`
- Data Model: `specs/001-baseline-capabilities/data-model.md`
- Research: `specs/001-baseline-capabilities/research.md`
