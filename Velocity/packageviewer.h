#ifndef PACKAGEVIEWER_H
#define PACKAGEVIEWER_H

// forms
#include "mainwindow.h"
#include "metadata.h"
#include "xdbfdialog.h"
#include "strbdialog.h"
#include "profileeditor.h"
#include "renamedialog.h"
#include "certificatedialog.h"

// qt
#include <QDialog>
#include <QTreeWidgetItem>
#include <QPixmap>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QUuid>

// xbox libs
#include "Stfs/StfsPackage.h"
#include "GPD/GPDBase.h"
#include "AvatarAsset/AvatarAsset.h"

// other
#include <stdio.h>
#include "FileIO.h"

Q_DECLARE_METATYPE( FileEntry* )

namespace Ui {
class PackageViewer;
}

class PackageViewer : public QDialog
{
    Q_OBJECT
    
public:
    explicit PackageViewer(StfsPackage *package, QWidget *parent = 0);
    ~PackageViewer();
    
private slots:
    void on_btnFix_clicked();

    void on_btnViewAll_clicked();

    void showRemoveContextMenu(QPoint point);

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_btnProfileEditor_clicked();

private:
    Ui::PackageViewer *ui;
    void PopulateTreeWidget(FileListing *entry, QTreeWidgetItem *parent = NULL);
    void GetPackagePath(QTreeWidgetItem *item, QString *out, bool folderOnly = false);
    StfsPackage *package;
    bool disposePackage;
    FileListing listing;
    QWidget *parent;
};

#endif // PACKAGEVIEWER_H
