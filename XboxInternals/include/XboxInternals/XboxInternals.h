#ifndef XBOXINTERNALS_H
#define XBOXINTERNALS_H

// XboxInternals - Master header for Xbox 360 file format library
// Include this header to access all public APIs

// Export macros
#include <XboxInternals/Export.h>

// Type definitions
#include <XboxInternals/TypeDefinitions.h>

// Account module
#include <XboxInternals/Account/Account.h>
#include <XboxInternals/Account/AccountDefinitions.h>
#include <XboxInternals/Account/AccountHelpers.h>

// Avatar Asset module
#include <XboxInternals/AvatarAsset/AssetHelpers.h>
#include <XboxInternals/AvatarAsset/AvatarAsset.h>
#include <XboxInternals/AvatarAsset/AvatarAssetDefinitions.h>
#include <XboxInternals/AvatarAsset/Ytgr.h>

// Cryptography module
#include <XboxInternals/Cryptography/XeCrypt.h>
#include <XboxInternals/Cryptography/XeKeys.h>

// Disc module
#include <XboxInternals/Disc/Gdfx.h>
#include <XboxInternals/Disc/Svod.h>

// FATX filesystem module
#include <XboxInternals/Fatx/FatxConstants.h>
#include <XboxInternals/Fatx/FatxDrive.h>
#include <XboxInternals/Fatx/FatxDriveDetection.h>
#include <XboxInternals/Fatx/FatxHelpers.h>

// GPD (Game Profile Data) module
#include <XboxInternals/Gpd/AvatarAwardGpd.h>
#include <XboxInternals/Gpd/DashboardGpd.h>
#include <XboxInternals/Gpd/GameGpd.h>
#include <XboxInternals/Gpd/GpdBase.h>
#include <XboxInternals/Gpd/Xdbf.h>
#include <XboxInternals/Gpd/XdbfDefinitions.h>
#include <XboxInternals/Gpd/XdbfHelpers.h>

// I/O abstraction module
#include <XboxInternals/IO/BaseIO.h>
#include <XboxInternals/IO/DeviceIO.h>
#include <XboxInternals/IO/FatxIO.h>
#include <XboxInternals/IO/FileIO.h>
#include <XboxInternals/IO/MemoryIO.h>
#include <XboxInternals/IO/MultiFileIO.h>
#include <XboxInternals/IO/SvodIO.h>
#include <XboxInternals/IO/SvodMultiFileIO.h>

// STFS (Secure Transacted File System) module
#include <XboxInternals/Stfs/StfsConstants.h>
#include <XboxInternals/Stfs/StfsDefinitions.h>
#include <XboxInternals/Stfs/StfsPackage.h>
#include <XboxInternals/Stfs/XContentHeader.h>

#endif // XBOXINTERNALS_H
