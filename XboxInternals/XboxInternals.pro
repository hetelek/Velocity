#-------------------------------------------------
#
# Project created by QtCreator 2013-01-30T20:09:47
#
#-------------------------------------------------

QT       -= gui core

TARGET = XboxInternals
TEMPLATE = lib
DEFINES += XBOXINTERNALS_LIBRARY

unix {
    CONFIG += staticlib app_bundle
}

# flags
#QMAKE_CXXFLAGS += -fpermissive
QMAKE_CFLAGS_RELEASE = -O3

# library version
#VERSION = 0.1.0.0
#DEFINES += VERSION=\\\"$$VERSION\\\"

win32 {
    include(Stfs/Botan.pri)
    LIBS += C:/botan/libbotan-1.10.a
    PRE_TARGETDEPS += C:/botan/libbotan-1.10.a
    INCLUDEPATH += C:/botan/include
}
macx|unix {
    INCLUDEPATH += /usr/local/include/botan-1.10
    LIBS += /usr/local/lib/libbotan-1.10.a
}

SOURCES += \
    AvatarAsset/AssetHelpers.cpp \
    Gpd/XdbfHelpers.cpp \
    AvatarAsset/AvatarAsset.cpp \
    Gpd/AvatarAwardGpd.cpp \
    Account/Account.cpp \
    Disc/Gdfx.cpp \
    Disc/Svod.cpp \
    IO/BaseIO.cpp \
    IO/SvodIO.cpp \
    Stfs/StfsDefinitions.cpp \
    Stfs/StfsPackage.cpp \
    Stfs/XContentHeader.cpp \
    Account/AccountHelpers.cpp \
    Gpd/DashboardGpd.cpp \
    Gpd/GpdBase.cpp \
    Gpd/Xdbf.cpp \
    Gpd/GameGpd.cpp \
    IO/MemoryIO.cpp \
    IO/DeviceIO.cpp \
    Cryptography/XeKeys.cpp \
    Cryptography/XeCrypt.cpp \
    AvatarAsset/Ytgr.cpp \
    Fatx/FatxHelpers.cpp \
    Fatx/FatxDrive.cpp \
    IO/FileIO.cpp \
    IO/FatxIO.cpp \
    Fatx/FatxDriveDetection.cpp \
    IO/SvodMultiFileIO.cpp \
    IO/MultiFileIO.cpp

HEADERS +=\
        XboxInternals_global.h \
    Account/AccountHelpers.h \
    AvatarAsset/AssetHelpers.h \
    AvatarAsset/AvatarAssetDefinintions.h \
    AvatarAsset/AvatarAsset.h \
    Gpd/AvatarAwardGpd.h \
    Gpd/DashboardGpd.h \
    Gpd/XdbfHelpers.h \
    Gpd/XdbfDefininitions.h \
    Account/Account.h \
    winnames.h \
    Gpd/GpdBase.h \
    Gpd/Xdbf.h \
    Disc/Gdfx.h \
    Disc/Svod.h \
    Gpd/GameGpd.h \
    IO/BaseIO.h \
    IO/SvodIO.h \
    Stfs/StfsConstants.h \
    Stfs/StfsDefinitions.h \
    Stfs/StfsPackage.h \
    Stfs/XContentHeader.h \
    Account/AccountDefinitions.h \
    IO/MemoryIO.h \
    IO/DeviceIO.h \
    Cryptography/XeKeys.h \
    Cryptography/XeCrypt.h \
    AvatarAsset/Ytgr.h \
    Fatx/FatxHelpers.h \
    Fatx/FatxDrive.h \
    Fatx/FatxConstants.h \
    IO/FileIO.h \
    IO/FatxIO.h \
    Fatx/FatxDriveDetection.h \
    IO/SvodMultiFileIO.h \
    IO/MultiFileIO.h
