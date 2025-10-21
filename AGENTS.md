# Repository Guidelines
## Project Structure & Module Organization
Velocity is a CMake superproject: the Qt desktop client lives in `Velocity/`, the shared Xbox file processing library in `XboxInternals/`, and helper macros in `cmake/`. Generated builds land under `out/build/<preset>/`, so clean artifacts by pruning that directory rather than the sources.

## Build, Test, and Development Commands
- `cmake --preset windows-mingw-debug` configures a debug build with Qt 6.7.3+ auto-detection and exports `compile_commands.json` for clangd.
- `cmake --build --preset windows-mingw-debug` compiles Velocity and XboxInternals together; use the matching release preset for distributables.
- `ctest --preset windows-mingw-debug` is available once you add tests to CMake; today it returns immediately because no suites are registered.
- `./out/build/windows-mingw-debug/Velocity.exe` launches the debug UI; keep Botan artifacts in place so cryptographic features work.

## Coding Style & Naming Conventions
Source is modern C++20 with Qt idioms: four-space indentation, braces on the same line, and `CamelCase` class names with lowerCamelCase methods. Prefer Qt containers and signals/slots, wrap QFile operations in helpers from `Velocity/qthelpers.cpp`, and keep public headers free of using-directives.

## Testing Guidelines
The tree currently ships without automated coverage, so smoke-test UI flows that touch FATX, STFS, and GPD viewers after each change. When introducing tests, wire them through CTest using Qt Test or GoogleTest, store fixtures beside the module (`XboxInternals/Fatx/fixtures/`), and name cases `ComponentScenario_ExpectedBehavior`.

## Commit & Pull Request Guidelines
History favors short summaries (`added CMAKE_EXPORT_COMPILE_COMMANDS ON`) and GitHub's merge auto-messages; follow suit with a present-tense, <=72-character subject and expanded detail in the body as needed. PRs should describe the motivation, list affected modules, link to issues, and attach before/after screenshots for UI tweaks; draft checklists help reviewers verify Botan, Qt kit, and packaging impacts.

## Security & Configuration Tips
CMake can download Botan when the submodule is absent; ensure your environment allows that fetch or preload `botan/`. Guard release artifacts by validating Qt paths via `qmake6 --version`, and never commit generated `botan_all.cpp` blobs from the build tree.
