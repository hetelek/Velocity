#ifndef XdbfDIALOG_H
#define XdbfDIALOG_H

// qt
#include <QDialog>
#include <QModelIndex>
#include <QMessageBox>
#include <QDateTime>
#include <QMenu>
#include <QFile>
#include <QFileDialog>
#include <QStatusBar>
#include "qthelpers.h"

// xbox360
#include "Gpd/Xdbf.h"
#include "Gpd/XdbfDefininitions.h"
#include "Gpd/XdbfHelpers.h"
#include "Gpd/GpdBase.h"
#include "IO/FileIO.h"

// forms
#include "imagedialog.h"
#include "addressconverterdialog.h"

namespace Ui {
class XdbfDialog;
}

struct Entry
{
    EntryType type;
    DWORD index;
};

class XdbfDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit XdbfDialog(QStatusBar *statusBar, GpdBase *gpd, bool *modified = NULL, QWidget *parent = 0);
    ~XdbfDialog();
    
private slots:
    void on_treeWidget_doubleClicked(const QModelIndex &index);

    void showContextMenu(QPoint p);

private:
    Ui::XdbfDialog *ui;
    GpdBase *gpd;
    bool *modified;
    QStatusBar *statusBar;

    void addEntriesToTable(vector<XdbfEntry> entries, QString type);

    Entry indexToEntry(DWORD index);

    void loadEntries();
};

#endif // XdbfDIALOG_H
