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
#include "qthelpers.h"

// forms
#include "multiprogressdialog.h"
#include "clustertooldialog.h"
#include "fatxfiledialog.h"

// xbox
#include "Fatx/FatxDrive.h"
#include "Stfs/StfsDefinitions.h"

Q_DECLARE_METATYPE( FatxFileEntry* )
Q_DECLARE_METATYPE( Partition* )

namespace Ui {
class DeviceViewer;
}

class DeviceViewer : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceViewer(QWidget *parent = 0);
    ~DeviceViewer();
    
private slots:
    void on_pushButton_clicked();
    void showRemoveContextMenu(QPoint point);
    void on_treeWidget_doubleClicked(const QModelIndex &index);
    void on_btnBack_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_btnClusterTool_clicked();

private:
    Ui::DeviceViewer *ui;
    FatxDrive *currentDrive;
    QList<FatxFileEntry*> directoryChain;
    int currentIndex;

    void LoadFolder(FatxFileEntry *folder);

    void LoadPartitions();
};

#endif // DEVICEVIEWER_H
