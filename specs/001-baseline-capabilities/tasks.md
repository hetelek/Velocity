---
description: "Actionable, dependency-ordered task list for Velocity Baseline Capabilities"
---

# Tasks: Velocity Baseline Capabilities

**Input**: Design documents from `/specs/001-baseline-capabilities/`
**Prerequisites**: plan.md (required), spec.md (required for user stories), research.md, data-model.md, contracts/

**Tests**: Optional. This baseline focuses on manual smoke tests as per the constitution; add Qt Test/GoogleTest items later if requested.

**Organization**: Tasks are grouped by user story to enable independent implementation and testing.

## Format: `[ID] [P?] [Story] Description`
- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (US1, US2, US3)
- Include exact file paths in descriptions

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Ensure environment and presets are ready; no code changes to core.

- [x] T001 Verify CMake preset `windows-mingw-debug` works; ensure `compile_commands.json` exports (no file changes) — ✅ COMPLETE (verified 2025-10-21: preset configures successfully, compile_commands.json generated)
- [x] T002 [P] Validate Qt 6.7.3+ detection (`qmake6 --version`) and Botan availability (no file changes) — ✅ COMPLETE (verified 2025-10-21: Qt 6.8.0 detected, Botan download configured)
- [x] T003 [P] Confirm out/build/<preset>/ is ignored and clean up local artifacts (no file changes) — ✅ COMPLETE (verified 2025-10-21: build directory properly git-ignored)

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Platform abstraction (TOP PRIORITY) + UX guardrails needed by all stories.

**⚠️ Platform Abstraction (Constitution III-A - MUST complete first)**:

- [x] T004 Audit `XboxInternals/` public headers for exposed platform-specific types (Windows handles, POSIX fds, etc.); document in `specs/001-baseline-capabilities/platform-audit.md` — ✅ COMPLETE (commits d8271e6, 2cc9003, 8ba64ed)
- [x] T005 [P] Identify file I/O hotspots in `XboxInternals/` calling platform APIs directly; categorize: replaceable with `std::filesystem` vs. requiring PIMPL — ✅ COMPLETE (commits 9a91471, 83ab1e0, 6d6431e)
- [x] T006 [P] Identify threading/sync hotspots in `XboxInternals/` using platform-specific primitives; categorize: replaceable with `std::thread`/`std::mutex` vs. requiring platform code — ✅ COMPLETE (no threading hotspots requiring changes identified)
- [x] T007 [P] Identify crypto/Botan integration points in `XboxInternals/` exposing platform dependencies — ✅ COMPLETE (Botan integration reviewed, no platform leakage found)
- [x] T008 Replace platform-specific file I/O with `std::filesystem` (paths, directory iteration, file metadata) and `std::fstream` where behavior-equivalent in `XboxInternals/` — ✅ COMPLETE (commits 9a91471 SvodMultiFileIO, 83ab1e0 FatxDrive, 6d6431e FatxDriveDetection, efcfb9d Xdbf)
- [x] T009 Replace platform-specific threading with `std::thread`, `std::mutex`, `std::condition_variable`, `std::atomic` where behavior-equivalent in `XboxInternals/` — ✅ COMPLETE (no threading primitives requiring migration identified)
- [x] T010 For remaining unavoidable platform code: create abstract interfaces with clear contracts in `XboxInternals/` — ✅ COMPLETE (DeviceIO verified, commit 495d07b)
- [x] T011 Implement PIMPL wrappers for unavoidable platform hotspots; move platform code to `*_win.cpp`, `*_posix.cpp` in `XboxInternals/` — ✅ COMPLETE (DeviceIO existing PIMPL confirmed constitution-compliant, commit a792e92)
- [x] T012 Update `XboxInternals/` public headers to remove platform includes; verify `Velocity/` compiles cleanly and smoke test affected flows — ✅ COMPLETE (TypeDefinitions.h reviewed as only legitimate platform conditional, commits a5f75dc, 9c64033)

**UI Foundation**:

- [ ] T013 Add consistent write operation confirmation guidance strings (backup warnings, validation status, confirmation prompts) in `Velocity/` UI components (centralize copy in helper header)
- [ ] T014 [P] Wire long-op progress dialogs for open/extract/export in `Velocity/` (review `multiprogressdialog.*`)
- [ ] T015 [P] Add validation indicator plumbing (verified|failed|unknown with specific reasons) in `Velocity/`
- [ ] T016 Ensure error messages include actionable next steps (permissions, disk full, unsupported variant) in `Velocity/`
- [ ] T017 [P] Implement Help → View Logs dialog with search/filter in `Velocity/`
- [ ] T018 [P] Add "Save Support Bundle" (zip logs) to View Logs dialog in `Velocity/`

Checkpoint: Platform abstraction complete + UI foundation ready → proceed to user stories.

---

## Phase 3: User Story 1 - Browse and extract Xbox content (Priority: P1)

**Goal**: Open FATX/STFS/SVOD/GDFX and extract selections with responsive UI. (Core MVP functionality)
**Independent Test**: Open each format and extract one file with progress and cancel.

- [ ] T019 [US1] Review and document open/browse flow entry points in `Velocity/` (identify dialogs and viewers)
- [ ] T020 [P] [US1] Verify FATX open → tree population; wire progress/cancel paths in `Velocity/deviceviewer.*`
- [ ] T021 [P] [US1] Verify STFS open → metadata + tree; wire progress/cancel paths in `Velocity/packageviewer.*`
- [ ] T022 [P] [US1] Verify SVOD/GDFX open → structure; wire progress/cancel paths in respective viewers in `Velocity/`
- [ ] T023 [US1] Implement or confirm extract selection flow with destination chooser and progress in `Velocity/` (ensure cancel propagates)
- [ ] T024 [US1] Add extraction conflict prompt (Overwrite/Skip/Rename + "Apply to all") in `Velocity/`
- [ ] T025 [US1] Add clear error handling for permission/disk-full/path-conflict during extraction in `Velocity/`
- [ ] T026 [US1] Manual smoke test script: document test procedures and expected outcomes in `specs/001-baseline-capabilities/quickstart.md` (update as needed)
- [ ] T077 [US1] Test drag-and-drop functionality across all supported file types in `Velocity/mainwindow.*` (FR-044)
- [ ] T078 [US1] Test MDI behavior with multiple files open simultaneously in `Velocity/mainwindow.*` (FR-043)

Checkpoint: MVP delivered (read-only open + extract for all formats with MDI and drag-drop support).

---

## Phase 4: User Story 2 - Inspect profiles and achievements (Priority: P2)

**Goal**: View profile and achievement details and export CSV/JSON with specified fields.
**Independent Test**: Open representative GPD, export both CSV and JSON with required fields.

- [ ] T027 [US2] Identify profile/achievement view modules in `Velocity/` (e.g., `profileeditor.*`, GPD viewers)
- [ ] T028 [P] [US2] Ensure achievements table shows fields: titleId, achievementId, name, description, unlocked, timestamp?, gamerscore
- [ ] T029 [P] [US2] Implement Export → CSV in the achievements/profile view (visible fields) in `Velocity/` — Include validation: handle missing optional fields (timestamp), encode special characters, test with empty/incomplete profiles
- [ ] T030 [P] [US2] Implement Export → JSON in the achievements/profile view (full records) in `Velocity/` — Include validation: handle null values, verify JSON structure, test with corrupted profile data
- [ ] T031 [US2] Add user-facing confirmations and success/failure summaries for exports in `Velocity/`

Checkpoint: Achievements/profile inspection and export complete.

---

## Phase 5: User Story 3 - Perform safe edits with explicit confirmation (Priority: P3)

**Goal**: Editing is allowed with mandatory backup-or-abort protection; ensure all write paths create backups and abort on backup failure.
**Independent Test**: Attempt edits, confirm backup created before changes, test that backup failure prevents write.

- [ ] T032 [US3] Audit all write/edit entry points (menus, dialogs, context actions) in `Velocity/` to verify backup-or-abort policy
- [ ] T033 [P] [US3] Ensure all edit handlers create backups before writes and abort with clear messaging on backup failure in `Velocity/`
- [ ] T034 [US3] Add validation that backups are created for all write paths; add defensive checks for any latent code that might bypass backup (constitution violation safeguard) in `Velocity/`

---

## Phase 6: User Story 4 - Create Xbox 360 content from scratch (Priority: P2)

**Goal**: Creation wizards for packages, profiles, themes, and gamer pictures produce valid outputs.
**Independent Test**: Use each wizard to create content and verify it opens in Velocity and Xbox 360.

- [ ] T035 [US4] Review and test Package Creator wizard (`creationwizard.*`) - verify it creates valid STFS packages with metadata
- [ ] T036 [P] [US4] Review and test Profile Creator wizard (`profilecreatorwizard.*`) - verify profile package with GPD files is created
- [ ] T037 [P] [US4] Review and test Theme Creator wizard (`themecreationwizard.*`) - verify theme package with assets and metadata
- [ ] T038 [P] [US4] Review and test Gamer Picture Pack Creator (`gamerpicturepackdialog.*`) - verify gamer picture package creation
- [ ] T039 [US4] Review and test Achievement Creation wizard (`achievementcreationwizard.*`) - verify custom achievements can be added to game GPDs
- [ ] T040 [US4] Ensure all creation wizards have progress feedback, validation, and error handling in `Velocity/`
- [ ] T041 [US4] Add backup mechanisms for any existing files that creation wizards might overwrite in `Velocity/`

Checkpoint: All creation wizards functional and produce valid Xbox 360 content.

---

## Phase 7: User Story 5 - Manage and modify profiles (Priority: P2)

**Goal**: Profile tools (Game Adder, Profile Cleaner, comprehensive Profile Editor) work correctly.
**Independent Test**: Add game to profile, clean data, verify profile remains valid.

- [ ] T042 [US5] Review and test Game Adder dialog (`gameadderdialog.*`) - verify title search and addition to profiles
- [ ] T043 [P] [US5] Review and test Profile Cleaner wizard (`profilecleanerwizard.*`) - verify data category removal with backup
- [ ] T044 [P] [US5] Review and test Profile Editor (`profileeditor.*`) - verify comprehensive profile viewing and editing
- [ ] T045 [US5] Ensure profile tools create backups before modifications in `Velocity/`
- [ ] T046 [US5] Add validation to prevent invalid profile states (duplicate IDs, invalid title IDs) in `Velocity/` — Optional: Validate profile tool performance meets SC-009 targets (complete operations on 100-500 achievements within 10 seconds)

Checkpoint: Profile management tools functional with safety mechanisms.

---

## Phase 8: User Story 6 - Work with device images and raw FATX volumes (Priority: P2)

**Goal**: Device Viewer and FATX utilities provide device-level access.
**Independent Test**: Open device image, browse partitions, generate FATX paths.

- [ ] T047 [US6] Review and test Device Viewer (`deviceviewer.*`) - verify partition display and FATX browsing
- [ ] T048 [P] [US6] Review and test FATX File Path Tool (`fatxpathgendialog.*`) - verify path generation from metadata
- [ ] T049 [P] [US6] Review cluster-level tools (`clustertooldialog.*`) - verify low-level FATX operations with warnings
- [ ] T050 [US6] Ensure device write operations validate FATX integrity and create backups in `XboxInternals/Fatx/`

Checkpoint: Device and FATX tools operational with safety checks.

---

## Phase 9: User Story 7 - Extend functionality with plugins (Priority: P3)

**Goal**: Plugin system loads and executes game/profile modder plugins safely.
**Independent Test**: Load sample plugin, verify menu appearance and execution.

- [ ] T051 [US7] Review plugin loading mechanism in `mainwindow.cpp` and `LoadPlugin()` method
- [ ] T052 [P] [US7] Review plugin interfaces (`PluginInterfaces/igamemodder.h`, `PluginInterfaces/igpdmodder.h`)
- [ ] T053 [US7] Ensure plugin failures are caught and logged without crashing Velocity in `Velocity/mainwindow.*`
- [ ] T054 [US7] Add plugin load progress, **consent dialog with prominent security warning (FR-035)**, and error reporting in `Velocity/` (warning must explain plugins have full memory access and can crash app; require explicit user consent before loading)

Checkpoint: Plugin system stable and extensible.

---

## Phase 10: User Story 8 - Inspect and work with specialized Xbox formats (Priority: P3)

**Goal**: Specialized format dialogs (YTGR, STRB, certificates, security, licensing) function correctly.
**Independent Test**: Open each specialized format/dialog and verify data display.

- [ ] T055 [US8] Review and test YTGR dialog (`ytgrdialog.*`) - verify avatar asset viewing
- [ ] T056 [P] [US8] Review and test STRB dialog (`strbdialog.*`) - verify avatar asset data display
- [ ] T057 [P] [US8] Review and test Certificate dialog (`certificatedialog.*`) - verify cert details and validation status
- [ ] T058 [P] [US8] Review and test Licensing Data dialog (`licensingdatadialog.*`) - verify license info display
- [ ] T059 [P] [US8] Review and test Security Sector dialog (`securitysectordialog.*`) - verify security data display
- [ ] T060 [P] [US8] Review and test Volume Descriptor dialogs (`stfsvolumedescriptordialog.*`, `svodvolumedescriptordialog.*`)
- [ ] T061 [P] [US8] Review and test Transfer Flags dialog (`transferflagsdialog.*`) - verify flag display and editing
- [ ] T062 [P] [US8] Review and test SVOD tool dialogs (`svoddialog.*`, `svodtooldialog.*`, `svodfileinfodialog.*`)
- [ ] T063 [US8] Ensure specialized dialogs have proper error handling and validation in `Velocity/`

Checkpoint: All specialized format tools functional.

---

## Phase 11: User Story 9 - Discover and organize content with utilities (Priority: P4)

**Goal**: Utility tools (Title ID Finder, Address Converter, Preferences) enhance workflows.
**Independent Test**: Use each utility and verify functionality.

- [ ] T064 [US9] Review and test Title ID Finder dialog (`titleidfinderdialog.*`) - verify title search and display
- [ ] T065 [P] [US9] Review and test Address Converter dialog (`addressconverterdialog.*`) - verify address conversion
- [ ] T066 [P] [US9] Review and test Preferences dialog (`preferencesdialog.*`) - verify settings persistence
- [ ] T067 [US9] Review update check mechanism (`on_actionCheck_For_Updates_triggered()` in mainwindow.cpp:855) - verify graceful network failure handling and non-blocking behavior; update check exists but may need error handling improvements per edge cases

Checkpoint: Utilities functional and settings persist.

---

## Phase N: Polish & Cross-Cutting Concerns

- [ ] T068 [P] Audit keyboard nav + focus order for all dialogs/wizards (a11y baseline) in `Velocity/` — Qt framework provides baseline; verify custom dialogs follow standards
- [ ] T069 [P] Verify AA contrast in default theme across all dialogs and document findings in `Velocity/`
- [ ] T070 [P] Document sample file sources and checksums for all formats in `specs/001-baseline-capabilities/quickstart.md` — Builds upon T026 smoke test documentation; focuses on sample file acquisition guidance
- [ ] T071 [P] Update `README.md` with complete feature list and expanded audience summary
- [ ] T072 [P] Add/refresh screenshots for all major features and save to `specs/001-baseline-capabilities/`
- [ ] T073 [P] Audit and remove obsolete read-only enforcement code that contradicts US3-US6 editing features; update any stale UI copy/messages in `Velocity/` — **Note: Current FR-006 (crypto validation) is correct and should NOT be removed; this targets old read-only blocks that prevent editing**
- [ ] T074 [P] Update error dialogs to include "View Logs" button and guidance to export Support Bundle when reporting issues (FR-011) in `Velocity/` — **Required to complete FR-011 integration; must be done alongside T017-T018**
- [ ] T075 [P] Implement backup location preference in Preferences dialog with path picker and validation; add "Open Backup Folder" action to Tools menu; persist via QSettings (FR-039) in `Velocity/preferencesdialog.*` — **Note: Consider moving to Phase 2 or earlier since backup location affects US3 write operations (Phase 5)**
- [ ] T076 Verify Botan artifacts present for runtime crypto features in `out/build/<preset>/` (release checklist)

---

## Dependencies & Execution Order

- Phases must be executed in order. Within phases, tasks marked [P] can proceed in parallel.
- US1 (P1) is MVP baseline and should be validated first.
- US2/US3 follow as core viewing/editing capabilities.
- US4/US5/US6 (P2) can proceed in parallel as they touch different subsystems (creation wizards, profile tools, device tools).
- US7/US8/US9 (P3/P4) are lower priority extensions and utilities.

### Parallel Opportunities

- T002, T003 can run alongside T001
- In Phase 2: T005, T006, T007 can run in parallel; T008-T009 depend on them; T014, T015, T017, T018 can run in parallel with abstraction work
- In US1: T020, T021, T022 can run in parallel; T023 depends on them; T077-T078 can run in parallel after T023
- In US2: T028, T029, T030 can run in parallel; T031 depends on them
- In US3: T033 can proceed in parallel with T032; T034 follows
- In US4: T036, T037, T038, T039 can run in parallel; T040-T041 follow
- In US5: T043, T044 can run in parallel with T042; T045-T046 follow
- In US6: T048, T049 can run in parallel with T047; T050 follows
- In US7: T052 can run in parallel with T051; T053-T054 follow
- In US8: T056-T062 can all run in parallel; T063 follows
- In US9: T065, T066, T067 can run in parallel with T064
- In Phase N: T068-T076 many can run in parallel as they are independent audits/reviews

