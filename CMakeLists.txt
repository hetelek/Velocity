cmake_minimum_required(VERSION 3.20)

# ==============================
# Velocity - CMake superproject
# ==============================
project(Velocity VERSION 1.0.0 LANGUAGES CXX)

# Enforce C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------
# Options
# ------------------------------
# Library type for XboxInternals
option(BUILD_XBOXINTERNALS_SHARED "Build XboxInternals as a shared library" ON)
option(BUILD_XBOXINTERNALS_STATIC "Also build XboxInternals as a static library" OFF)

# Always avoid any system Botan - we only use the submodule or a fetched copy.
set(BOTAN_USE_SYSTEM OFF CACHE BOOL "Do not use any system Botan" FORCE)

# Botan version control
# Change this default to test newer Botan versions when needed.
set(BOTAN_VERSION "3.9.0" CACHE STRING "Botan version to fetch/use")
# Fallback download directory if submodule is missing; overridable by -DBOTAN_FALLBACK_DIR=...
set(BOTAN_FALLBACK_DIR "${CMAKE_BINARY_DIR}/botan" CACHE PATH "Where to fetch Botan if submodule is absent")
# Preferred local source path (submodule). Keep this path stable at repo root.
set(BOTAN_SUBMODULE_DIR "${CMAKE_SOURCE_DIR}/botan" CACHE PATH "Path to Botan submodule at repo root")

# Qt setup
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# The build expects Qt6 Core and Xml at minimum
find_package(Qt6 6.2 REQUIRED COMPONENTS Core Xml)

# Make cmake/ available
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# -------------------------------------------------
# Find & generate minimal Botan from source (amalgamation)
# Priority:
#   1) Submodule at ${BOTAN_SUBMODULE_DIR} (expected to be tag 3.9.0)
#   2) Download to ${BOTAN_FALLBACK_DIR}
# System Botan is never used.
# -------------------------------------------------
# We pass Botan_PATH into FindBotan.cmake. If the submodule exists, point to it.
if(EXISTS "${BOTAN_SUBMODULE_DIR}/configure.py")
  set(Botan_PATH "${BOTAN_SUBMODULE_DIR}")
else()
  # If submodule is not present, set the fallback dir target where FindBotan.cmake will download into.
  set(Botan_PATH "")  # leave empty -> FindBotan.cmake will fetch
  set(BOTAN_DOWNLOAD_DIR "${BOTAN_FALLBACK_DIR}")
endif()

# Locate Botan "source" (not prebuilt); this module will generate an amalgamated static lib target
# with only the requested modules.
find_package(Botan ${BOTAN_VERSION} REQUIRED)

# Minimal modules required by Velocity/XboxInternals runtime usage.
# Keep this list small and documented. Add entries only if code requires them.
# - sha1,hmac : HMAC(SHA-1) used by XeCrypt flows
# - rc4       : RC4 stream cipher
# - rsa,pubkey,bigint : RSA + bignum usage
# - auto_rng,system_rng : RNGs used via AutoSeeded_RNG/System_RNG
# - sha2_32   : SHA-224/256 used widely in hashing
# - asn1,base64,hex,pem,pk_pad : helper modules commonly needed by RSA key load/PKCS1/PSS/etc
include(cmake/botan_modules.cmake)

# Create an amalgamated botan static library we can link against from our libs.
botan_generate(
  BotanAmalgamation
    ${VELOCITY_BOTAN_MODULES}
)

# ------------------------------
# Subdirectories
# ------------------------------
add_subdirectory(XboxInternals)
add_subdirectory(Velocity)
