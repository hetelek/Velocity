#ifndef DEVICEVIEWER_H
#define DEVICEVIEWER_H

// qt
#include <QDialog>
#include <QTreeWidget>
#include <QFileDialog>
#include <QDesktopServices>
#include <QMenu>

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
    void on_treeWidget_expanded(const QModelIndex &index);
    void showRemoveContextMenu(QPoint point);

private:
    Ui::DeviceViewer *ui;
    FatxDrive *currentDrive;
};

#endif // DEVICEVIEWER_H
