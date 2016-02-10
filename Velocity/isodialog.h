#ifndef ISODIALOG_H
#define ISODIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QMenu>
#include <QStatusBar>
#include "qthelpers.h"

#include "singleprogressdialog.h"
#include "packageviewer.h"
#include "isosectordialog.h"

#include "Disc/ISO.h"
#include "IO/IsoIO.h"
#include "Stfs/StfsPackage.h"

namespace Ui {
class ISODialog;
}

enum IsoTreeWidgetItemData
{
    IsoTreeWidgetItemDataPathInISO = 0,
    IsoTreeWidgetItemDataMagic = 1,
    IsoTreeWidgetItemIsDirectory = 2
};

class ISODialog : public QDialog
{
    Q_OBJECT

public:
    explicit ISODialog(ISO *iso, QStatusBar *statusBar, QWidget *parent = 0);
    ~ISODialog();

private slots:
    void on_pushButton_clicked();

    void showContextMenu(QPoint point);

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_btnSectorTool_clicked();

private:
    Ui::ISODialog *ui;
    ISO *iso;
    QStatusBar *statusBar;

    void LoadISO();

    void LoadDirectory(QObject *parent, std::vector<GdfxFileEntry> directoryContents, bool root=false);

    DWORD GetISOFileMagic(QString pathInIso);
};

#endif // ISODIALOG_H
