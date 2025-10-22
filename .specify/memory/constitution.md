# Velocity Constitution

## Core Principles

### I. Superproject & Module Boundaries
Velocity is a CMake superproject. The Qt desktop client lives in `Velocity/`, the shared Xbox file processing library in `XboxInternals/`, and helper macros in `cmake/`. Generated artifacts go under `out/build/<preset>/` and are never committed.

**Critical separation**: `XboxInternals/` is a pure C++ library and MUST NOT depend on Qt or any heavy GUI framework. It may use C++20 standard library and lightweight dependencies (e.g., Botan for crypto). Only `Velocity/` (the GUI client) should link Qt. This ensures `XboxInternals/` remains reusable for CLI tools, servers, or other non-GUI contexts.

Respect boundaries: UI code should consume well-defined library APIs; shared code must remain broadly reusable. Keep public headers free of using-directives.

### II. Reproducible Builds via Presets
All builds use CMake presets. Configure with `windows-mingw-debug` for day-to-day work; use the matching release preset for distributables. Cleaning means pruning `out/build/<preset>/`, not editing sources. Ensure Qt 6.7.3+ auto-detection works; exported `compile_commands.json` supports tooling like clangd.

### III. C++20 with Qt Idioms
Code follows modern C++20 and Qt conventions: four-space indentation; braces on the same line; `CamelCase` class names with `lowerCamelCase` methods. Prefer Qt containers and signals/slots. Wrap QFile operations via helpers in `Velocity/qthelpers.cpp`. Favor clarity and maintainability over cleverness.

### III-A. Platform Abstraction and Implementation Hiding
XboxInternals and shared library code MUST prefer clean, platform-independent C++20 standard library features over platform-specific APIs wherever functionality permits. When platform-specific code is unavoidable:
- Abstract behind well-defined interfaces using the PIMPL (Pointer to Implementation) idiom
- Hide platform dependencies from public headers
- Enable cross-platform builds without leaking OS-specific types
- Reduce compilation dependencies and improve build times
- Facilitate testing via dependency injection

**Preference order for XboxInternals/ (library code)**:
1. **C++20 standard library** (`std::filesystem`, `std::thread`, `std::mutex`, `std::atomic`, `std::chrono`, etc.) — always preferred when adequate
2. **Platform-specific code with PIMPL** (`*_win.cpp`, `*_posix.cpp` behind common interface) — only when C++20 standard library cannot provide required functionality

**Preference order for Velocity/ (GUI code)**:
1. **C++20 standard library** (when adequate)
2. **Qt cross-platform abstractions** (`QFile`, `QThread`, `QMutex`, `QDateTime`, etc.) — preferred for Qt integration and GUI-specific needs
3. **Platform-specific code with PIMPL** — only as last resort

Maintain original functionality during migration; verify behavior equivalence via tests or smoke testing.

### IV. Testing Mindset and Coverage Growth
The tree currently ships without automated coverage; therefore, smoke-test UI flows (FATX, STFS, GPD viewers) after each change. New tests should be introduced using CTest with Qt Test or GoogleTest. Store fixtures beside modules (e.g., `XboxInternals/Fatx/fixtures/`). Name cases `ComponentScenario_ExpectedBehavior`.

### V. Security, Dependencies, and Release Hygiene
CMake can download Botan if the submodule is absent; allow the fetch or preload `botan/`. Keep Botan artifacts with the built app so cryptographic features work; never commit generated `botan_all.cpp` from the build tree. Validate Qt paths (e.g., `qmake6 --version`) for release builds. Treat packaging changes as potentially security-impacting.

## Additional Constraints & Standards

- Platform and Tooling
	- Primary development target is Windows using the provided CMake presets; other platforms should align with the same layout and conventions when added.
	- Qt 6.7.3+ is the baseline. Avoid APIs deprecated in this range without justification.

- Performance & UX Discipline
	- UI must remain responsive; move long-running work off the GUI thread and surface progress via the existing dialogs (e.g., multi-progress patterns already present in the codebase).
	- Favor simple, discoverable UI changes; attach before/after screenshots for any visible tweak.

- Source Organization & Naming
	- Keep module responsibilities crisp. New shared functionality belongs in `XboxInternals/` with clear headers; UI-only helpers stay in `Velocity/`.
	- Follow naming/style rules in this constitution consistently; do not introduce competing conventions.

- Documentation
	- Update `README.md` and/or in-tree comments for any change that affects build, run, or packaging steps.
	- Prefer small, focused commit messages (<=72 characters, present tense) with optional detail in the body.

## Development Workflow, Reviews, and Quality Gates

### Workflow
1. Create a feature branch and implement changes respecting module boundaries.
2. Build with the appropriate CMake preset for your target (debug for day-to-day, release for packaging).
3. Manually smoke-test FATX, STFS, and GPD flows. If tests exist, run `ctest` with the same preset.
4. Update docs/screenshots as needed.

### Pull Requests
- PR description must include: motivation, affected modules, links to issues, and screenshots for UI changes.
- Keep changesets focused; split unrelated changes into separate PRs.
- Reviewers validate: coding style, module boundaries, build instructions, Botan/Qt impacts, and packaging considerations.

### Quality Gates (must pass before merge)
- Build: Project compiles cleanly using the documented preset for the target environment. Build artifacts remain confined to `out/build/<preset>/`.
- Tests: Any existing tests pass; new functionality includes targeted tests where practical. Manual smoke tests are documented for critical flows.
- Style: C++20 + Qt idioms applied; public headers avoid using-directives; naming/format rules respected.
- Security/Packaging: No generated Botan blobs checked in; Qt paths validated for release workflows; crypto features runnable from the expected artifact layout.

## Governance

- This constitution captures the non-negotiable engineering practices for Velocity. Where conflicts arise, it supersedes ad-hoc conventions.
- Amendments require a PR that:
	- Explains motivation and scope
	- Lists impacted modules and processes
	- Provides migration guidance if developers must adjust habits or code
- PR reviewers are accountable for verifying compliance with this constitution. Exceptions must be explicitly documented in the PR and time-bounded.

**Version**: 1.0.0 | **Ratified**: 2025-10-19 | **Last Amended**: 2025-10-19
