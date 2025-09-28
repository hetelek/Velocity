## FindBotan.cmake
## Based on https://github.com/Tectu/botan-cmake (Unlicense) with small project-specific adjustments:
##  - Prefer a local submodule (Botan_PATH if set to repo's botan/) for version ${Botan_FIND_VERSION}
##  - If not present, download to BOTAN_DOWNLOAD_DIR (default: ${CMAKE_BINARY_DIR}/botan)
##  - Never use system Botan
##  - Expose `botan_generate(<target> <modules...>)` that builds a minimal static amalgamation

cmake_minimum_required(VERSION 3.19)
include(FetchContent)

# Never use system Botan in this project
set(_VELOCITY_BOTAN_DISABLE_SYSTEM TRUE)

# Policy for download timestamps
cmake_policy(SET CMP0135 NEW)

# Python is required by Botan's configure.py
find_package(Python COMPONENTS Interpreter REQUIRED)

# Compose version string
set(Botan_VERSION_STRING "${Botan_FIND_VERSION_MAJOR}.${Botan_FIND_VERSION_MINOR}.${Botan_FIND_VERSION_PATCH}")

# Determine download URL for the exact tag/version
set(_BOTAN_URL "https://github.com/randombit/botan/archive/refs/tags/${Botan_VERSION_STRING}.tar.gz")

# Optional: caller may set Botan_PATH to a local Botan checkout (expected submodule)
# Optional: caller may set BOTAN_DOWNLOAD_DIR; default to binary dir to keep source tree clean
if(NOT DEFINED BOTAN_DOWNLOAD_DIR)
  set(BOTAN_DOWNLOAD_DIR "${CMAKE_BINARY_DIR}/botan")
endif()

# Helper to assert the checkout/tag matches requested version (best-effort)
function(_botan_check_version_from_news OUTVAR SRC_DIR)
  if(EXISTS "${SRC_DIR}/news.rst")
    file(READ "${SRC_DIR}/news.rst" _news)
    string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" _found "${_news}")
    if(_found)
      set(${OUTVAR} "${_found}" PARENT_SCOPE)
      return()
    endif()
  endif()
  set(${OUTVAR} "" PARENT_SCOPE)
endfunction()

# If Botan_PATH points to a local checkout with configure.py, use it.
set(botan_upstream_SOURCE_DIR "")
if(Botan_PATH)
  if(EXISTS "${Botan_PATH}/configure.py")
    set(botan_upstream_SOURCE_DIR "${Botan_PATH}")
    message(STATUS "FindBotan: Using Botan at '${Botan_PATH}'")
  else()
    message(FATAL_ERROR "FindBotan: Botan_PATH was set to '${Botan_PATH}' but configure.py was not found there")
  endif()
endif()

# Otherwise, fetch the exact version tarball into BOTAN_DOWNLOAD_DIR
if(NOT botan_upstream_SOURCE_DIR)
  if(NOT Botan_FIND_VERSION)
    message(FATAL_ERROR "FindBotan: Refusing to download Botan without an explicit version in find_package()")
  endif()

  # Quick check connectivity
  file(DOWNLOAD "${_BOTAN_URL}" "${CMAKE_BINARY_DIR}/_botan_probe.tgz" STATUS _dlstat SHOW_PROGRESS)
  list(GET _dlstat 0 _rc)
  if(NOT _rc EQUAL 0)
    message(FATAL_ERROR "FindBotan: Could not download Botan ${Botan_VERSION_STRING} tarball: ${_BOTAN_URL}")
  endif()
  file(REMOVE "${CMAKE_BINARY_DIR}/_botan_probe.tgz")

  # Fetch into BOTAN_DOWNLOAD_DIR
  set(_DL_DIR "${BOTAN_DOWNLOAD_DIR}")
  file(MAKE_DIRECTORY "${_DL_DIR}")
  FetchContent_Declare(
    botan_upstream
    URL "${_BOTAN_URL}"
    SOURCE_DIR "${_DL_DIR}"
  )
  FetchContent_MakeAvailable(botan_upstream)
  set(botan_upstream_SOURCE_DIR "${_DL_DIR}")
  message(STATUS "FindBotan: Downloaded Botan ${Botan_VERSION_STRING} into '${_DL_DIR}'")
endif()

# Try to validate version
set(_BOTAN_DETECTED_VER "")
_botan_check_version_from_news(_BOTAN_DETECTED_VER "${botan_upstream_SOURCE_DIR}")
if(Botan_VERSION_STRING STREQUAL "..")
  # No explicit version elements provided; accept detected
  if(_BOTAN_DETECTED_VER)
    set(Botan_VERSION_STRING "${_BOTAN_DETECTED_VER}")
  endif()
else()
  if(_BOTAN_DETECTED_VER AND NOT _BOTAN_DETECTED_VER STREQUAL Botan_VERSION_STRING)
    message(FATAL_ERROR "FindBotan: Botan version mismatch: requested ${Botan_VERSION_STRING} but repo seems to be ${_BOTAN_DETECTED_VER}")
  endif()
endif()

# Heavy lifting by CMake
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Botan DEFAULT_MSG Botan_VERSION_STRING)

# ==================
# botan_generate()
# ==================
function(botan_generate TARGET_NAME)
  if(ARGC LESS 2)
    message(FATAL_ERROR "botan_generate(<target> <modules...>) requires at least one module")
  endif()

  # Gather module list
  set(_mods "")
  foreach(i RANGE 1 ${ARGC}-1)
    list(APPEND _mods "${ARGV${i}}")
  endforeach()
  list(JOIN _mods "," _mods_csv)

  # Determine Botan's --cc value from CMAKE_CXX_COMPILER_ID
  set(_cc "${CMAKE_CXX_COMPILER_ID}")
  string(TOLOWER "${_cc}" _cc)
  if(_cc STREQUAL "gnu")
    set(_cc "gcc")
  endif()

  # Generate amalgamation
  add_custom_command(
    OUTPUT botan_all.cpp botan_all.h
    DEPENDS "${botan_upstream_SOURCE_DIR}/configure.py"
    COMMENT "Botan: Generating amalgamation for modules: ${_mods_csv}"
    COMMAND ${Python_EXECUTABLE}
      "${botan_upstream_SOURCE_DIR}/configure.py"
      --quiet
      --cc-bin=${CMAKE_CXX_COMPILER}
      --cc=${_cc}
      $<$<BOOL:${MINGW}>:--os=mingw>
      --disable-shared
      --amalgamation
      --minimized-build
      --enable-modules=${_mods_csv}
  )

  # Create static library from amalgamation
  add_library(${TARGET_NAME} STATIC
    "${CMAKE_CURRENT_BINARY_DIR}/botan_all.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/botan_all.h"
  )

  target_compile_features(${TARGET_NAME} PUBLIC cxx_std_20)

  target_include_directories(${TARGET_NAME}
    PUBLIC
      "${CMAKE_CURRENT_BINARY_DIR}"
  )

  # pthread for non-MSVC platforms
  if(NOT MSVC)
    target_link_libraries(${TARGET_NAME} PRIVATE pthread)
  endif()

  set_target_properties(${TARGET_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endfunction()
