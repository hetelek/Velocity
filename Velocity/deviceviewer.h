#ifndef DEVICEVIEWER_H
#define DEVICEVIEWER_H

// qt
#include <QDialog>
#include <QTreeWidget>
#include <QFileDialog>
#include <QDesktopServices>
#include <QMenu>
#include <QMessageBox>
#include <QDateTime>
#include <QUrl>
#include <QStatusBar>
#include <QInputDialog>
#include <QProgressBar>
#include <QPixmap>
#include <QAction>
#include "qthelpers.h"

// forms
#include "multiprogressdialog.h"
#include "clustertooldialog.h"
#include "securitysectordialog.h"
#include "fatxfiledialog.h"
#include "partitiondialog.h"
#include "singleprogressdialog.h"
#include "packageviewer.h"
#include "flashdriveconfigdatadialog.h"
#include "svoddialog.h"

// xbox
#include "Fatx/FatxDriveDetection.h"
#include "Fatx/FatxDrive.h"
#include "Stfs/StfsDefinitions.h"

// widgets
#include "nightcharts.h"
#include "dragdroptreewidget.h"

Q_DECLARE_METATYPE( FatxFileEntry* )
Q_DECLARE_METATYPE( FatxDrive* )
Q_DECLARE_METATYPE( Partition* )

namespace Ui {
class DeviceViewer;
}

void updateUI(void *arg, bool finished);
void updateUIDelete(void *arg);

class DeviceViewer : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceViewer(QStatusBar *statusBar, QList<QAction *> gpdActions, QList<QAction *> gameActions, QWidget *parent = 0);

    void LoadDrives();
    ~DeviceViewer();

private slots:
    void showContextMenu(QPoint point);
    void on_treeWidget_doubleClicked(const QModelIndex &index);
    void on_btnBack_clicked();
    void on_treeWidget_2_itemExpanded(QTreeWidgetItem *item);
    void on_treeWidget_2_itemClicked(QTreeWidgetItem *item, int column);
    void on_btnSecurityBlob_clicked();
    void on_btnPartitions_clicked();
    void onDragDropped(QDropEvent *event);
    void onDragEntered(QDragEnterEvent *event);
    void onDragLeft(QDragLeaveEvent *event);
    void on_btnBackup_clicked();
    void on_btnRestore_clicked();
    void on_txtSearch_textChanged(const QString &arg1);
    void on_btnShowAll_clicked();
    void on_txtPath_returnPressed();
    void on_txtDriveName_editingFinished();

private:
    Ui::DeviceViewer *ui;
    std::vector<FatxDrive*> loadedDrives;
    FatxDrive *currentDrive;
    FatxFileEntry *parentEntry;
    QList<FatxFileEntry*> directoryChain;
    QList<QAction *> gpdActions, gameActions;
    QStatusBar *statusBar;
    QProgressBar *progressBar;
    QTreeWidgetItem *currentDriveItem;
    QString previousName;
    bool drivesLoaded;

    void LoadFolderAll(FatxFileEntry *folder);
    void LoadFolderTree(QTreeWidgetItem *item);
    void LoadPartitions();
    void GetSubFilesFATX(FatxFileEntry *parent, QList<void*> &entries);
    void GetSubFilesLocal(QString parent, QList<void *> &files);
    FatxFileEntry* GetFatxFileEntry(QTreeWidgetItem *item);
    void DrawMemoryGraph();
    void InjectFiles(QList<void *> files, QString rootPath);
    void DrawHeader(QString driveName);
    void SetWidgetsEnabled(bool enabled);

    friend void updateUI(void *arg, bool finished);
    friend void updateUIDelete(void *arg);
};

#endif // DEVICEVIEWER_H
