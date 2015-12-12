#ifndef SVODDIALOG_H
#define SVODDIALOG_H

// qt
#include <QDialog>
#include <QTreeWidgetItem>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QUuid>
#include "qthelpers.h"

// xbox
#include "Disc/Svod.h"
#include "IO/SvodIO.h"
#include "Stfs/StfsPackage.h"

// forms
#include "svodfileinfodialog.h"
#include "svodtooldialog.h"
#include "multiprogressdialog.h"
#include "imagedialog.h"
#include "metadata.h"
#include "packageviewer.h"
#include "singleprogressdialog.h"

namespace Ui {
class SvodDialog;
}

Q_DECLARE_METATYPE( GdfxFileEntry* )

class SvodDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SvodDialog(SVOD *svod, QStatusBar *statusBar, QWidget *parent = 0, bool preserveSvod = false);
    ~SvodDialog();
    
private slots:
    void showFileContextMenu(QPoint pos);

    void on_btnViewAll_clicked();

    void on_pushButton_clicked();

    void on_txtSearch_textChanged(const QString &arg1);

    void on_btnShowAll_clicked();

    void on_pushButton_3_clicked();

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_btnResign_clicked();

private:
    Ui::SvodDialog *ui;
    SVOD *svod;
    QStatusBar *statusBar;
    bool preserveSvod;

    void loadListing(QTreeWidgetItem *parent, vector<GdfxFileEntry> *files);

    friend void UpdateProgress(DWORD cur, DWORD total, void *arg);
};

void UpdateProgress(DWORD cur, DWORD total, void *arg);

#endif // SVODDIALOG_H
