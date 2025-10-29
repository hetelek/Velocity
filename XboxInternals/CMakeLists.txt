# XboxInternals library - Xbox 360 file format processing
# Provides: FATX filesystem, STFS packages, GPD files, Account data, Avatar assets, Disc images
#
# License Compliance:
# - XboxInternals: GPL-3.0 (inherits from Velocity project)
# - Botan (embedded): BSD-2-Clause - See ../LICENSE-Botan.txt
#   Botan is statically linked as amalgamation build (botan_all.cpp/botan_all.h)

set(_lib_type "STATIC")
if(BUILD_XBOXINTERNALS_SHARED)
  set(_lib_type "SHARED")
endif()

add_library(XboxInternals ${_lib_type}
  # Source files are in src/ with module subdirectories
  src/Account/Account.cpp
  src/Account/AccountHelpers.cpp
  src/AvatarAsset/AssetHelpers.cpp
  src/AvatarAsset/AvatarAsset.cpp
  src/AvatarAsset/Ytgr.cpp
  src/Cryptography/XeCrypt.cpp
  src/Cryptography/XeKeys.cpp
  src/Disc/Gdfx.cpp
  src/Disc/Svod.cpp
  src/Disc/IsoImage.cpp
  src/Disc/GodImage.cpp
  src/Disc/XexExecutable.cpp
  src/Fatx/FatxDrive.cpp
  src/Fatx/FatxDriveDetection.cpp
  src/Gpd/AvatarAwardGpd.cpp
  src/Gpd/DashboardGpd.cpp
  src/Gpd/GameGpd.cpp
  src/Gpd/GpdBase.cpp
  src/Gpd/Xdbf.cpp
  src/Gpd/XdbfHelpers.cpp
  src/IO/BaseIO.cpp
  src/IO/DeviceIO.cpp
  src/IO/FatxIO.cpp
  src/IO/FileIO.cpp
  src/IO/MemoryIO.cpp
  src/IO/MultiFileIO.cpp
  src/IO/SvodIO.cpp
  src/IO/SvodMultiFileIO.cpp
  src/Stfs/StfsDefinitions.cpp
  src/Stfs/StfsPackage.cpp
  src/Stfs/XContentHeader.cpp
)

# Include directories with generator expressions for proper package support
target_include_directories(XboxInternals
  PUBLIC
    # When building: use include/ for public headers
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    # When installing: headers will be in <prefix>/include/
    $<INSTALL_INTERFACE:include>
  PRIVATE
    # Source files in src/ can access private headers
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Export macro for Windows DLLs
target_compile_definitions(XboxInternals
  PRIVATE
    # Define when building the library to export symbols
    $<$<BOOL:${BUILD_XBOXINTERNALS_SHARED}>:BUILD_XBOXINTERNALS_SHARED>
  INTERFACE
    # Consumers will import symbols when using shared library
    $<$<BOOL:${BUILD_XBOXINTERNALS_SHARED}>:USING_XBOXINTERNALS_SHARED>
)

# Requires C++20
target_compile_features(XboxInternals PUBLIC cxx_std_20)

# Link dependencies with correct visibility:
# - Botan: PUBLIC because botan_all.h is included in StfsPackage.h (public API)
# - Qt: PRIVATE because Qt types are not exposed in public headers (uses std::string, std::vector, etc.)
# - velocity_compiler_flags: PRIVATE to avoid propagating compile options to consumers
target_link_libraries(XboxInternals
  PUBLIC
    BotanAmalgamation  # Required by public headers (StfsPackage.h includes botan_all.h)
  PRIVATE
    Qt6::Core          # Used internally, not exposed in API
    Qt6::Xml           # Used internally, not exposed in API
    velocity_compiler_flags  # Inherit common compile options
)

# Organize in IDE (Visual Studio solution explorer)
set_target_properties(XboxInternals PROPERTIES FOLDER "Libraries")

# Create alias target for namespaced access (modern CMake pattern)
# Allows: target_link_libraries(MyApp PRIVATE XboxInternals::XboxInternals)
add_library(XboxInternals::XboxInternals ALIAS XboxInternals)

# ====================================================================
# INSTALLATION RULES FOR STANDALONE DISTRIBUTION
# ====================================================================
# XboxInternals can be installed as a standalone library package.
# BotanAmalgamation is embedded and installed alongside XboxInternals.
# Consumers need Qt6::Core and Qt6::Xml available on their system.

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install XboxInternals library (shared or static)
install(TARGETS XboxInternals BotanAmalgamation
  EXPORT XboxInternalsTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install public headers preserving directory structure
install(DIRECTORY include/XboxInternals
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.h"
)

# Install LICENSE-Botan.txt alongside the library
install(FILES ${CMAKE_SOURCE_DIR}/LICENSE-Botan.txt
  DESTINATION ${CMAKE_INSTALL_DOCDIR}
)

# Generate and install CMake package config files
configure_package_config_file(
  ${CMAKE_SOURCE_DIR}/cmake/XboxInternalsConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/XboxInternalsConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/XboxInternals
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/XboxInternalsConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

# Install CMake package files
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/XboxInternalsConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/XboxInternalsConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/XboxInternals
)

# Install export targets for find_package() support
install(EXPORT XboxInternalsTargets
  FILE XboxInternalsTargets.cmake
  NAMESPACE XboxInternals::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/XboxInternals
)

# Optional second build as static if requested
# Use case: Link XboxInternals statically when BUILD_XBOXINTERNALS_STATIC=ON
# The main XboxInternals target follows BUILD_XBOXINTERNALS_SHARED setting
if(BUILD_XBOXINTERNALS_STATIC)
  add_library(XboxInternalsStatic STATIC EXCLUDE_FROM_ALL
    $<TARGET_OBJECTS:XboxInternals>
  )
  target_link_libraries(XboxInternalsStatic 
    PUBLIC 
      BotanAmalgamation
    PRIVATE
      Qt6::Core 
      Qt6::Xml 
      velocity_compiler_flags
  )
  target_include_directories(XboxInternalsStatic
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src
  )
  target_compile_features(XboxInternalsStatic PUBLIC cxx_std_20)
  set_target_properties(XboxInternalsStatic PROPERTIES 
    FOLDER "Libraries"
    OUTPUT_NAME XboxInternals_static
  )
  
  # Create alias for static variant
  add_library(XboxInternals::XboxInternalsStatic ALIAS XboxInternalsStatic)
  
  # Install static variant if built
  install(TARGETS XboxInternalsStatic
    EXPORT XboxInternalsTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
endif()

