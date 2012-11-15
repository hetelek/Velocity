#ifndef XDBFDIALOG_H
#define XDBFDIALOG_H

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
#include "GPD/XDBF.h"
#include "GPD/XDBFDefininitions.h"
#include "GPD/XDBFHelpers.h"
#include "GPD/GPDBase.h"
#include "FileIO.h"

// forms
#include "imagedialog.h"
#include "addressconverterdialog.h"

namespace Ui {
class XdbfDialog;
}

struct Entry
{
    EntryType type;
    int index;
};

class XdbfDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit XdbfDialog(QStatusBar *statusBar, GPDBase *gpd, bool *modified = NULL, QWidget *parent = 0);
    ~XdbfDialog();
    
private slots:
    void on_treeWidget_doubleClicked(const QModelIndex &index);

    void showContextMenu(QPoint p);

private:
    Ui::XdbfDialog *ui;
    GPDBase *gpd;
    bool *modified;
    QStatusBar *statusBar;

    void addEntriesToTable(QVector<XDBFEntry> entries, QString type);

    Entry indexToEntry(int index);

    void loadEntries();
};

#endif // XDBFDIALOG_H
