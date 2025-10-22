# ==============================
# Velocity - CMake superproject
# ==============================
cmake_minimum_required(VERSION 3.20)
project(Velocity VERSION 0.1.0.0 LANGUAGES CXX)

# Export compilation database for clang-tidy / clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

# Ensure MSVC reports correct __cplusplus value for C++20 compliance
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

# ------------------------------
# Options
# ------------------------------
option(BUILD_XBOXINTERNALS_SHARED "Build XboxInternals as a shared library" ON)
option(BUILD_XBOXINTERNALS_STATIC "Also build XboxInternals as a static library" OFF)

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
find_package(Qt6 ${QT_MIN_VERSION} QUIET COMPONENTS Core Xml Widgets Network)

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

message(STATUS "Found Qt6: ${Qt6Core_DIR}")

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

# ------------------------------
# Subdirectories
# ------------------------------
add_subdirectory(XboxInternals)
add_subdirectory(Velocity)