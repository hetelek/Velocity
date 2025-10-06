# Changelog

## Velocity Setup v1.1 - 2025-09-28
- CMake superproject for Velocity 1.0.0
- Integrated Botan 3.9.0 amalgamation build (no system Botan ever used)
- Minimal Botan modules list consolidated
- XboxInternals defaults to SHARED library, optional STATIC
- Qt6 Core + Xml required
- Added CMakePresets for MinGW & MSVC
- Added .gitmodules (botan submodule on release-3 branch; recommend tag 3.9.0)
