# Quickstart & Validation Guide: Velocity Complete Functionality# Quickstart: Velocity Baseline Capabilities



Date: 2025-10-19Date: 2025-10-19

Branch: 001-baseline-capabilitiesBranch: 001-baseline-capabilities

Spec: specs/001-baseline-capabilities/spec.md

## Open and Browse

## Overview- Launch Velocity and choose File → Open.

This guide provides step-by-step validation procedures for all 9 user stories in the Velocity specification. Use these smoke tests after building each phase to ensure functionality works as expected.- Select a source: FATX image, STFS package, GPD/XDBF file, or disc image (SVOD/GDFX).

- Browse the tree; double-click to inspect items. Read-only baseline prevents edits.

---

## Extract Files

## US1: Browse & Extract Files (P1)- Select files/folders, choose Extract.

- Pick a destination; observe progress and cancel control.

### Test: Open and Browse STFS Package- Verify extracted files in the destination folder.

**Steps**:

1. Launch Velocity## Export Achievements/Profile

2. File → Open → Select an STFS package (.pkg, .bin, or similar)- Open a GPD file and navigate to achievements/profile view.

3. Wait for directory tree to populate (should be <5s for typical package)- Choose Export → CSV or JSON.

4. Observe validation indicator (checkmark/warning/error icon)- Confirm exported file contains: titleId, achievementId, name, description, unlocked, timestamp (if present), gamerscore.

5. Click on directories in the tree to browse contents

6. Verify file metadata displays (name, size, modified date)## Troubleshooting

- Corrupt or unsupported source: the app will warn and disable affected actions.

**Expected Results**:- Missing permissions: choose a different destination directory.

- Tree populates successfully- Long operations: progress should appear within ~0.3s; cancel remains available.

- Validation indicator shows appropriate status
- Navigation is responsive
- No crashes or freezes

**Troubleshooting**:
- If package fails to open: Check that file is valid STFS format
- If validation shows error: Click "View Details" to see specific issues

---

### Test: Extract Selection
**Steps**:
1. With an STFS package open, select 2-3 files or folders
2. Right-click → Extract Selection (or menu: Edit → Extract)
3. Choose a destination folder
4. Configure transfer flags if desired (overwrite, timestamps, etc.)
5. Observe progress dialog with per-file progress
6. Wait for extraction to complete
7. Verify files exist at destination with correct contents

**Expected Results**:
- Progress dialog appears within 0.5s if extraction >0.5s
- Cancel button is functional
- All selected files extracted successfully
- Summary notification shows results ("X files extracted")

**Troubleshooting**:
- If permission denied: Choose different destination or run as admin
- If disk full: Free up space or choose different destination

---

### Test: Export Achievements
**Steps**:
1. File → Open → Select a profile package (STFS with GPD)
2. Navigate to Achievements tab
3. Click Export button
4. Choose format: CSV or JSON
5. Select destination path
6. Open exported file in text editor or Excel

**Expected Results**:
- Export completes in <5s for typical profile
- File contains required fields: titleId, achievementId, name, description, unlocked, timestamp, gamerscore
- CSV is valid (opens in Excel without errors)
- JSON is valid (parses without errors)

---

### Test: Validation Indicator
**Steps**:
1. Open a known-good STFS package → Observe checkmark icon
2. Open a corrupt/invalid file → Observe warning/error icon
3. Hover over icon → Verify tooltip shows validation message
4. Click "View Details" (if available) → Verify detailed validation report

**Expected Results**:
- Icons accurately reflect validation status
- Tooltips provide useful information
- Detailed report explains issues clearly

---

## US2: Profile & Achievements Viewer (P2)

### Test: Open Profile Viewer
**Steps**:
1. File → Open → Select profile package
2. Wait for profile to load (<10s)
3. Navigate to Overview tab → Verify gamertag, gamerscore, reputation display
4. Navigate to Achievements tab → Verify achievements list populates
5. Navigate to Game Settings tab → Verify per-title settings display

**Expected Results**:
- All tabs populate with data
- No crashes or errors
- Performance <10s for open

---

### Test: Filter and Sort Achievements
**Steps**:
1. With profile open, go to Achievements tab
2. Use filter dropdown to select "Unlocked Only"
3. Verify list shows only unlocked achievements
4. Sort by "Date Unlocked" descending
5. Verify newest unlocked achievements appear first
6. Search by keyword (e.g., game title)
7. Verify filtered results match search

**Expected Results**:
- Filters apply instantly (no lag)
- Sort order is correct
- Search returns relevant results
- Result count accurate ("Showing X of Y achievements")

---

## US3: Safe Edits with Backup (P3)

### Test: Edit Achievement with Backup
**Steps**:
1. Open profile package
2. Select an achievement in list
3. Click "Edit" button
4. If first-time: Accept warning dialog
5. Change "Unlocked" status or unlock date
6. Click "Save"
7. Observe "Creating backup..." progress
8. Observe "Saving changes..." progress
9. Verify success notification with "Undo" option
10. Check backup folder (%APPDATA%/Velocity/Backups/) → Verify backup exists

**Expected Results**:
- Backup created before any write
- If backup fails, edit aborts with clear error
- Changes saved successfully to original file
- Backup timestamp matches save time
- "Undo" option available in notification

**Troubleshooting**:
- If backup fails: Check disk space, permissions, backup folder path in Preferences

---

### Test: Backup Failure Handling
**Steps**:
1. Configure backup location to read-only folder (e.g., C:\Windows\)
2. Attempt to edit achievement
3. Observe error: "Backup failed: [reason]. Changes NOT saved."
4. Verify original file is unchanged

**Expected Results**:
- Operation aborts immediately on backup failure
- No partial writes to original file
- Clear error message with suggested fixes

---

### Test: Rehash/Resign Package
**Steps**:
1. Modify a profile package (e.g., edit achievement)
2. Save changes (backup created automatically)
3. Observe "Rehashing package..." status
4. Verify package hash/signature updated
5. Close and re-open package → Verify validation passes

**Expected Results**:
- Rehashing completes in <10s
- Package validates as "Valid" after rehash
- Xbox 360 recognizes package (if tested on console)

---

### Test: Restore from Backup
**Steps**:
1. After editing profile, open Tools → Manage Backups
2. Select most recent backup from list
3. Click "Restore"
4. Confirm overwrite warning
5. Wait for restore to complete
6. Re-open profile → Verify changes are reverted

**Expected Results**:
- Backup list shows all backups with timestamps
- Restore completes in <5s
- Original file matches backup exactly

---

## US4: Creation Wizards (P2)

### Test: Create STFS Package
**Steps**:
1. File → New → STFS Package
2. Step 1: Select 2-3 source files from disk
3. Step 2: Fill metadata (displayName, titleId, contentType)
4. Step 3: Configure certificate options (or skip)
5. Step 4: Choose destination path
6. Click "Create"
7. Wait for "Creating package..." to complete
8. Verify new package exists at destination
9. Open new package in Velocity → Verify contents match source files

**Expected Results**:
- Wizard guides through all steps
- Package created successfully in <30s
- Package is valid (validation passes)
- All source files embedded correctly

---

### Test: Create Achievement
**Steps**:
1. Open profile package
2. Tools → Create Achievement
3. Fill form: titleId, name, description, gamerscore
4. Upload achievement icon (optional)
5. Click "Create"
6. Observe backup creation and save
7. Verify new achievement appears in profile viewer

**Expected Results**:
- Achievement created in <5s
- Backup created automatically
- Achievement visible immediately in list
- Profile remains valid after addition

---

### Test: Create Theme Package
**Steps**:
1. File → New → Theme Package
2. Upload background image (1280x720)
3. Upload highlight image (required sizes)
4. Choose background color
5. Add theme sounds (optional)
6. Enter theme name
7. Click "Create"
8. Verify new YTGR package created

**Expected Results**:
- Wizard validates image dimensions
- Theme created in <10s
- Package valid for Xbox 360

---

### Test: Create Gamer Picture Pack
**Steps**:
1. File → New → Gamer Picture Pack
2. Upload 3-5 images (64x64 PNG/JPEG)
3. Enter pack name
4. Click "Create"
5. Verify new STRB package created

**Expected Results**:
- Wizard validates image size (64x64)
- Pack created in <5s
- All pictures embedded correctly

---

## US5: Profile Management Tools (P2)

### Test: Profile Cleaner
**Steps**:
1. Open profile package
2. Tools → Profile Cleaner
3. Step 1: Wait for scan to complete
4. Step 2: Review findings (unused titles, orphaned data)
5. Confirm items to remove
6. Step 3: Click "Clean"
7. Observe backup creation and cleaning progress
8. Review summary report ("Removed X items, freed Y KB")
9. Verify profile still valid and loads correctly

**Expected Results**:
- Scan completes in <10s
- Findings are accurate (manually verify if possible)
- Backup created before cleaning
- Profile remains valid after cleaning

---

### Test: Profile Creator
**Steps**:
1. File → New → Profile
2. Enter gamertag (3-15 chars, alphanumeric)
3. Configure initial settings (reputation, etc.)
4. Choose destination path
5. Click "Create"
6. Wait for profile creation (<10s)
7. Open new profile → Verify valid structure

**Expected Results**:
- Profile created with valid GPD structure
- STFS container properly signed
- Profile loads without errors

---

### Test: Transfer Profile to Device
**Steps**:
1. Connect USB device with FATX partition
2. Open profile package
3. Tools → Transfer to Device
4. Select target device and partition from list
5. Configure transfer flags
6. Click "Transfer"
7. Observe progress with speed/ETA
8. Wait for transfer to complete
9. Verify profile copied to device Content directory

**Expected Results**:
- Device list populates with connected devices
- Progress updates at 0.5s intervals
- Transfer completes successfully
- Profile functional on Xbox 360 (if tested)

---

## US6: Device & FATX Tools (P2)

### Test: Scan for Devices
**Steps**:
1. Connect USB device to computer
2. Launch Velocity (auto-scan on startup)
3. Observe Devices panel → Verify device appears in list
4. Click "Refresh" button → Verify list updates
5. Select device → Verify partitions shown with capacity, filesystem type

**Expected Results**:
- Scan completes in <5s
- Device list accurate (matches OS device manager)
- Partition info correct (capacity, type)

---

### Test: Open Device Partition
**Steps**:
1. Select device in Devices panel
2. Double-click FATX partition
3. If prompted: Grant elevated permissions (Windows UAC)
4. Wait for partition to open (<30s)
5. Browse directory tree
6. Extract files to verify read access

**Expected Results**:
- Partition mounts successfully
- Directory tree populates
- Files extractable
- Performance <30s for open

**Troubleshooting**:
- If permission denied: Restart Velocity as administrator
- If partition corrupt: App shows warning but displays recoverable data

---

### Test: Format FATX Partition
**Steps**:
1. Select device partition
2. Tools → Format Partition
3. Confirm destructive operation warning
4. Enter partition label (optional)
5. Choose cluster size
6. Click "Format"
7. Wait for formatting to complete (<30s)
8. Verify partition is blank and accessible

**Expected Results**:
- Warning dialog requires explicit confirmation
- Formatting completes in <30s
- New partition is empty and valid FATX structure

**⚠️ Caution**: Only test on disposable devices!

---

## US7: Plugin System (P3)

### Test: Scan for Plugins
**Steps**:
1. Place test plugin DLL in `%APPDATA%/Velocity/Plugins/`
2. Launch Velocity (auto-scan on startup)
3. File → Preferences → Plugins tab
4. Verify plugin appears in list with name, version, description, author
5. Click "Refresh" → Verify list updates

**Expected Results**:
- Plugin detected automatically
- Metadata displays correctly
- Invalid plugins skipped with warning

---

### Test: Enable Plugin with Consent
**Steps**:
1. In Preferences → Plugins, select disabled plugin
2. Click "Enable" button
3. Observe consent dialog with warning about file access
4. Review plugin capabilities, source, author
5. Check "I understand and trust this plugin"
6. Click "Accept"
7. Wait for plugin to load (<5s)
8. Verify plugin features available (menu items, format handlers, etc.)

**Expected Results**:
- Consent dialog appears on first enable
- Clear warning about plugin permissions
- Plugin loads in <5s
- Features functional

---

### Test: Disable Plugin
**Steps**:
1. In Preferences → Plugins, select enabled plugin
2. Click "Disable" button
3. Verify plugin features removed from UI
4. Restart Velocity → Verify plugin remains disabled

**Expected Results**:
- Plugin unloads cleanly
- No crashes or errors
- State persists across restarts

---

### Test: Plugin ABI Mismatch
**Steps**:
1. Place plugin with wrong interfaceVersion in Plugins folder
2. Attempt to enable plugin
3. Observe error: "Plugin was built for API version X, expected Y"
4. Verify plugin remains disabled

**Expected Results**:
- App detects version mismatch
- Clear error message
- No crash or corruption

---

## US8: Specialized Formats (P3)

### Test: View Theme Package (YTGR)
**Steps**:
1. File → Open → Select YTGR theme package
2. Wait for theme viewer to open
3. Observe background image, colors, sounds displayed
4. View theme metadata (name, etc.)
5. Right-click asset → Extract to disk

**Expected Results**:
- Theme loads successfully
- All assets displayed correctly
- Extraction works

---

### Test: View Gamer Picture Pack (STRB)
**Steps**:
1. File → Open → Select STRB gamer picture pack
2. Wait for picture pack viewer to open
3. Observe grid of 64x64 picture thumbnails
4. Select pictures → Extract to disk

**Expected Results**:
- Pack loads successfully
- All pictures displayed in grid
- Extraction works

---

### Test: Import/Export Theme Assets
**Steps**:
1. Open YTGR theme package
2. Click "Import Asset" button
3. Select PNG image from disk
4. Verify asset added to theme (with backup created)
5. Click "Export Assets" button
6. Select assets to export
7. Choose destination
8. Verify assets written to disk

**Expected Results**:
- Import validates asset format/dimensions
- Backup created before import
- Export completes successfully

---

## US9: Utilities (P4)

### Test: Address Converter
**Steps**:
1. Tools → Address Converter
2. Enter hex address (e.g., "0x12345678")
3. Select conversion mode (physical↔virtual)
4. Observe converted address updates in real-time
5. Read explanation text

**Expected Results**:
- Conversion is instant (real-time)
- Explanation is clear
- Invalid input shows error

---

### Test: View Certificate Details
**Steps**:
1. Open STFS package with certificate
2. View → Certificate Details
3. Observe all certificate fields (console ID, title ID, dates, signature)
4. Check validation indicator (signature validity via Botan)

**Expected Results**:
- All certificate fields visible
- Validation indicator shows signature status
- Invalid signatures clearly marked

---

### Test: Download Avatar Assets
**Steps**:
1. Open profile with avatar data
2. Tools → Download Avatar Assets
3. Wait for network request to Xbox Live
4. Observe download progress for each asset
5. Verify assets displayed in profile viewer

**Expected Results**:
- Network request succeeds (if online)
- Assets download in reasonable time
- Downloaded assets display correctly

**Troubleshooting**:
- If network failure: App shows error, offline features remain functional

---

### Test: View GitHub Commits
**Steps**:
1. Help → View Recent Changes
2. Wait for commit list to load
3. Observe recent commits with message, author, date
4. Click link → Verify opens GitHub in browser

**Expected Results**:
- Commits load in <5s (if online)
- List is up-to-date
- Links are functional

---

## Cross-Cutting Validation

### Test: Progress Feedback (FR-024)
**Applies to**: All long-running operations
**Steps**:
1. Trigger operation >0.5s (e.g., device open, extraction)
2. Verify progress indicator appears within 0.5s
3. Verify progress bar updates regularly (not frozen)
4. Verify time remaining estimate (if applicable)

**Expected Results**:
- Progress appears at 0.5s threshold
- Updates at least every 0.5s
- Accurate percentage/ETA

---

### Test: Cancellation (FR-026)
**Applies to**: Extraction, device operations, transfers, plugin loading
**Steps**:
1. Start long-running operation
2. Click "Cancel" button mid-operation
3. Verify operation stops at next checkpoint
4. Verify partial work cleaned up
5. Verify notification: "Operation cancelled by user"

**Expected Results**:
- Cancel button responsive
- Operation stops cleanly
- No corrupt files or partial state

---

### Test: Keyboard Navigation (FR-027)
**Applies to**: All dialogs and forms
**Steps**:
1. Open any dialog (e.g., preferences, creation wizard)
2. Press Tab → Verify focus moves logically
3. Press Shift+Tab → Verify reverse navigation
4. Press Enter on focused button → Verify action triggers
5. Press Escape → Verify dialog closes
6. Try Alt+<key> access keys for primary actions

**Expected Results**:
- Tab order is logical
- All actions accessible via keyboard
- No keyboard traps

---

## Troubleshooting Common Issues

### Issue: "Backup failed" error
**Cause**: Disk space, permissions, or invalid backup path
**Solution**: 
- Check available disk space
- Verify backup folder path in Preferences
- Try different backup location
- Run Velocity as administrator (Windows)

### Issue: "Device not found" or permission denied
**Cause**: Insufficient permissions for device access
**Solution**: 
- Windows: Restart Velocity as administrator
- Verify device is connected
- Check USB cable/connection

### Issue: "Plugin initialization failed"
**Cause**: Plugin incompatibility or missing dependencies
**Solution**: 
- Check plugin API version matches Velocity version
- Disable plugin and contact plugin author
- Check for missing DLL dependencies

### Issue: Network operations fail (avatar download, GitHub commits)
**Cause**: No internet connection or firewall blocking
**Solution**: 
- Check network connection
- Verify firewall allows Velocity
- Offline features remain functional

### Issue: Package validation fails
**Cause**: Corrupted file or unsupported format variant
**Solution**: 
- Try opening in read-only mode
- Extract recoverable data if partial corruption
- Contact support with validation details

---

## Performance Checklist

Use this checklist to verify performance targets from FR-025:

- [ ] STFS package open: <5s
- [ ] Device partition open: <30s
- [ ] Profile open: <10s
- [ ] Profile cleaner: <10s
- [ ] Profile creator: <10s
- [ ] Plugin loading: <5s
- [ ] Progress feedback: Appears at 0.5s threshold
- [ ] Progress updates: Every 0.5s during operation
- [ ] No UI freezes >1s (FR-023)
- [ ] >30s operations: Warning shown before start

---

## Validation Sign-Off

After completing all smoke tests, sign off on each user story:

- [ ] **US1: Browse & Extract** - All tests passed
- [ ] **US2: Profile & Achievements** - All tests passed
- [ ] **US3: Safe Edits** - All tests passed, backup-or-abort verified
- [ ] **US4: Creation Wizards** - All tests passed
- [ ] **US5: Profile Management** - All tests passed
- [ ] **US6: Device & FATX Tools** - All tests passed
- [ ] **US7: Plugin System** - All tests passed, consent flow verified
- [ ] **US8: Specialized Formats** - All tests passed
- [ ] **US9: Utilities** - All tests passed
- [ ] **Cross-Cutting** - Progress, cancellation, keyboard nav verified

---

## Next Steps After Validation

Once all smoke tests pass:
1. Commit changes to `001-baseline-capabilities` branch
2. Test on clean Windows install (verify Qt/Botan deployment)
3. Performance profiling for operations near limits
4. Accessibility audit (keyboard nav, screen reader support)
5. Merge to main after review

---

## References
- Spec: `specs/001-baseline-capabilities/spec.md`
- Contracts: `specs/001-baseline-capabilities/contracts/ui-actions.md`
- Data Model: `specs/001-baseline-capabilities/data-model.md`
- Tasks: `specs/001-baseline-capabilities/tasks.md`
