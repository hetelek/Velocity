#-------------------------------------------------
#
# Project created by QtCreator 2012-08-14T18:07:11
#
#-------------------------------------------------

QT       += core gui network xml widgets

# application
TARGET = Velocity
TEMPLATE = app

CONFIG += c++20

win32:UI_DIR = ../Velocity

# application version
VERSION = 0.1.0.0
DEFINES += VERSION=\\\"$$VERSION\\\"

# linking against botan (and adding to include path)
win32 {
    PLATFORM_BUILD_NAME = Win
    LIBS += -LC:/botan/lib/ -lbotan-3
    INCLUDEPATH += C:/botan/include/botan-3
    RC_FILE = velocity.rc
}
macx {
    PLATFORM_BUILD_NAME = OSX
    INCLUDEPATH += /opt/homebrew/include/botan-3
    LIBS += /opt/homebrew/lib/libbotan-3.a
    QMAKE_INFO_PLIST = Info.plist.app
    ICON = velocity.icns
}
else:unix {
    PLATFORM_BUILD_NAME = Unix
    INCLUDEPATH += /usr/include/botan-3
    LIBS += /usr/lib/libbotan-3.so.0
}

# linking against XboxInternals (and adding to include path)
XBOX_INTERNAL_ROOT = $$PWD/../XboxInternals
INCLUDEPATH += $$XBOX_INTERNAL_ROOT
CONFIG(debug, debug|release) {
    LIBS += -L$$XBOX_INTERNAL_ROOT/build/XboxInternals-$$PLATFORM_BUILD_NAME/debug/ -lXboxInternals
    DESTDIR = $$PWD/build/Velocity-$$PLATFORM_BUILD_NAME/debug/
} else {
    LIBS += -L$$XBOX_INTERNAL_ROOT/build/XboxInternals-$$PLATFORM_BUILD_NAME/release/ -lXboxInternals
    DESTDIR = $$PWD/build/Velocity-$$PLATFORM_BUILD_NAME/release/
}


SOURCES += main.cpp \
    mainwindow.cpp \
    packageviewer.cpp \
    profileeditor.cpp \
    about.cpp \
    metadata.cpp \
    certificatedialog.cpp \
    qthelpers.cpp \
    licensingdatadialog.cpp \
    transferflagsdialog.cpp \
    xdbfdialog.cpp \
    strbdialog.cpp \
    imagedialog.cpp \
    renamedialog.cpp \
    creationwizard.cpp \
    achievementcreationwizard.cpp \
    propertiesdialog.cpp \
    addressconverterdialog.cpp \
    stfstoolsdialog.cpp \
    singleprogressdialog.cpp \
    profilecreatorwizard.cpp \
    themecreationwizard.cpp \
    gameadderdialog.cpp \
    avatarassetdownloader.cpp \
    gpddownloader.cpp \
    titleidfinder.cpp \
    titleidfinderdialog.cpp \
    gamerpicturepackdialog.cpp \
    preferencesdialog.cpp \
    githubcommitsdialog.cpp \
    gpduploader.cpp \
    fatxpathgendialog.cpp \
    profilecleanerwizard.cpp \
    svoddialog.cpp \
    svodfileinfodialog.cpp \
    stfsvolumedescriptordialog.cpp \
    svodtooldialog.cpp \
    svodvolumedescriptordialog.cpp \
    multiprogressdialog.cpp \
    ytgrdialog.cpp \
    deviceviewer.cpp \
    clustertooldialog.cpp \
    fatxfiledialog.cpp \
    partitiondialog.cpp \
    securitysectordialog.cpp \
    nightcharts.cpp \
    dragdroptreewidget.cpp \
    flashdriveconfigdatadialog.cpp

HEADERS  += mainwindow.h \
    packageviewer.h \
    profileeditor.h \
    about.h \
    metadata.h \
    certificatedialog.h \
    qthelpers.h \
    licensingdatadialog.h \
    transferflagsdialog.h \
    xdbfdialog.h \
    strbdialog.h \
    imagedialog.h \
    renamedialog.h \
    achievementcreationwizard.h \
    creationwizard.h \
    propertiesdialog.h \
    addressconverterdialog.h \
    stfstoolsdialog.h \
    singleprogressdialog.h \
    profilecreatorwizard.h \
    themecreationwizard.h \
    gameadderdialog.h \
    avatarassetdownloader.h \
    gpddownloader.h \
    titleidfinder.h \
    titleidfinderdialog.h \
    gamerpicturepackdialog.h \
    preferencesdialog.h \
    githubcommitsdialog.h \
    gpduploader.h \
    fatxpathgendialog.h \
    profilecleanerwizard.h \
    svoddialog.h \
    svodfileinfodialog.h \
    stfsvolumedescriptordialog.h \
    svodtooldialog.h \
    svodvolumedescriptordialog.h \
    multiprogressdialog.h \
    PluginInterfaces/igpdmodder.h \
    PluginInterfaces/igamemodder.h \
    ytgrdialog.h \
    deviceviewer.h \
    clustertooldialog.h \
    fatxfiledialog.h \
    partitiondialog.h \
    securitysectordialog.h \
    nightcharts.h \
    dragdroptreewidget.h \
    flashdriveconfigdatadialog.h

FORMS    += mainwindow.ui \
    packageviewer.ui \
    profileeditor.ui \
    about.ui \
    metadata.ui \
    certificatedialog.ui \
    licensingdatadialog.ui \
    transferflagsdialog.ui \
    xdbfdialog.ui \
    strbdialog.ui \
    imagedialog.ui \
    renamedialog.ui \
    creationwizard.ui \
    achievementcreationwizard.ui \
    propertiesdialog.ui \
    addressconverterdialog.ui \
    stfstoolsdialog.ui \
    singleprogressdialog.ui \
    profilecreatorwizard.ui \
    themecreationwizard.ui \
    gameadderdialog.ui \
    titleidfinderdialog.ui \
    gamerpicturepackdialog.ui \
    preferencesdialog.ui \
    githubcommitsdialog.ui \
    fatxpathgendialog.ui \
    profilecleanerwizard.ui \
    svoddialog.ui \
    svodfileinfodialog.ui \
    svodtooldialog.ui \
    svodvolumedescriptordialog.ui \
    stfsvolumedescriptordialog.ui \
    multiprogressdialog.ui \
    ytgrdialog.ui \
    deviceviewer.ui \
    clustertooldialog.ui \
    fatxfiledialog.ui \
    partitiondialog.ui \
    securitysectordialog.ui \
    flashdriveconfigdatadialog.ui

RESOURCES += \
    Resources.qrc
