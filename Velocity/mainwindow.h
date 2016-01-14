#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <vector>

// qt
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFile>
#include <QPluginLoader>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>

// forms
#include "profileeditor.h"
#include "about.h"
#include "packageviewer.h"
#include "xdbfdialog.h"
#include "strbdialog.h"
#include "profileeditor.h"
#include "creationwizard.h"
#include "profilecreatorwizard.h"
#include "themecreationwizard.h"
#include "gameadderdialog.h"
#include "titleidfinderdialog.h"
#include "gamerpicturepackdialog.h"
#include "preferencesdialog.h"
#include "githubcommitsdialog.h"
#include "fatxpathgendialog.h"
#include "profilecleanerwizard.h"
#include "svoddialog.h"
#include "ytgrdialog.h"
#include "deviceviewer.h"
#include "devicecontentviewer.h"
#include "isodialog.h"
#include "xexdialog.h"
#include "xuizdialog.h"

// other
#include "PluginInterfaces/igamemodder.h"
#include "PluginInterfaces/igpdmodder.h"
#include "qthelpers.h"
#include "IO/FileIO.h"
#include "json.h"
#include "Stfs/StfsPackage.h"
#include "Gpd/GpdBase.h"
#include "IO/IndexableMultiFileIO.h"
#include "Disc/Svod.h"
#include "AvatarAsset/Ytgr.h"
#include "Disc/Svod.h"
#include "IO/DeviceIO.h"
#include "Fatx/FatxDrive.h"
#include "Disc/ISO.h"

#include "fatxdevicenotifier.h"

using namespace std;

Q_DECLARE_METATYPE( StfsPackage* )

namespace Ui {
class MainWindow;
}

struct Arguments
{
    StfsPackage *package;
    QString tempFilePath;
    bool fromPackageViewer;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QList<QUrl> arguments, QWidget *parent = 0);
    void LoadPlugin(QString filename, bool addToMenu, StfsPackage *package = NULL);
    void LoadAllPlugins();
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *);

private slots:
    void on_actionAbout_triggered();

    void on_actionPackage_triggered();

    void on_actionXDBF_File_triggered();

    void on_actionSTRB_File_triggered();

    void on_actionCreate_Package_triggered();

    void on_actionTitle_ID_Finder_triggered();

    void on_actionProfile_Creator_triggered();

    void on_actionProfile_Editor_triggered();

    void on_actionTheme_Creator_triggered();

    void on_actionGame_Adder_triggered();

    void on_actionGamer_Picture_Pack_Creator_triggered();

    void on_actionModder_triggered();

    void on_actionDonate_triggered();

    void on_actionView_Wiki_triggered();

    void PluginFinished();

    void on_actionPreferences_triggered();

    void on_actionFATX_File_Path_triggered();

    void versionReplyFinished(QNetworkReply *aReply);

    void pluginVersionReplyFinished(QNetworkReply *aReply);

    void on_actionProfile_Cleaner_triggered();

    void on_actionCheck_For_Updates_triggered();

    void on_actionSVOD_System_triggered();

    void on_actionYTGR_triggered();

    void on_actionDevice_Viewer_triggered();

    void on_actionContent_Device_Viewer_triggered();

    void onNewDeviceFound(QList<FatxDrive *> devices);

    void onContentDeviceViewerClosed();

    void onDeviceViewerClosed();

    void on_actionISO_triggered();

    void on_actionXEX_triggered();

    void on_actionXUIZ_triggered();

private:
    Ui::MainWindow *ui;
    QSettings *settings;
    QNetworkAccessManager *manager, *pluginManager;
    FatxDeviceNotifier *deviceNotifier;

    QList<QAction*> gpdActions, gameActions;
    bool firstUpdateCheck;

    void LoadFiles(QList<QUrl> &filePaths);
};

#endif // MAINWINDOW_H

