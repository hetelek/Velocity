#-------------------------------------------------
#
# Project created by QtCreator 2013-01-30T20:09:47
#
#-------------------------------------------------

QT       -= gui core

TARGET = XboxInternals
TEMPLATE = lib
DEFINES += XBOXINTERNALS_LIBRARY

# flags
QMAKE_CXXFLAGS += -fpermissive

# library version
#VERSION = 0.1.0.0
#DEFINES += VERSION=\\\"$$VERSION\\\"

win32 {
    include(Stfs/Botan.pri)
    LIBS += -LC:/botan/ -lBotan
    INCLUDEPATH += C:/botan/include
}

SOURCES += \
    AvatarAsset/AssetHelpers.cpp \
    GPD/XDBFHelpers.cpp \
    AvatarAsset/AvatarAsset.cpp \
    FileIO.cpp \
    GPD/AvatarAwardGPD.cpp \
    Account/Account.cpp \
    Disc/gdfx.cpp \
    Disc/svod.cpp \
    IO/BaseIO.cpp \
    IO/SvodIO.cpp \
    Stfs/StfsDefinitions.cpp \
    Stfs/StfsPackage.cpp \
    Stfs/XContentHeader.cpp \
    IO/MultiFileIO.cpp \
    Account/AccountHelpers.cpp \
    GPD/DashboardGPD.cpp \
    GPD/GPDBase.cpp \
    GPD/XDBF.cpp \
    GPD/GameGPD.cpp

HEADERS +=\
        XboxInternals_global.h \
    Account/AccountHelpers.h \
    AvatarAsset/AssetHelpers.h \
    AvatarAsset/AvatarAssetDefinintions.h \
    AvatarAsset/AvatarAsset.h \
    GPD/AvatarAwardGPD.h \
    GPD/DashboardGPD.h \
    GPD/XDBFHelpers.h \
    GPD/XDBFDefininitions.h \
    Account/Account.h \
    winnames.h \
    GPD/GPDBase.h \
    GPD/XDBF.h \
    Disc/gdfx.h \
    Disc/svod.h \
    FileIO.h \
    GPD/GameGPD.h \
    IO/BaseIO.h \
    IO/SvodIO.h \
    Stfs/StfsConstants.h \
    Stfs/StfsDefinitions.h \
    Stfs/StfsPackage.h \
    Stfs/XContentHeader.h \
    IO/MultiFileIO.h \
    Account/AccountDefinitions.h
