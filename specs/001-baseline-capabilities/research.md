# Research: Velocity Baseline Capabilities

Date: 2025-10-19
Branch: 001-baseline-capabilities
Spec: specs/001-baseline-capabilities/spec.md

## Decisions

### Export formats and fields
- Decision: Support both CSV and JSON exports for achievements/profile views.
- Rationale: CSV enables spreadsheet analysis; JSON captures richer structures.
- Minimum fields: titleId, achievementId, name, description, unlocked (bool), timestamp (optional), gamerscore.
- Alternatives considered: CSV only (simpler but lossy); JSON only (richer but less user-friendly for quick analysis).

### Read-only baseline policy [OBSOLETE]
- Decision: **REMOVED via FR-006 elimination** - Original baseline spec enforced read-only, but expanded spec (US3-US6) requires editing/creation capabilities.
- Historical Rationale: Safety for archivists and casual users; aligns with constitution's security and packaging discipline.
- Superseded By: Backup-or-abort policy (see below); write operations now supported with mandatory backups per FR-017.

### Sample data for validation
- Decision: Curate a local set of representative samples for FATX, STFS, GPD, SVOD/GDFX.
- Rationale: Repeatable smoke testing and demos.
- Alternatives considered: Ad-hoc samples (inconsistent coverage).

### Platform abstraction strategy
- Decision: Apply PIMPL idiom with three-tier preference order (C++20 std lib → Qt → PIMPL for GUI; C++20 std lib → PIMPL for XboxInternals library) per Constitution III-A.
- Rationale: XboxInternals must remain Qt-independent for potential server/CLI reuse; platform-specific code isolated behind stable interfaces; migration to C++20 std lib where possible reduces dependencies.
- Alternatives considered: Qt everywhere (violates module boundaries); full abstraction layer (over-engineering for mature std lib features).
- Implementation: T004-T012 identify and wrap platform-specific APIs in XboxInternals (file I/O, threading, time/date, crypto primitives).

### Backup storage and retention
- Decision: User-configurable backup location (default: `%APPDATA%/Velocity/Backups/` or XDG equivalent); no automatic retention policy (user manages disk space).
- Rationale: User control over sensitive data location; avoids app choosing retention limits that might conflict with user archival needs.
- Alternatives considered: Temp directory (harder to locate for recovery); hardcoded retention (conflicts with archival workflows).
- Implementation: FR-040 exposes backup path preference; UI provides "Open Backup Folder" action.

### Backup failure handling
- Decision: Abort write operation immediately on backup failure; never proceed with edits if backup fails.
- Rationale: Safety-first for archivists; predictable behavior (no silent degradation); aligns with backup-or-abort principle.
- Alternatives considered: Warn and proceed (violates safety guarantee); retry indefinitely (hangs on disk full).
- Implementation: FR-017 requires backup success verification before any write; UI shows clear error message with disk space/permissions guidance.

### Plugin security model
- Decision: In-process plugin loading with explicit consent warning at first load; no sandboxing.
- Rationale: Simpler architecture for trusted-source plugins (community mods); user base expects full filesystem access; sandboxing adds complexity without strong threat model for modding tools.
- Alternatives considered: Out-of-process (high IPC overhead for file ops); web-based (limits plugin capabilities).
- Implementation: FR-030 requires consent dialog with plugin source/capabilities display; FR-031 adds plugin disable option.

### Write operation performance targets
- Decision: No hard performance limits; 0.5s progress feedback threshold; operations cancelable at any checkpoint; warn user if operation would take >30s.
- Rationale: Xbox file formats vary widely in size (64KB profile vs 4GB disc image); hard limits would be arbitrary; user control via cancellation more flexible.
- Alternatives considered: Hard time limits (breaks large operations); no feedback (poor UX for long ops).
- Implementation: FR-025 requires progress indicators at 0.5s; FR-026 ensures cancellation support; pre-operation size estimation warns for >30s expected duration.

## Best Practices

### C++20 Migration Patterns
- Prefer `std::filesystem` over platform-specific file I/O (XboxInternals and Velocity).
- Use `std::jthread` over platform threads; `std::atomic` over mutexes where applicable.
- Replace `std::chrono` platform wrappers with C++20 calendar/timezone types.
- Keep `QFile`/`QDir` in Velocity UI layer for Qt resource system integration, but wrap for testing.

### PIMPL Idiom Application
- Place platform-specific includes in `.cpp` files only; forward-declare impl pointer in headers.
- Use `std::unique_ptr<Impl>` with custom deleter for incomplete types.
- Document abstraction boundaries in header comments (e.g., "Wraps Win32 file handles").
- Test with mock implementations to verify interface stability.

### Concurrency Migration
- Replace Win32/pthreads with `std::jthread` for background tasks (extraction, device scanning).
- Use `QThreadPool`/`QtConcurrent` in Velocity UI for Qt event loop integration.
- Document thread safety in class headers (e.g., "Thread-safe for read operations").
- Add `std::atomic` flags for cancellation support per FR-026.

### Backup Workflow Implementation
- Check available disk space before backup (fail if insufficient).
- Use atomic file operations (write to temp, rename on success).
- Log backup metadata (timestamp, original path, checksum) for recovery UI.
- Provide "Restore from Backup" action in Tools menu.

### Plugin Loading Safety
- Validate plugin interface version before loading (ABI compatibility).
- Catch exceptions during plugin init and report to user.
- Isolate plugin state to prevent cross-plugin interference.
- Document plugin API stability guarantees in developer docs.

## Open Questions (to track, not blocking)
- Location and licensing for sample files (ensure redistribution is allowed or instruct users to obtain their own).
- Exact UX copy for validation status messages, backup warnings, and plugin consent dialogs.

