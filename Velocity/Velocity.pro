#-------------------------------------------------
#
# Project created by QtCreator 2012-08-14T18:07:11
#
#-------------------------------------------------

QT       += core gui network xml widgets

CONFIG += c++11
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11

# application
TARGET = Velocity
TEMPLATE = app
win32:UI_DIR = ../Velocity

# application version
VERSION = 0.1.0.0
DEFINES += VERSION=\\\"$$VERSION\\\"

# flags
macx {
    QMAKE_CFLAGS_X86_64 += -mmacosx-version-min=10.7
    QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64
}

# header include path
INCLUDEPATH += $$PWD/../XboxInternals

win32 {
    LIBS += -LC:/botan/ -lbotan-1.10
    INCLUDEPATH += C:/botan/include
}
macx|unix {
    INCLUDEPATH += /usr/local/Cellar/botan/1.10.12/include/botan-1.10
    LIBS += /usr/local/Cellar/botan/1.10.12/lib/libbotan-1.10.1.dylib
}


win32:RC_FILE = velocity.rc
macx:ICON = velocity.icns

CONFIG(debug, debug|release) {
    win32:LIBS += -L$$PWD/../XboxInternals-Win/debug/ -lXboxInternals
    macx:LIBS += -L$$PWD/../XboxInternals-OSX/debug/ -lXboxInternals
    unix:!macx:LIBS += -L$$PWD/../XboxInternals-Linux/debug/ -lXboxInternals
    unix:!macx:PRE_TARGETDEPS += $$PWD/../XboxInternals-Linux/debug/libXboxInternals.a
}
CONFIG(release, debug|release) {
    win32:LIBS += -L$$PWD/../XboxInternals-Win/release/ -lXboxInternals
    macx:LIBS += -L$$PWD/../XboxInternals-OSX/release/ -lXboxInternals
    unix:!macx:LIBS += -L$$PWD/../XboxInternals-Linux/release/ -lXboxInternals
    unix:!macx:PRE_TARGETDEPS += $$PWD/../XboxInternals-Linux/release/libXboxInternals.a
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
    json.cpp \
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
    flashdriveconfigdatadialog.cpp \
    devicecontentviewer.cpp \
    fatxdevicenotifier.cpp \
    profileselectiondialog.cpp \
    isodialog.cpp \
    isosectordialog.cpp

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
    json.h \
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
    flashdriveconfigdatadialog.h \
    devicecontentviewer.h \
    fatxdevicenotifier.h \
    profileselectiondialog.h \
    isodialog.h \
    isosectordialog.h

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
    flashdriveconfigdatadialog.ui \
    devicecontentviewer.ui \
    profileselectiondialog.ui \
    isodialog.ui \
    isosectordialog.ui

RESOURCES += \
    Resources.qrc
