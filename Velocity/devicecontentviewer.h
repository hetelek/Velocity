#ifndef DEVICECONTENTVIEWER_H
#define DEVICECONTENTVIEWER_H

// forms
#include "profileeditor.h"
#include "packageviewer.h"
#include "svoddialog.h"

// qt
#include <QDialog>
#include <QStatusBar>
#include <QList>
#include <QTreeWidgetItem>
#include <QProgressBar>
#include <QtGlobal>
#include <QInputDialog>
#include <QDebug>
#include <QApplication>

// std
#include <iostream>
#include <vector>

// xbox
#include "Fatx/XContentDevice.h"
#include "Fatx/FatxDriveDetection.h"
#include "Stfs/IXContentHeader.h"
#include "Stfs/StfsPackage.h"

// widgets
#include "dragdroptreewidget.h"

#include "fatxdevicenotifier.h"
#include "profileselectiondialog.h"

namespace Ui {
class DeviceContentViewer;
}

void updateUI(void *arg, bool finished);
void DisplayProgress(void *arg, bool finished);

Q_DECLARE_METATYPE( IXContentHeader* )

enum ItemType
{
    ItemTypeDevice = 0,
    ItemTypeProfile,
    ItemTypeTitle,
    ItemTypeContent,
    ItemTypeSharedItemCategory
};

class DeviceContentViewer : public QDialog
{
    Q_OBJECT
    
public:
    static bool OPEN;

    explicit DeviceContentViewer(QStatusBar *statusBar, QWidget *parent = 0);
    ~DeviceContentViewer();
    
private slots:
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void showContextMenu(const QPoint &pos);
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_btnViewPackage_clicked();
    void onDragDropped(QDropEvent *event);
    void onDragEntered(QDragEnterEvent *event);
    void onDragLeft(QDragLeaveEvent *event);
    void onNewDevicesDetected(QList<FatxDrive*> newDrives);

private:
    Ui::DeviceContentViewer *ui;
    QStatusBar *statusBar;
    QList<XContentDevice*> devices;
    IXContentHeader *currentPackage;
    QProgressBar *progressBar;
    FatxDeviceNotifier *deviceNotifier;
    XContentDevice *currentDevice;

    void resizeEvent(QResizeEvent *);

    void LoadSharedItemCategory(QString category, std::vector<XContentDeviceSharedItem> *items, QTreeWidgetItem *parent, QString iconPath);
    void LoadDevicesp();
    void ClearSidePanel();
    void SetLabelText(QLabel *label, QString text);
    void OpenContent(bool updateButton);
    void CopyFilesToDevice(XContentDevice *device, QStringList files);
    void UpdateDevicePanel();
    void CleanupEmptyItems(QTreeWidgetItem *leafItem);

    friend void DisplayProgress(void *arg, bool finished);

    void UpdateUI();
};

#endif // DEVICECONTENTVIEWER_H
