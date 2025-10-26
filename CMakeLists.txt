# ===================================
# Velocity-Next - CMake superproject
# ===================================
# Project structure:
#   cmake/          - CMake modules (FindBotan.cmake, botan_modules.cmake, etc.)
#   Velocity/       - Main GUI application
#   XboxInternals/  - Xbox file format library
#   botan/          - Cryptography library (Git submodule, optional)
#   out/build/      - Generated build outputs (not in source control)
#   specs/          - Feature specifications and documentation
cmake_minimum_required(VERSION 3.20)

# Prevent in-source builds
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "
In-source builds are not allowed!

Please create a separate build directory and run CMake from there:

  cmake -B out/build/your-preset
  cmake --build out/build/your-preset

Or use a preset:

  cmake --preset windows-mingw-release
  cmake --build --preset windows-mingw-release

To clean up, remove: CMakeCache.txt and CMakeFiles/
")
endif()

project(VelocityNext VERSION 0.2.0.0 LANGUAGES CXX)

# Display build configuration for debugging
message(STATUS "Generator: ${CMAKE_GENERATOR}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
if(CMAKE_GENERATOR_PLATFORM)
    message(STATUS "Platform: ${CMAKE_GENERATOR_PLATFORM}")
endif()

# Export compilation database for clang-tidy / clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use folders in IDE (Visual Studio) for cleaner grouping
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Build System Strategy:
# - All presets use the Ninja generator (including MSVC) for consistency across platforms
# - This provides faster builds and uniform behavior between MinGW/MSVC/GCC/Clang
# - If you prefer the Visual Studio generator for IDE integration:
#   * Use: cmake -G "Visual Studio 17 2022" -A x64 -S . -B build
#   * Note: VS generator uses multi-config (Debug/Release in same tree)
#   * Presets use single-config (separate directories per build type)

# Platform-specific settings
if(APPLE)
    set(PLATFORM_BUILD_NAME "OSX" CACHE STRING "Platform build name")
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_BUILD_NAME "Unix" CACHE STRING "Platform build name") 
elseif(WIN32)
    set(PLATFORM_BUILD_NAME "Win" CACHE STRING "Platform build name")
endif()

# Enforce C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Create an INTERFACE library to hold common compile options
add_library(velocity_compiler_flags INTERFACE)

# Ensure MSVC reports correct __cplusplus value for C++20 compliance
if(MSVC)
    target_compile_options(velocity_compiler_flags INTERFACE /Zc:__cplusplus)
endif()

# All Velocity targets (XboxInternals, Velocity) must link against velocity_compiler_flags
# to inherit common compile options (e.g., MSVC /Zc:__cplusplus for C++20 compliance)
# Example: target_link_libraries(MyTarget PRIVATE velocity_compiler_flags)

# ------------------------------
# Options
# ------------------------------
# XboxInternals library build configuration:
# - Default: BUILD_XBOXINTERNALS_SHARED=ON (shared library only)
# - Both ON: Builds both shared and static variants
# - Both OFF: Automatically enables STATIC to ensure library is built
option(BUILD_XBOXINTERNALS_SHARED "Build XboxInternals as a shared library" ON)
option(BUILD_XBOXINTERNALS_STATIC "Also build XboxInternals as a static library" OFF)

# Ensure at least one library type is built
if(NOT BUILD_XBOXINTERNALS_SHARED AND NOT BUILD_XBOXINTERNALS_STATIC)
    message(STATUS "Neither SHARED nor STATIC selected for XboxInternals; defaulting to STATIC")
    set(BUILD_XBOXINTERNALS_STATIC ON)
endif()

# Botan version control
set(BOTAN_VERSION "3.9.0" CACHE STRING "Botan version to fetch/use")

# ====================================================================
# BOTAN DEPENDENCY MANAGEMENT
# ====================================================================
# 
# Botan (cryptography library) is handled via two possible sources:
#
# 1. GIT SUBMODULE (Developer/CI workflow):
#    - Location: ${CMAKE_SOURCE_DIR}/botan/
#    - Used when: The submodule is initialized and contains configure.py
#    - Benefits: Fast, version-controlled, no download needed
#
# 2. AUTOMATIC DOWNLOAD (End-user workflow):
#    - Location: ${CMAKE_BINARY_DIR}/botan/
#    - Used when: Submodule is not present (fresh clone without --recurse-submodules)
#    - Benefits: No manual steps for end users
#
# The FindBotan.cmake module automatically chooses between these options.
# ====================================================================

# Primary location for Botan Git submodule (if available)
# This is the preferred source for developers and CI systems
set(BOTAN_SUBMODULE_DIR "${CMAKE_SOURCE_DIR}/botan" 
    CACHE PATH 
    "Path to Botan Git submodule at repository root. Used when submodule is initialized."
)

# Fallback download location for Botan source
# Used when submodule is not available (end-user builds)
set(BOTAN_FALLBACK_DIR "${CMAKE_BINARY_DIR}/botan" 
    CACHE PATH 
    "Where to download Botan tarball if Git submodule is absent. Used for end-user builds."
)

# Qt setup
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ------------------------------
# Qt Discovery
# ------------------------------
set(QT_MIN_VERSION "6.7.3")

# Primary method: Use CMake's built-in Qt discovery
find_package(Qt6 ${QT_MIN_VERSION} COMPONENTS Core Xml Widgets Network)

if(NOT Qt6_FOUND)
    # Secondary method: Try qmake-based discovery
    find_program(QMAKE_EXECUTABLE NAMES qmake6 qmake-qt6 qmake)
    if(QMAKE_EXECUTABLE)
        message(STATUS "Qt6 not found via CMake, trying qmake discovery...")
        execute_process(
            COMMAND ${QMAKE_EXECUTABLE} -query QT_INSTALL_PREFIX
            OUTPUT_VARIABLE QT_PREFIX
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(QT_PREFIX AND EXISTS "${QT_PREFIX}")
            message(STATUS "Found Qt via qmake at: ${QT_PREFIX}")
            set(CMAKE_PREFIX_PATH ${QT_PREFIX} ${CMAKE_PREFIX_PATH})
            find_package(Qt6 ${QT_MIN_VERSION} QUIET COMPONENTS Core Xml Widgets Network)
        endif()
    endif()
endif()

if(NOT Qt6_FOUND)
    # Final method: Check common environment variables
    if(DEFINED ENV{QT6_PREFIX_PATH})
        message(STATUS "Trying QT6_PREFIX_PATH environment variable: $ENV{QT6_PREFIX_PATH}")
        set(CMAKE_PREFIX_PATH $ENV{QT6_PREFIX_PATH} ${CMAKE_PREFIX_PATH})
        find_package(Qt6 ${QT_MIN_VERSION} QUIET COMPONENTS Core Xml Widgets Network)
    endif()
endif()

if(NOT Qt6_FOUND)
    message(FATAL_ERROR "
Qt6 not found! Please install Qt6 (version ${QT_MIN_VERSION} or higher).

Detection methods tried:
1. CMake's built-in Qt discovery
2. qmake query (qmake6, qmake-qt6, qmake)
3. QT6_PREFIX_PATH environment variable

Solutions:
- Install Qt6 from https://www.qt.io/download
- Ensure Qt6 binaries are in your PATH
- Set QT6_PREFIX_PATH environment variable
- Set CMAKE_PREFIX_PATH: cmake -B build -DCMAKE_PREFIX_PATH=/path/to/qt
")
endif()

message(STATUS "Found Qt6 ${Qt6Core_VERSION} at: ${Qt6Core_DIR}")

# Make cmake/ available
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# ====================================================================
# BOTAN INTEGRATION
# ====================================================================
# The FindBotan.cmake module will:
# 1. Check if BOTAN_SUBMODULE_DIR exists and contains configure.py
# 2. If yes: Use the submodule as Botan source
# 3. If no:  Download Botan ${BOTAN_VERSION} to BOTAN_FALLBACK_DIR
# 4. Generate amalgamated build (botan_all.cpp/h) with only required modules
# ====================================================================

if(EXISTS "${BOTAN_SUBMODULE_DIR}/configure.py")
  message(STATUS "Using Botan submodule at: ${BOTAN_SUBMODULE_DIR}")
  set(Botan_PATH "${BOTAN_SUBMODULE_DIR}")
else()
  message(STATUS "Botan submodule not found, will download to: ${BOTAN_FALLBACK_DIR}")
  set(Botan_PATH "")  # Empty triggers download in FindBotan.cmake
  set(BOTAN_DOWNLOAD_DIR "${BOTAN_FALLBACK_DIR}")
endif()

# Locate Botan source and generate amalgamation
find_package(Botan ${BOTAN_VERSION} REQUIRED)

# Minimal modules required by Velocity/XboxInternals
include(cmake/botan_modules.cmake)

# Create an amalgamated botan static library for ALL platforms
botan_generate(
  BotanAmalgamation
    ${VELOCITY_BOTAN_MODULES}
)

# Install Botan amalgamation header for standalone XboxInternals distribution
# This allows consumers to use XboxInternals without rebuilding Botan
include(GNUInstallDirs)
install(FILES ${CMAKE_BINARY_DIR}/botan_all.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/botan
  OPTIONAL  # File might not exist until build time
)

# ------------------------------
# Subdirectories
# ------------------------------
add_subdirectory(XboxInternals)
add_subdirectory(Velocity)