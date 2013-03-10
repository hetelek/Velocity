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
#include "qthelpers.h"

// forms
#include "multiprogressdialog.h"
#include "clustertooldialog.h"
#include "securitysectordialog.h"
#include "fatxfiledialog.h"
#include "partitiondialog.h"
#include "singleprogressdialog.h"

// xbox
#include "Fatx/FatxDrive.h"
#include "Stfs/StfsDefinitions.h"

// widgets
#include "nightcharts.h"
#include "dragdroptreewidget.h"

Q_DECLARE_METATYPE( FatxFileEntry* )
Q_DECLARE_METATYPE( Partition* )

namespace Ui {
class DeviceViewer;
}

class DeviceViewer : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceViewer(QStatusBar *statusBar, QWidget *parent = 0);
    ~DeviceViewer();

private slots:
    void on_pushButton_clicked();
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

private:
    Ui::DeviceViewer *ui;
    FatxDrive *currentDrive;
    FatxFileEntry *parentEntry;
    QList<FatxFileEntry*> directoryChain;
    QStatusBar *statusBar;

    void LoadFolderAll(FatxFileEntry *folder);

    void LoadFolderTree(QTreeWidgetItem *item);

    void LoadPartitions();

    void GetSubFiles(FatxFileEntry *parent, QList<void*> &entries);

    FatxFileEntry* GetFatxFileEntry(QTreeWidgetItem *item);

    void DrawMemoryGraph();

    void InjectFiles(QStringList files);
};

#endif // DEVICEVIEWER_H
