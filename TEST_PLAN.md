# ISO/GOD Support Testing Plan

## Build: 003-iso-xex-support
**Date:** 2025-10-27

---

## ✅ Pre-Testing: Build Verification

- [x] Code compiles without errors
- [x] Code compiles without warnings  
- [x] VelocityNext.exe created successfully
- [x] XboxInternals.dll exports ISO symbols correctly

---

## 🔧 Test 1: Application Launch

**Steps:**
```powershell
.\out\build\windows-mingw-debug\Velocity\VelocityNext.exe
```

**Expected Results:**
- [ ] Application launches without crashing
- [ ] Main window appears
- [ ] No error dialogs on startup
- [ ] Menus are responsive

**Status:** ⏳ PENDING

---

## 📂 Test 2: ISO/GOD Menu Integration

**Steps:**
1. Launch VelocityNext
2. Click **Tools** menu → **Device Tools**
3. Look for **ISO / GOD Viewer** menu item

**Expected Results:**
- [ ] "ISO / GOD Viewer" menu item exists
- [ ] Menu item is enabled (not grayed out)
- [ ] Clicking it opens file browser dialog

**Status:** ⏳ PENDING

---

## 📄 Test 3: ISO File Dialog

**Steps:**
1. Tools → Device Tools → ISO / GOD Viewer
2. Click "Open File" button or use file browser
3. Navigate to a test ISO file (if you have one)
4. Select the ISO file

**Expected Results:**
- [ ] File browser shows "Xbox 360 Images (*.iso *.000)" filter
- [ ] Can browse to ISO file location
- [ ] IsoDialog window opens in MDI area
- [ ] Dialog doesn't crash on file selection

**Test Files Needed:**
- [ ] Xbox 360 ISO file (GDF format)
- [ ] Xbox 360 ISO file (XGD3 format) 
- [ ] GOD package (.000 file from Data0000 file)

**Status:** ⏳ PENDING

---

## 📊 Test 4: ISO Contents Display

**Prerequisites:** Valid Xbox 360 ISO file

**Steps:**
1. Open an ISO file via ISO / GOD Viewer
2. Observe the tree view widget

**Expected Results:**
- [ ] Tree view populates with file/folder structure
- [ ] Folders shown with folder icon
- [ ] Files shown with file icon
- [ ] File sizes displayed correctly
- [ ] Full directory tree is traversable
- [ ] No missing files or folders

**Known Limitations:**
- Stub implementation may show empty tree if parsing isn't complete

**Status:** ⏳ PENDING

---

## 🔍 Test 5: File Preview

**Prerequisites:** ISO file loaded with text files

**Steps:**
1. Load ISO file
2. Double-click on a text file (e.g., .txt, .xml, .cfg, .ini)
3. Check preview pane

**Expected Results:**
- [ ] Preview pane shows file content
- [ ] Text is readable and formatted
- [ ] Non-text files show "Binary file" or similar message
- [ ] Preview updates when different files are clicked

**Test Files:**
- Look for `default.xex.xml`, `.cfg` files, or readme files in ISO

**Status:** ⏳ PENDING

---

## 💾 Test 6: File Extraction (Single)

**Prerequisites:** ISO file loaded

**Steps:**
1. Select a single file in tree view
2. Click "Extract Selected" button
3. Choose output directory
4. Wait for extraction

**Expected Results:**
- [ ] File browser prompts for output location
- [ ] Selected file extracts to chosen directory
- [ ] File size matches original
- [ ] File is readable/valid
- [ ] Success message appears
- [ ] No crashes or errors

**Status:** ⏳ PENDING

---

## 📦 Test 7: Batch Extraction (All Files)

**Prerequisites:** ISO file loaded

**Steps:**
1. Click "Extract All" button
2. Choose output directory  
3. Wait for extraction

**Expected Results:**
- [ ] All files extract with full directory structure
- [ ] Nested folders created correctly
- [ ] File sizes match originals
- [ ] Progress indicator works (if implemented)
- [ ] Success message after completion
- [ ] No missing files

**Status:** ⏳ PENDING

---

## 🎮 Test 8: XEX Metadata Parsing

**Prerequisites:** ISO with default.xex file

**Steps:**
1. Load ISO file
2. Click "Extract All"
3. Check console output or UI for Title ID detection

**Expected Results:**
- [ ] Title ID extracted from default.xex filename
- [ ] Title ID displayed correctly (8-character hex)
- [ ] Game name parsed if available
- [ ] No crashes during XEX parsing

**Example Title IDs:**
- `4D530001` - Halo 3
- `4D53082B` - Halo Reach
- `4D530919` - Halo 4

**Status:** ⏳ PENDING

---

## 🗜️ Test 9: GOD Package Support

**Prerequisites:** Xbox 360 GOD package (Data0000, Data0001 files)

**Steps:**
1. Tools → Device Tools → ISO / GOD Viewer
2. Select a `.000` file (Data0000 from GOD package)
3. Load the package

**Expected Results:**
- [ ] GOD package opens without errors
- [ ] File tree displays (if implemented)
- [ ] Block remapping works correctly
- [ ] Can extract files from GOD package

**Known Limitations:**
- GOD support may be partial/stub implementation
- Hash table validation may not be implemented

**Status:** ⏳ PENDING

---

## 🔌 Test 10: Plugin System Integration

**Steps:**
1. Launch VelocityNext
2. Click Preferences
3. Check "PluginPath" setting
4. Navigate to plugin directory (default: `./plugins`)

**Expected Results:**
- [ ] Plugin path setting exists
- [ ] Plugin directory is accessible
- [ ] No errors when loading plugin directory
- [ ] Tools menu is accessible for future plugins

**Status:** ⏳ PENDING

---

## 🎯 Test 11: Drag & Drop Support

**Prerequisites:** VelocityNext running, ISO file available

**Steps:**
1. Open ISO / GOD Viewer dialog
2. Drag an .iso file from Windows Explorer
3. Drop onto IsoDialog window

**Expected Results:**
- [ ] Drag operation is recognized (cursor changes)
- [ ] Drop loads the ISO file
- [ ] File tree populates correctly
- [ ] Same behavior as using "Open File" button

**Status:** ⏳ PENDING

---

## 🛑 Test 12: Error Handling

### Test 12a: Invalid File
**Steps:**
1. Try to open a non-ISO file (e.g., .txt, .jpg)

**Expected Results:**
- [ ] Error message displayed
- [ ] Application doesn't crash
- [ ] Can try again with different file

### Test 12b: Corrupted ISO
**Steps:**
1. Try to open corrupted/truncated ISO

**Expected Results:**
- [ ] Graceful error message
- [ ] No crash or hang
- [ ] Clear explanation of issue

### Test 12c: Missing File
**Steps:**
1. Start extraction
2. Delete output directory during extraction

**Expected Results:**
- [ ] Error message displayed
- [ ] Extraction stops gracefully
- [ ] No data corruption

**Status:** ⏳ PENDING

---

## 🚀 Test 13: Performance & Stress

### Test 13a: Large ISO
**Steps:**
1. Open large ISO file (4GB+ XGD3 image)

**Expected Results:**
- [ ] Opens within reasonable time (<10 seconds)
- [ ] UI remains responsive during load
- [ ] Memory usage is reasonable

### Test 13b: Many Files
**Steps:**
1. Extract ISO with thousands of files

**Expected Results:**
- [ ] Extraction completes successfully
- [ ] Progress updates regularly
- [ ] Can cancel extraction if needed

**Status:** ⏳ PENDING

---

## 📝 Test 14: Multiple Instances

**Steps:**
1. Open first ISO file
2. Tools → Device Tools → ISO / GOD Viewer again
3. Open second ISO file

**Expected Results:**
- [ ] Two separate ISO dialogs open
- [ ] Each shows different ISO contents
- [ ] No cross-contamination of data
- [ ] Can work with both simultaneously

**Status:** ⏳ PENDING

---

## 🔄 Test 15: Memory Leaks

**Steps:**
1. Open and close ISO files repeatedly (10+ times)
2. Monitor memory usage in Task Manager

**Expected Results:**
- [ ] Memory usage stabilizes
- [ ] No continuous memory growth
- [ ] Application remains responsive

**Status:** ⏳ PENDING

---

## 📋 Test Results Summary

### Critical Issues Found:
- [ ] None yet - testing not started

### Non-Critical Issues:
- [ ] None yet - testing not started

### Known Limitations (Expected):
- GOD hash table validation not implemented
- ISO creation methods are stubs
- GOD creation methods are stubs
- Batch plugin system ready but no plugins exist yet

---

## 🎬 Getting Started

**To begin testing:**

1. **Prepare test files:**
   - Download or locate Xbox 360 ISO files
   - Locate GOD packages if available
   - Create a test directory: `C:\Temp\IsoTest\`

2. **Launch the application:**
   ```powershell
   cd C:\Users\holvo\Documents\gitkraken\Velocity
   .\out\build\windows-mingw-debug\Velocity\VelocityNext.exe
   ```

3. **Start with Test 1** and work through sequentially

4. **Document results** by checking boxes and noting any issues

---

## 🐛 Bug Report Template

**If you find a bug, note:**
- Test number and name
- Steps to reproduce
- Expected vs actual behavior  
- Error messages (if any)
- Crash details (if applicable)
- System info (Windows version, Qt version)

---

**Happy Testing! 🎮**
